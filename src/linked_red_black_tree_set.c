#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <seagrass.h>
#include <rock.h>
#include <coral.h>

#include "private/stack.h"

#ifdef TEST
#include <test/cmocka.h>
#endif

struct entry {
    struct rock_red_black_tree_node rbt_node;
    struct rock_linked_list_node ll_node;
    unsigned char data[];
};

static _Thread_local const struct coral_linked_red_black_tree_set *this;

/* stack is used to defer the copying of key until we know that the
 * red black tree does not contain it */
static _Thread_local struct coral_stack stack = {};

static int entry_compare(const struct rock_red_black_tree_node *const a,
                         const struct rock_red_black_tree_node *const b) {
    void *ptr;
    seagrass_required_true(!coral_stack_peek(&stack, &ptr));
    if (!ptr) {
        const struct entry *const A =
                rock_container_of(a, struct entry, rbt_node);
        ptr = (void *) &A->data;
    }
    const struct entry *const B = rock_container_of(b, struct entry, rbt_node);
    return this->compare(ptr, &B->data);
}

int coral_linked_red_black_tree_set_init(
        struct coral_linked_red_black_tree_set *const object,
        const size_t size,
        int (*compare)(const void *first,
                       const void *second)) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!size) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SIZE_IS_ZERO;
    }
    if (!compare) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_COMPARE_IS_NULL;
    }
    int error;
    uintmax_t alloc;
    if ((error = seagrass_uintmax_t_add(size, sizeof(struct entry), &alloc))
        || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SIZE_IS_TOO_LARGE;
    }
    *object = (struct coral_linked_red_black_tree_set) {0};
    seagrass_required_true(!rock_red_black_tree_init(
            &object->tree, entry_compare));
    object->compare = compare;
    object->size = size;
    object->alloc = alloc;
    return 0;
}

static _Thread_local void (*on_destroy_callback)(void *item);

static void entry_on_destroy(struct rock_red_black_tree_node *const node) {
    struct entry *A = rock_container_of(node, struct entry, rbt_node);
    if (on_destroy_callback) {
        on_destroy_callback(&A->data);
    }
    free(A);
}

int coral_linked_red_black_tree_set_invalidate(
        struct coral_linked_red_black_tree_set *const object,
        void (*const on_destroy)(void *)) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    struct {
        void (*on_destroy_callback)(void *item);
    } saved = {
            .on_destroy_callback = on_destroy_callback
    };
    on_destroy_callback = on_destroy;
    seagrass_required_true(!rock_red_black_tree_invalidate(
            &object->tree, entry_on_destroy));
    on_destroy_callback = saved.on_destroy_callback;
    *object = (struct coral_linked_red_black_tree_set) {0};
    return 0;
}

int coral_linked_red_black_tree_set_size(
        const struct coral_linked_red_black_tree_set *const object,
        size_t *const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    *out = object->size;
    return 0;
}

int coral_linked_red_black_tree_set_count(
        const struct coral_linked_red_black_tree_set *const object,
        uintmax_t *const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    seagrass_required_true(!rock_red_black_tree_count(
            &object->tree, out));
    return 0;
}

/**
 * @brief Find value in linked tree set.
 * @param [in] object linked tree set instance.
 * @param [in] value to be found.
 * @param [out] out node of exact match or insertion point.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND if value was
 * not found.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is insufficient memory to search for value.
 */
static int find(const struct coral_linked_red_black_tree_set *const object,
                const void *const value,
                struct rock_red_black_tree_node **const out) {
    assert(object);
    assert(out);
    this = object;
    const void *ptr = value;
    int error;
    if ((error = coral_stack_push(&stack, &ptr))) {
        seagrass_required_true(CORAL_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                                == error);
        return error;
    }
    if ((error = rock_red_black_tree_find(&object->tree,
                                          NULL,
                                          (void *) 1, /* dummy non-NULL value */
                                          out))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == error);
    }
    seagrass_required_true(!coral_stack_pop(&stack, (void **) &ptr));
    return error;
}

