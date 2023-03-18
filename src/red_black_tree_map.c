#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <seagrass.h>
#include <coral.h>

#include "private/stack.h"

#ifdef TEST
#include <test/cmocka.h>
#endif

struct coral_red_black_tree_map_entry {
    void *data;
};

struct entry {
    struct rock_red_black_tree_node node;
    unsigned char data[];
};

static _Thread_local const struct coral_red_black_tree_map *this = NULL;

/* stack is used to defer the copying of key until we know that the red
 * black tree does not contain it */
static _Thread_local struct coral_stack stack = {};

static int entity_compare(const struct rock_red_black_tree_node *const a,
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

int coral_red_black_tree_map_init(
        struct coral_red_black_tree_map *const object,
        const size_t key,
        const size_t value,
        int (*const compare)(const void *first,
                             const void *second)) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_SIZE_IS_ZERO;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_SIZE_IS_ZERO;
    }
    if (!compare) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_COMPARE_IS_NULL;
    }
    int error;
    uintmax_t entry;
    if ((error = seagrass_uintmax_t_add(key, value, &entry))
        || entry > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE;
    }
    /* calculate padding to align 'value' on void* boundary after 'key' */
    uintmax_t qr[2];
    seagrass_required_true(!seagrass_uintmax_t_divide(
            key, sizeof(void *), &qr[0], &qr[1]));
    const uintmax_t padding = !qr[1] ? 0 : sizeof(void *) - qr[1];
    if ((error = seagrass_uintmax_t_add(padding, entry, &entry))
        || entry > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE;
    }
    uintmax_t alloc;
    if ((error = seagrass_uintmax_t_add(entry, sizeof(struct entry), &alloc))
        || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE;
    }
    *object = (struct coral_red_black_tree_map) {0};
    seagrass_required_true(!rock_red_black_tree_init(
            &object->tree, entity_compare));
    object->compare = compare;
    object->key = key;
    object->padding = padding;
    object->value = value;
    object->alloc = alloc;
    return 0;
}

static _Thread_local void (*on_destroy_callback)(void *key, void *value);

static void entry_on_destroy(struct rock_red_black_tree_node *const node) {
    struct entry *A = rock_container_of(node, struct entry, node);
    if (on_destroy_callback) {
        void *key = &A->data;
        void *value = ((char *) &A->data) + this->key + this->padding;
        on_destroy_callback(key, value);
    }
    free(A);
}

int coral_red_black_tree_map_invalidate(
        struct coral_red_black_tree_map *const object,
        void (*const on_destroy)(void *key,
                                 void *value)) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    struct {
        void (*on_destroy_callback)(void *key, void *value);
        const struct coral_red_black_tree_map *this;
    } saved = {
            .on_destroy_callback = on_destroy_callback,
            .this = this
    };
    this = object; /* needed for 'entry_on_destroy' */
    on_destroy_callback = on_destroy;
    seagrass_required_true(!rock_red_black_tree_invalidate(
            &object->tree, entry_on_destroy));
    this = saved.this;
    on_destroy_callback = saved.on_destroy_callback;
    *object = (struct coral_red_black_tree_map) {0};
    return 0;
}

int coral_red_black_tree_map_key_size(
        const struct coral_red_black_tree_map *const object,
        size_t *const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    *out = object->key;
    return 0;
}

int coral_red_black_tree_map_value_size(
        const struct coral_red_black_tree_map *const object,
        size_t *const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    *out = object->value;
    return 0;
}

int coral_red_black_tree_map_count(
        const struct coral_red_black_tree_map *const object,
        uintmax_t *const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    seagrass_required_true(!rock_red_black_tree_count(
            &object->tree, out));
    return 0;
}

/**
 * @brief Find key in tree map.
 * @param [in] object tree map instance.
 * @param [in] key to be found.
 * @param [out] out node of exact match or insertion point.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key was not found.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to search for key.
 */
static int find(const struct coral_red_black_tree_map *const object,
                 const void *const key,
                 struct rock_red_black_tree_node **const out) {
    assert(object);
    assert(out);
    int error;
    this = object;
    const void *ptr = key;
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
 * @brief Insert entry at insertion point in tree map.
 * @param [in] object tree map instance.
 * @param [in] insertion_point where entry will be inserted.
 * @param [in] entry to be inserted.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to insert entry.
 */
static int insert(struct coral_red_black_tree_map *const object,
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

int coral_red_black_tree_map_add(
        struct coral_red_black_tree_map *const object,
        const void *const key,
        const void *const value) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *insertion_point;
    if (!(error = find(object, key, &insertion_point))) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_ALREADY_EXISTS;
    } else if (CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
               == error) {
        return error;
    }
    seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND
                           == error);
    struct entry *entry;
    {
        error = posix_memalign((void **) &entry, sizeof(void *), object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            return CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED;
        }
    }
    seagrass_required_true(!rock_red_black_tree_node_init(&entry->node));
    memcpy(&entry->data, key, object->key);
    void *const dest = ((char *) &entry->data) + object->key + object->padding;
    memcpy(dest, value, object->value);
    if ((error = insert(object, insertion_point, entry))) {
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
                == error);
        free(entry);
    }
    return error;
}

