#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <seagrass.h>
#include <coral.h>
#include <string.h>

#include "test/cmocka.h"

struct entry {
    struct rock_red_black_tree_node rbt_node;
    struct rock_linked_list_node ll_node;
    unsigned char data[];
};

static _Thread_local const struct coral_linked_red_black_tree_set *this;
static _Thread_local const void *ptr;

static int entry_compare(const struct rock_red_black_tree_node *const a,
                         const struct rock_red_black_tree_node *const b) {
    if (!ptr) {
        const struct entry *const A =
                rock_container_of(a, struct entry, rbt_node);
        ptr = &A->data;
    }
    const struct entry *const B = rock_container_of(b, struct entry, rbt_node);
    return this->compare(ptr, &B->data);
}

bool coral_linked_red_black_tree_set_init(
        struct coral_linked_red_black_tree_set *const object,
        const size_t size,
        int (*compare)(const void *first,
                       const void *second)) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!size) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SIZE_IS_ZERO;
        return false;
    }
    if (!compare) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_COMPARE_IS_NULL;
        return false;
    }
    uintmax_t alloc;
    bool result = seagrass_uintmax_t_add(size, sizeof(struct entry), &alloc);
    if (!result || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == seagrass_error || result);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_SIZE_IS_TOO_LARGE;
        return false;
    }
    *object = (struct coral_linked_red_black_tree_set) {0};
    seagrass_required_true(rock_red_black_tree_init(
            &object->tree, entry_compare));
    object->compare = compare;
    object->size = size;
    object->alloc = alloc;
    return true;
}

static _Thread_local void (*on_destroy_callback)(void *item);

static void entry_on_destroy(struct rock_red_black_tree_node *const node) {
    struct entry *A = rock_container_of(node, struct entry, rbt_node);
    if (on_destroy_callback) {
        on_destroy_callback(&A->data);
    }
    free(A);
}

bool coral_linked_red_black_tree_set_invalidate(
        struct coral_linked_red_black_tree_set *const object,
        void (*const on_destroy)(void *)) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    on_destroy_callback = on_destroy;
    seagrass_required_true(rock_red_black_tree_invalidate(
            &object->tree, entry_on_destroy));
    *object = (struct coral_linked_red_black_tree_set) {0};
    return true;
}

bool coral_linked_red_black_tree_set_size(
        const struct coral_linked_red_black_tree_set *const object,
        size_t *const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->size;
    return true;
}

bool coral_linked_red_black_tree_set_count(
        const struct coral_linked_red_black_tree_set *const object,
        uintmax_t *const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    seagrass_required_true(rock_red_black_tree_count(
            &object->tree, out));
    return true;
}

static bool tree_add(struct coral_linked_red_black_tree_set *const object,
                     const void *const value,
                     struct entry **out) {
    assert(object);
    assert(value);
    struct rock_red_black_tree_node *insertion_point;
    this = object;
    ptr = value; /* delay copy */
    if (rock_red_black_tree_find(&object->tree,
                                 NULL,
                                 (void *) 1, /* dummy non-NULL value */
                                 &insertion_point)) {
        coral_error =
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS;
        return false;
    }
    seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                           == rock_error);
    struct entry *entry;
    {
        const int error = posix_memalign((void **) &entry, sizeof(void *),
                                         object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            coral_error =
                    CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED;
            return false;
        }
    }
    struct rock_red_black_tree_node *const rbt_node = &entry->rbt_node;
    struct rock_linked_list_node *const ll_node = &entry->ll_node;
    seagrass_required_true(rock_red_black_tree_node_init(rbt_node));
    seagrass_required_true(rock_linked_list_node_init(ll_node));
    memcpy(&entry->data, value, object->size);
    ptr = NULL; /* insert the entry into the container */
    seagrass_required_true(rock_red_black_tree_insert(
            &object->tree, insertion_point, rbt_node));
    *out = entry;
    return true;
}

bool coral_linked_red_black_tree_set_add(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    struct entry *entry;
    const bool result = tree_add(object, value, &entry);
    if (!result) {
        switch (coral_error) {
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    struct rock_linked_list_node *const ll_node = &entry->ll_node;
    if (!object->list) {
        object->list = ll_node;
    } else {
        seagrass_required_true(rock_linked_list_insert_before(
                object->list, ll_node));
    }
    return true;
}

bool coral_linked_red_black_tree_set_remove(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    this = object;
    ptr = value; /* avoid unnecessary copying */
    struct rock_red_black_tree_node *rbt_node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &rbt_node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND;
        return false;
    }
    seagrass_required_true(rock_red_black_tree_remove(
            &object->tree, rbt_node));
    struct entry *const A = rock_container_of(rbt_node, struct entry, rbt_node);
    struct rock_linked_list_node *ll_node = &A->ll_node;
    if (ll_node == object->list) {
        object->list = object->tree.count
                       ? ll_node->next
                       : NULL;
    }
    seagrass_required_true(rock_linked_list_remove(&A->ll_node));
    free(A);
    return true;
}

bool coral_linked_red_black_tree_set_contains(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        bool *const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    this = object;
    ptr = value; /* avoid unnecessary copying */
    struct rock_red_black_tree_node *node;
    *out = rock_red_black_tree_find(&object->tree,
                                    NULL,
                                    (void *) 1, /* dummy non-NULL value */
                                    &node);
    if (!*out) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
    }
    return true;
}

