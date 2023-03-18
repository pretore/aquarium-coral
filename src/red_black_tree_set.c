#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <seagrass.h>
#include <coral.h>

#include "private/stack.h"

#ifdef TEST
#include <test/cmocka.h>
#endif

struct entry {
    struct rock_red_black_tree_node node;
    unsigned char data[];
};

static _Thread_local const struct coral_red_black_tree_set *this;

/* stack is used to defer the copying of key until we know that the red
 * black tree does not contain it */
static _Thread_local struct coral_stack stack = {};

static int entry_compare(const struct rock_red_black_tree_node *const a,
                         const struct rock_red_black_tree_node *const b) {
    void *ptr;
    seagrass_required_true(!coral_stack_peek(&stack, &ptr));
    if (!ptr) {
        const struct entry *const A = rock_container_of(a, struct entry, node);
        ptr = (void *) &A->data;
    }
    const struct entry *const B = rock_container_of(b, struct entry, node);
    return this->compare(ptr, &B->data);
}

int coral_red_black_tree_set_init(
        struct coral_red_black_tree_set *const object,
        const size_t size,
        int (*compare)(const void *first,
                       const void *second)) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!size) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_SIZE_IS_ZERO;
    }
    if (!compare) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_COMPARE_IS_NULL;
    }
    int error;
    uintmax_t alloc;
    if ((error = seagrass_uintmax_t_add(size, sizeof(struct entry), &alloc))
        || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_RED_BLACK_TREE_SET_ERROR_SIZE_IS_TOO_LARGE;
    }
    *object = (struct coral_red_black_tree_set) {0};
    seagrass_required_true(!rock_red_black_tree_init(
            &object->tree, entry_compare));
    object->compare = compare;
    object->size = size;
    object->alloc = alloc;
    return 0;
}

static _Thread_local void (*on_destroy_callback)(void *item);

static void entry_on_destroy(struct rock_red_black_tree_node *const node) {
    struct entry *A = rock_container_of(node, struct entry, node);
    if (on_destroy_callback) {
        on_destroy_callback(&A->data);
    }
    free(A);
}

int coral_red_black_tree_set_invalidate(
        struct coral_red_black_tree_set *const object,
        void (*const on_destroy)(void *)) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
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
    *object = (struct coral_red_black_tree_set) {0};
    return 0;
}

int coral_red_black_tree_set_size(
        const struct coral_red_black_tree_set *const object,
        size_t *const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    *out = object->size;
    return 0;
}

int coral_red_black_tree_set_count(
        const struct coral_red_black_tree_set *const object,
        uintmax_t *const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    seagrass_required_true(!rock_red_black_tree_count(
            &object->tree, out));
    return 0;
}

/**
 * @brief Find value in tree set.
 * @param [in] object tree set instance.
 * @param [in] value to be found.
 * @param [out] out node of exact match or insertion point.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND if value was not
 * found.
 * @throws CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to search for value.
 */
static int find(const struct coral_red_black_tree_set *const object,
                const void *const value,
                struct rock_red_black_tree_node **const out) {
    assert(object);
    assert(out);
    int error;
    this = object;
    const void *ptr = value;
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
 * @throws CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to insert entry.
 */
static int insert(struct coral_red_black_tree_set *const object,
                  struct rock_red_black_tree_node *const insertion_point,
                  struct entry *const entry) {
    assert(object);
    assert(entry);
    int error;
    this = object;
    void *ptr = NULL;
    if ((error = coral_stack_push(&stack, &ptr))) {
        seagrass_required_true(CORAL_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                               == error);
        return error;
    }
    seagrass_required_true(!rock_red_black_tree_insert(
            &object->tree, insertion_point, &entry->node));
    seagrass_required_true(!coral_stack_pop(&stack, (void **) &ptr));
    return 0;
}

int coral_red_black_tree_set_add(
        struct coral_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *insertion_point;
    if (!(error = find(object, value, &insertion_point))) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS;
    } else if (CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
               == error) {
        return error;
    }
    seagrass_required_true(CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND
                           == error);
    struct entry *entry;
    {
        error = posix_memalign((void **) &entry, sizeof(void *), object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            return CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED;
        }
    }
    struct rock_red_black_tree_node *const node = &entry->node;
    seagrass_required_true(!rock_red_black_tree_node_init(node));
    memcpy(&entry->data, value, object->size);
    if ((error = insert(object, insertion_point, entry))) {
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                == error);
        free(entry);
    }
    return error;
}

int coral_red_black_tree_set_remove(
        struct coral_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    seagrass_required_true(!rock_red_black_tree_remove(
            &object->tree, node));
    struct entry *const A = rock_container_of(node, struct entry, node);
    free(A);
    return 0;
}

int coral_red_black_tree_set_contains(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        bool *const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                *out = false;
                break;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    } else {
        *out = true;
    }
    return 0;
}

int coral_red_black_tree_set_get(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
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
 * @throws CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to compare the two items.
 */
static bool compare(const struct coral_red_black_tree_set *const object,
                    const void *const first,
                    const void *const second,
                    int *const out) {
    assert(object);
    assert(first);
    assert(second);
    assert(out);
    int error;
    this = object;
    void *ptr = NULL;
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

int coral_red_black_tree_set_ceiling(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result > 0) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return 0;
}

int coral_red_black_tree_set_floor(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result < 0) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return 0;
}

int coral_red_black_tree_set_higher(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result > 0) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    } else if ((error = rock_red_black_tree_next(node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return 0;
}

int coral_red_black_tree_set_lower(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, value, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                if (!node) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, value, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result < 0) {
                    return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                }
            }
        }
    } else if ((error = rock_red_black_tree_prev(node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return 0;
}

static int retrieve_fl(
        const struct coral_red_black_tree_set *const object,
        const void **const out,
        int (*const func)(const struct rock_red_black_tree *,
                          struct rock_red_black_tree_node **out)) {
    assert(func);
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = func(&object->tree, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == error);
        return error;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return 0;
}

int coral_red_black_tree_set_first(
        const struct coral_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_fl(object, out, rock_red_black_tree_first);
}

int coral_red_black_tree_set_last(
        const struct coral_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_fl(object, out, rock_red_black_tree_last);
}

int coral_red_black_tree_set_remove_item(
        struct coral_red_black_tree_set *const object,
        const void *const item) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    struct entry *const A = rock_container_of(item, struct entry, data);
    seagrass_required_true(!rock_red_black_tree_remove(
            &object->tree, &A->node));
    free(A);
    return 0;
}

int coral_red_black_tree_set_next(const void *const item,
                                  const void **const out) {
    if (!item) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_red_black_tree_node *node;
    if ((error = rock_red_black_tree_next(&B->node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return error;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return 0;
}

int coral_red_black_tree_set_prev(const void *const item,
                                  const void **const out) {
    if (!item) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
    }
    int error;
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_red_black_tree_node *node;
    if ((error = rock_red_black_tree_prev(&B->node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return error;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return 0;
}
