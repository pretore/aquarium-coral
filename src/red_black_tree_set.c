#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <seagrass.h>
#include <coral.h>

#ifdef TEST
#include <test/cmocka.h>
#endif

struct entry {
    struct rock_red_black_tree_node node;
    unsigned char data[];
};

static _Thread_local const struct coral_red_black_tree_set *this;
static _Thread_local struct coral_linked_stack stack;

static int entry_compare(const struct rock_red_black_tree_node *const a,
                         const struct rock_red_black_tree_node *const b) {
    void *ptr;
    seagrass_required_true(coral_linked_stack_peek(
            &stack, &ptr));
    if (!ptr) {
        const struct entry *const A = rock_container_of(a, struct entry, node);
        ptr = (void *) &A->data;
    }
    const struct entry *const B = rock_container_of(b, struct entry, node);
    return this->compare(ptr, &B->data);
}

bool coral_red_black_tree_set_init(
        struct coral_red_black_tree_set *const object,
        const size_t size,
        int (*compare)(const void *first,
                       const void *second)) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!size) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_SIZE_IS_ZERO;
        return false;
    }
    if (!compare) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_COMPARE_IS_NULL;
        return false;
    }
    uintmax_t alloc;
    bool result = seagrass_uintmax_t_add(
            size, sizeof(struct entry), &alloc);
    if (!result || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == seagrass_error || result);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_SIZE_IS_TOO_LARGE;
        return false;
    }
    /* stack is used to defer the copying of key until we know that the red
     * black tree does not contain it */
    if (!stack.size) {
        seagrass_required_true(coral_linked_stack_init(
                &stack, sizeof(void *)));
    }
    *object = (struct coral_red_black_tree_set) {0};
    seagrass_required_true(rock_red_black_tree_init(
            &object->tree, entry_compare));
    object->compare = compare;
    object->size = size;
    object->alloc = alloc;
    return true;
}

static _Thread_local void (*on_destroy_callback)(void *item);

static void entry_on_destroy(struct rock_red_black_tree_node *const node) {
    struct entry *A = rock_container_of(node, struct entry, node);
    if (on_destroy_callback) {
        on_destroy_callback(&A->data);
    }
    free(A);
}

bool coral_red_black_tree_set_invalidate(
        struct coral_red_black_tree_set *const object,
        void (*const on_destroy)(void *)) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    struct {
        void (*on_destroy_callback)(void *item);
    } saved = {
            .on_destroy_callback = on_destroy_callback
    };
    on_destroy_callback = on_destroy;
    seagrass_required_true(rock_red_black_tree_invalidate(
            &object->tree, entry_on_destroy));
    on_destroy_callback = saved.on_destroy_callback;
    *object = (struct coral_red_black_tree_set) {0};
    return true;
}

bool coral_red_black_tree_set_size(
        const struct coral_red_black_tree_set *const object,
        size_t *const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->size;
    return true;
}

bool coral_red_black_tree_set_count(
        const struct coral_red_black_tree_set *const object,
        uintmax_t *const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    seagrass_required_true(rock_red_black_tree_count(
            &object->tree, out));
    return true;
}

/**
 * @brief Find value in tree set.
 * @param [in] object tree set instance.
 * @param [in] value to be found.
 * @param [out] out node of exact match or insertion point.
 * @return If exact match found true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND if value was not
 * found.
 * @throws CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to search for value.
 */