int coral_red_black_tree_map_remove(
        struct coral_red_black_tree_map *const object,
        const void *const key) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
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

int coral_red_black_tree_map_contains(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        bool *const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                *out = false;
                break;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    } else {
        *out = true;
    }
    return 0;
}

int coral_red_black_tree_map_set(
        struct coral_red_black_tree_map *const object,
        const void *const key,
        const void *const value) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    seagrass_required_true(!coral_red_black_tree_map_entry_set_value(
            object,(struct coral_red_black_tree_map_entry *) &A->data, value));
    return 0;
}

int coral_red_black_tree_map_get(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    const struct coral_red_black_tree_map_entry *entry;
    if ((error = coral_red_black_tree_map_get_entry(object, key, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    seagrass_required_true(!coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return 0;
}

int coral_red_black_tree_map_ceiling(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    const struct coral_red_black_tree_map_entry *entry;
    if ((error = coral_red_black_tree_map_ceiling_entry(object, key, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    seagrass_required_true(!coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return 0;
}

int coral_red_black_tree_map_floor(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    const struct coral_red_black_tree_map_entry *entry;
    if ((error = coral_red_black_tree_map_floor_entry(object, key, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    seagrass_required_true(!coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return 0;
}

int coral_red_black_tree_map_higher(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    const struct coral_red_black_tree_map_entry *entry;
    if ((error = coral_red_black_tree_map_higher_entry(object, key, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    seagrass_required_true(!coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return 0;
}

int coral_red_black_tree_map_lower(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    const struct coral_red_black_tree_map_entry *entry;
    if ((error = coral_red_black_tree_map_lower_entry(object, key, &entry))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    seagrass_required_true(!coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return 0;
}

int coral_red_black_tree_map_first(
        const struct coral_red_black_tree_map *const object,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    const struct coral_red_black_tree_map_entry *entry;
    if ((error = coral_red_black_tree_map_first_entry(object, &entry))) {
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY
                               == error);
        return error;
    }
    seagrass_required_true(!coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return 0;
}

int coral_red_black_tree_map_last(
        const struct coral_red_black_tree_map *const object,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    const struct coral_red_black_tree_map_entry *entry;
    if ((error = coral_red_black_tree_map_last_entry(object, &entry))) {
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY
                               == error);
        return error;
    }
    seagrass_required_true(!coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return 0;
}

int coral_red_black_tree_map_get_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

/**
 * @brief Compare key with entry using tree map comparator.
 * @param [in] object tree map instance.
 * @param [in] first to be compared
 * @param [in] second to be compared
 * @param [out] out less than 0 if first item is less than second
 * item, 0 if they are equal and greater than 0 if second is greater than first.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to compare the two items.
 */
static int compare(const struct coral_red_black_tree_map *const object,
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
        seagrass_required_true(CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                               == error);
        return error;
    }
    *out = object->compare(first, second);
    seagrass_required_true(!coral_stack_pop(&stack, &ptr));
    return 0;
}

int coral_red_black_tree_map_ceiling_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return error;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, key, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result > 0) {
                    return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_floor_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return error;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, key, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result < 0) {
                    return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_higher_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return error;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, key, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result > 0) {
                    return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                }
            }
        }
    } else if ((error = rock_red_black_tree_next(node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_lower_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = find(object, key, &node))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return error;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return error;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if ((error = compare(object, key, &B->data, &result))) {
                    seagrass_required_true(
                            CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
                            == error);
                    return error;
                }
                if (result < 0) {
                    return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                }
            }
        }
    } else if ((error = rock_red_black_tree_prev(node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_first_entry(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = rock_red_black_tree_first(&object->tree, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == error);
        return error;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_last_entry(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    if ((error = rock_red_black_tree_last(&object->tree, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == error);
        return error;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_remove_entry(
        struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
    }
    struct entry *const A = rock_container_of(entry, struct entry, data);
    seagrass_required_true(!rock_red_black_tree_remove(
            &object->tree, &A->node));
    free(A);
    return 0;
}

int coral_red_black_tree_map_next_entry(
        const struct coral_red_black_tree_map_entry *const entry,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!entry) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    struct entry *const B = rock_container_of(entry, struct entry, data);
    if ((error = rock_red_black_tree_next(&B->node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return error;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_prev_entry(
        const struct coral_red_black_tree_map_entry *const entry,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!entry) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    int error;
    struct rock_red_black_tree_node *node;
    struct entry *const B = rock_container_of(entry, struct entry, data);
    if ((error = rock_red_black_tree_prev(&B->node, &node))) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == error);
        return error;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return 0;
}

int coral_red_black_tree_map_entry_key(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    *out = entry; /* address of entry is the address of the key in entry */
    return 0;
}

int coral_red_black_tree_map_entry_get_value(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry,
        const void **const out) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
    }
    if (!out) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
    }
    *out = ((char *) entry) + object->key + object->padding;
    return 0;
}

int coral_red_black_tree_map_entry_set_value(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry,
        const void *const value) {
    if (!object) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
    }
    if (!value) {
        return CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL;
    }
    void *const dest = ((char *) entry) + object->key + object->padding;
    memcpy(dest, value, object->value);
    return 0;
}