/**
 * @brief Insert entry at insertion point in tree set.
 * @param [in] object tree set instance.
 * @param [in] insertion_point where entry will be inserted.
 * @param [in] entry to be inserted.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is insufficient memory to insert entry.
 */
static int insert(struct coral_linked_red_black_tree_set *const object,
                  struct rock_red_black_tree_node *const insertion_point,
                  struct entry *const entry) {
    assert(object);
    assert(entry);
    this = object;
    void *ptr = NULL;
    int error;
    if ((error = coral_stack_push(&stack, &ptr))) {
        seagrass_required_true(CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                               == error);
        return error;
    }
    seagrass_required_true(!rock_red_black_tree_insert(
            &object->tree, insertion_point, &entry->rbt_node));
    seagrass_required_true(!coral_stack_pop(&stack, (void **) &ptr));
    return 0;
}

static int tree_add(struct coral_linked_red_black_tree_set *const object,
                    const void *const value,
                    struct entry **const out) {
    assert(object);
    assert(value);
    assert(out);
    int error;
    struct rock_red_black_tree_node *insertion_point;
    if (!(error = find(object, value, &insertion_point))) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS;
    } else if (CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
               == error) {
        return error;
    }
    seagrass_required_true(
            CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND
            == error);
    struct entry *entry;
    {
        error = posix_memalign((void **) &entry, sizeof(void *),
                               object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED;;
        }
    }
    struct rock_red_black_tree_node *const rbt_node = &entry->rbt_node;
    struct rock_linked_list_node *const ll_node = &entry->ll_node;
    seagrass_required_true(!rock_red_black_tree_node_init(rbt_node));
    seagrass_required_true(!rock_linked_list_node_init(ll_node));
    memcpy(&entry->data, value, object->size);
    if (!(error = insert(object, insertion_point, entry))) {
        *out = entry;
    } else {
        seagrass_required_true(
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                == error);
        free(entry);
    }
    return error;
}