static bool find(const struct coral_red_black_tree_set *const object,
                 const void *const value,
                 struct rock_red_black_tree_node **out) {
    assert(object);
    assert(out);
    this = object;
    const void *ptr = value;
    if (!coral_linked_stack_push(&stack, &ptr)) {
        seagrass_required_true(CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                               == coral_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    const bool result = rock_red_black_tree_find(
            &object->tree,
            NULL,
            (void *) 1, /* dummy non-NULL value */
            out);
    if (!result) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND;
    }
    seagrass_required_true(coral_linked_stack_pop(
            &stack, (void **) &ptr));
    return result;
}

/**
 * @brief Insert entry at insertion point in tree set.
 * @param [in] object tree set instance.
 * @param [in] insertion_point where entry will be inserted.
 * @param [in] entry to be inserted.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to insert entry.
 */
static bool insert(struct coral_red_black_tree_set *const object,
                   struct rock_red_black_tree_node *const insertion_point,
                   struct entry *entry) {
    assert(object);
    assert(entry);
    this = object;
    void *ptr = NULL;
    if (!coral_linked_stack_push(&stack, &ptr)) {
        seagrass_required_true(
                CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                == coral_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    seagrass_required_true(rock_red_black_tree_insert(
            &object->tree, insertion_point, &entry->node));
    seagrass_required_true(coral_linked_stack_pop(
            &stack, (void **) &ptr));
    return true;
}

bool coral_red_black_tree_set_add(
        struct coral_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *insertion_point;
    if (find(object, value, &insertion_point)) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS;
        return false;
    } else if (CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
               == coral_error) {
        return false;
    }
    seagrass_required_true(CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND
                           == coral_error);
    struct entry *entry;
    {
        const int error = posix_memalign((void **) &entry, sizeof(void *),
                                         object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            coral_error =
                    CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED;
            return false;
        }
    }
    struct rock_red_black_tree_node *const node = &entry->node;
    seagrass_required_true(rock_red_black_tree_node_init(node));
    memcpy(&entry->data, value, object->size);
    const bool result = insert(object, insertion_point, entry);
    if (!result) {
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED
                == coral_error);
        free(entry);
    }
    return result;
}

bool coral_red_black_tree_set_remove(
        struct coral_red_black_tree_set *const object,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, value, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    seagrass_required_true(rock_red_black_tree_remove(
            &object->tree, node));
    struct entry *const A = rock_container_of(node, struct entry, node);
    free(A);
    return true;
}

bool coral_red_black_tree_set_contains(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        bool *const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    *out = find(object, value, &node);
    if (!*out) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
    }
    return *out
           || CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND == coral_error;
}

bool coral_red_black_tree_set_get(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, value, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                break;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}

/**
 * @brief Compare key with entry using tree set comparator.
 * @param [in] object tree set instance.
 * @param [in] first to be compared.
 * @param [in] second to be compared.
 * @param [out] out less than 0 if first item is less than second
 * item, 0 if they are equal and greater than 0 if second is greater than first.
 * @return On success true, otherwise false if an error has occurred.
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
    this = object;
    void *ptr = NULL;
    if (!coral_linked_stack_push(&stack, &ptr)) {
        seagrass_required_true(CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                               == coral_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    *out = object->compare(first, second);
    seagrass_required_true(coral_linked_stack_pop(
            &stack, &ptr));
    return true;
}

bool coral_red_black_tree_set_ceiling(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, value, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, value, &B->data, &result)) {
                    return false;
                }
                if (result > 0) {
                    coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                    return false;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}

bool coral_red_black_tree_set_floor(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, value, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, value, &B->data, &result)) {
                    return false;
                }
                if (result < 0) {
                    coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                    return false;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}

bool coral_red_black_tree_set_higher(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, value, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, value, &B->data, &result)) {
                    return false;
                }
                if (result > 0) {
                    coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                    return false;
                }
            }
        }
    } else if (!rock_red_black_tree_next(node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}

bool coral_red_black_tree_set_lower(
        const struct coral_red_black_tree_set *const object,
        const void *const value,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, value, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND: {
                coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, value, &B->data, &result)) {
                    return false;
                }
                if (result < 0) {
                    coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
                    return false;
                }
            }
        }
    } else if (!rock_red_black_tree_prev(node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}

static bool retrieve_fl(
        const struct coral_red_black_tree_set *const object,
        const void **const out,
        bool (*const func)(const struct rock_red_black_tree *,
                           struct rock_red_black_tree_node **out)) {
    assert(func);
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!func(&object->tree, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}

bool coral_red_black_tree_set_first(
        const struct coral_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_fl(object, out, rock_red_black_tree_first);
}

bool coral_red_black_tree_set_last(
        const struct coral_red_black_tree_set *const object,
        const void **const out) {
    return retrieve_fl(object, out, rock_red_black_tree_last);
}

bool coral_red_black_tree_set_remove_item(
        struct coral_red_black_tree_set *const object,
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
            &object->tree, &A->node));
    free(A);
    return true;
}

bool coral_red_black_tree_set_next(const void *const item,
                                   const void **const out) {
    if (!item) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_next(&B->node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}

bool coral_red_black_tree_set_prev(const void *const item,
                                   const void **const out) {
    if (!item) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL;
        return false;
    }
    struct entry *const B = rock_container_of(item, struct entry, data);
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_prev(&B->node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = &A->data;
    return true;
}