bool coral_linked_red_black_tree_set_get(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    this = object;
    ptr = value; /* avoid unnecessary copying */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_ceiling(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    this = object;
    ptr = value; /* avoid unnecessary copying */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, rbt_node);
        if (object->compare(value, &B->data) > 0) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_floor(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    this = object;
    ptr = value; /* avoid unnecessary copying */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, rbt_node);
        if (object->compare(value, &B->data) < 0) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_higher(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    this = object;
    ptr = value; /* avoid unnecessary copying */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, rbt_node);
        if (object->compare(value, &B->data) > 0) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
    } else if (!rock_red_black_tree_next(node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_lower(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    this = object;
    ptr = value; /* avoid unnecessary copying */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, rbt_node);
        if (object->compare(value, &B->data) < 0) {
            coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            return false;
        }
    } else if (!rock_red_black_tree_prev(node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return true;
}

static bool retrieve_tree_fl(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out,
        bool (*const func)(const struct rock_red_black_tree *,
                           struct rock_red_black_tree_node **out)) {
    assert(func);
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!func(&object->tree, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == rock_error);
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_lowest(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_tree_fl(object, out, rock_red_black_tree_first);
}

bool coral_linked_red_black_tree_set_highest(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_tree_fl(object, out, rock_red_black_tree_last);
}

bool coral_linked_red_black_tree_set_first(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    if (!object->list) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY;
        return false;
    }
    const struct entry *const A
            = rock_container_of(object->list, struct entry, ll_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_last(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    if (!object->list) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY;
        return false;
    }
    const struct entry *const B
            = rock_container_of(object->list, struct entry, ll_node);
    struct rock_linked_list_node *node;
    seagrass_required_true(rock_linked_list_prev(&B->ll_node, &node));
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_remove_item(
        struct coral_linked_red_black_tree_set *const object,
        const void *const item) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
        return false;
    }
    struct entry *const A = rock_container_of(item, struct entry, data);
    seagrass_required_true(rock_red_black_tree_remove(
            &object->tree, &A->rbt_node));
    seagrass_required_true(rock_linked_list_remove(
            &A->ll_node));
    free(A);
    return true;
}

bool coral_linked_red_black_tree_set_next(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *node;
    seagrass_required_true(rock_linked_list_next(&B->ll_node, &node));
    if (node == object->list) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE;
        return false;
    }
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_prev(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *node;
    seagrass_required_true(rock_linked_list_prev(&B->ll_node, &node));
    struct rock_linked_list_node *end;
    seagrass_required_true(rock_linked_list_prev(node, &end));
    if (end == object->list) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE;
        return false;
    }
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = &A->data;
    return true;
}

bool coral_linked_red_black_tree_set_insert_after(
        struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    struct entry *entry;
    const bool result = tree_add(object, value, &entry);
    if (!result) {
        switch (coral_error) {
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *ll_object = &B->ll_node;
    struct rock_linked_list_node *ll_node = &entry->ll_node;
    seagrass_required_true(rock_linked_list_insert_after(
            ll_object, ll_node));
    return true;
}

bool coral_linked_red_black_tree_set_insert_before(
        struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    struct entry *entry;
    const bool result = tree_add(object, value, &entry);
    if (!result) {
        switch (coral_error) {
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *ll_object = &B->ll_node;
    struct rock_linked_list_node *ll_node = &entry->ll_node;
    seagrass_required_true(rock_linked_list_insert_before(
            ll_object, ll_node));
    if (ll_object == object->list) {
        object->list = ll_node;
    }
    return true;
}

bool coral_linked_red_black_tree_set_append(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    const void *out;
    if (!coral_linked_red_black_tree_set_last(object, &out)) {
        seagrass_required_true(
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY
                == coral_error);
        if (!coral_linked_red_black_tree_set_add(object, value)) {
            seagrass_required_true(
                    CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                    == coral_error);
            return false;
        }
    } else if (!coral_linked_red_black_tree_set_insert_after(
            object, out, value)) {
        switch (coral_error) {
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    return true;
}

bool coral_linked_red_black_tree_set_prepend(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    const void *out;
    if (!coral_linked_red_black_tree_set_first(object, &out)) {
        seagrass_required_true(
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY
                == coral_error);
        if (!coral_linked_red_black_tree_set_add(object, value)) {
            seagrass_required_true(
                    CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                    == coral_error);
            return false;
        }
    } else if (!coral_linked_red_black_tree_set_insert_before(
            object, out, value)) {
        switch (coral_error) {
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    return true;
}