int coral_linked_red_black_tree_set_add(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct entry *entry;
    if ((error = tree_add(object, value, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    struct rock_linked_list_node *const ll_node = &entry->ll_node;
    if (!object->list) {
        object->list = ll_node;
    } else {
        seagrass_required_true(!rock_linked_list_insert_before(
                object->list, ll_node));
    }
    return 0;
}

int coral_linked_red_black_tree_set_remove(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *rbt_node;
    if ((error = find(object, value, &rbt_node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    seagrass_required_true(!rock_red_black_tree_remove(
            &object->tree, rbt_node));
    struct entry *const A = rock_container_of(rbt_node, struct entry, rbt_node);
    struct rock_linked_list_node *ll_node = &A->ll_node;
    if (ll_node == object->list) {
        object->list = object->tree.count
                       ? ll_node->next
                       : NULL;
    }
    seagrass_required_true(!rock_linked_list_remove(&A->ll_node));
    free(A);
    return 0;
}

int coral_linked_red_black_tree_set_contains(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        bool *const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                *out = false;
                break;
            }
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    } else {
        *out = true;
    }
    return 0;
}

int coral_linked_red_black_tree_set_get(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            }
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return 0;
}

/**
 * @brief Compare key with entry using tree set comparator.
 * @param [in] object tree set instance.
 * @param [in] first to be compared.
 * @param [in] second to be compared.
 * @param [out] out less than 0 if first item is less than second
 * item, 0 if they are equal and greater than 0 if second is greater than first.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is insufficient memory to compare the two items.
 */
static int compare(const struct coral_linked_red_black_tree_set *const object,
                   const void *const first,
                   const void *const second,
                   int *const out) {
    assert(object);
    assert(first);
    assert(second);
    assert(out);
    this = object;
    void *ptr = NULL;
    int error;
    if ((error = coral_stack_push(&stack, &ptr))) {
        seagrass_required_true(
                CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                == error);
        return error;
    }
    *out = object->compare(first, second);
    seagrass_required_true(!coral_stack_pop(&stack, &ptr));
    return 0;
}

int coral_linked_red_black_tree_set_ceiling(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, rbt_node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result > 0) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_floor(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, rbt_node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result < 0) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_higher(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, rbt_node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result > 0) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    } else if ((error = rock_red_black_tree_next(node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_lower(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, rbt_node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result < 0) {
                    return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    } else if ((error = rock_red_black_tree_prev(node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return 0;
}

static int retrieve_tree_fl(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out,
        int (*const func)(const struct rock_red_black_tree *,
                          struct rock_red_black_tree_node **out)) {
    assert(func);
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = func(&object->tree, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == error);
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_lowest(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_tree_fl(object, out, rock_red_black_tree_first);
}

int coral_linked_red_black_tree_set_highest(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_tree_fl(object, out, rock_red_black_tree_last);
}

int coral_linked_red_black_tree_set_first(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    if (!object->list) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY;
    }
    const struct entry *const A
            = rock_container_of(object->list, struct entry, ll_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_last(
        const struct coral_linked_red_black_tree_set *const object,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    if (!object->list) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY;
    }
    const struct entry *const B
            = rock_container_of(object->list, struct entry, ll_node);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_prev(&B->ll_node, &node));
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_remove_item(
        struct coral_linked_red_black_tree_set *const object,
        const void *const item) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    struct entry *const A = rock_container_of(item, struct entry, data);
    seagrass_required_true(!rock_red_black_tree_remove(
            &object->tree, &A->rbt_node));
    seagrass_required_true(!rock_linked_list_remove(
            &A->ll_node));
    free(A);
    return 0;
}

int coral_linked_red_black_tree_set_next(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    const struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_next(&B->ll_node, &node));
    if (node == object->list) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE;
    }
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_prev(
        const struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    const struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_prev(&B->ll_node, &node));
    struct rock_linked_list_node *end;
    seagrass_required_true(!rock_linked_list_prev(node, &end));
    if (end == object->list) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE;
    }
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = &A->data;
    return 0;
}

int coral_linked_red_black_tree_set_insert_after(
        struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void *const value) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct entry *entry;
    if ((error = tree_add(object, value, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *ll_object = &B->ll_node;
    struct rock_linked_list_node *ll_node = &entry->ll_node;
    seagrass_required_true(!rock_linked_list_insert_after(
            ll_object, ll_node));
    return 0;
}

int coral_linked_red_black_tree_set_insert_before(
        struct coral_linked_red_black_tree_set *const object,
        const void *const item,
        const void *const value) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct entry *entry;
    if ((error = tree_add(object, value, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_linked_list_node *ll_object = &B->ll_node;
    struct rock_linked_list_node *ll_node = &entry->ll_node;
    seagrass_required_true(!rock_linked_list_insert_before(
            ll_object, ll_node));
    if (ll_object == object->list) {
        object->list = ll_node;
    }
    return 0;
}

int coral_linked_red_black_tree_set_append(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    const void *out;
    if ((error = coral_linked_red_black_tree_set_last(object, &out))) {
        seagrass_required_true(
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY
                == error);
        if ((error = coral_linked_red_black_tree_set_add(object, value))) {
            seagrass_required_true(
                    CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                    == error);
            return error;
        }
    } else if ((error = coral_linked_red_black_tree_set_insert_after(
            object, out, value))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    return 0;
}

int coral_linked_red_black_tree_set_prepend(
        struct coral_linked_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    const void *out;
    if ((error = coral_linked_red_black_tree_set_first(object, &out))) {
        seagrass_required_true(
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY
                == error);
        if ((error = coral_linked_red_black_tree_set_add(object, value))) {
            seagrass_required_true(
                    CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                    == error);
            return error;
        }
    } else if ((error = coral_linked_red_black_tree_set_insert_before(
            object, out, value))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS:
            case
                CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    return 0;
}
