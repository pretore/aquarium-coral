#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <seagrass.h>
#include <coral.h>

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
static _Thread_local struct coral_linked_stack stack;

static int entity_compare(const struct rock_red_black_tree_node *const a,
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

bool coral_red_black_tree_map_init(
        struct coral_red_black_tree_map *const object,
        const size_t key,
        const size_t value,
        int (*const compare)(const void *first,
                             const void *second)) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_SIZE_IS_ZERO;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_SIZE_IS_ZERO;
        return false;
    }
    if (!compare) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_COMPARE_IS_NULL;
        return false;
    }
    uintmax_t entry;
    bool result = seagrass_uintmax_t_add(key, value, &entry);
    if (!result || entry > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == seagrass_error || result);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE;
        return false;
    }
    /* calculate padding to align 'value' on void* boundary after 'key' */
    uintmax_t qr[2];
    seagrass_required_true(seagrass_uintmax_t_divide(
            key, sizeof(void *), &qr[0], &qr[1]));
    const uintmax_t padding = !qr[1] ? 0 : sizeof(void *) - qr[1];
    result = seagrass_uintmax_t_add(padding, entry, &entry);
    if (!result || entry > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == seagrass_error || result);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE;
        return false;
    }
    uintmax_t alloc;
    result = seagrass_uintmax_t_add(entry, sizeof(struct entry), &alloc);
    if (!result || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == seagrass_error || result);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE;
        return false;
    }
    /* stack is used to defer the copying of key until we know that the red
     * black tree does not contain it */
    if (!stack.size) {
        seagrass_required_true(coral_linked_stack_init(
                &stack, sizeof(void *)));
    }
    *object = (struct coral_red_black_tree_map) {0};
    seagrass_required_true(rock_red_black_tree_init(
            &object->tree, entity_compare));
    object->compare = compare;
    object->key = key;
    object->padding = padding;
    object->value = value;
    object->alloc = alloc;
    return true;
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

bool coral_red_black_tree_map_invalidate(
        struct coral_red_black_tree_map *const object,
        void (*const on_destroy)(void *key,
                                 void *value)) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
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
    seagrass_required_true(rock_red_black_tree_invalidate(
            &object->tree, entry_on_destroy));
    this = saved.this;
    on_destroy_callback = saved.on_destroy_callback;
    *object = (struct coral_red_black_tree_map) {0};
    return true;
}

bool coral_red_black_tree_map_key_size(
        const struct coral_red_black_tree_map *const object,
        size_t *const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->key;
    return true;
}

bool coral_red_black_tree_map_value_size(
        const struct coral_red_black_tree_map *const object,
        size_t *const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->value;
    return true;
}

bool coral_red_black_tree_map_count(
        const struct coral_red_black_tree_map *const object,
        uintmax_t *const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    seagrass_required_true(rock_red_black_tree_count(
            &object->tree, out));
    return true;
}

/**
 * @brief Find key in tree map.
 * @param [in] object tree map instance.
 * @param [in] key to be found.
 * @param [out] out node of exact match or insertion point.
 * @return If exact match found true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key was not found.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to search for key.
 */
static bool find(const struct coral_red_black_tree_map *const object,
                 const void *const key,
                 struct rock_red_black_tree_node **out) {
    assert(object);
    assert(out);
    this = object;
    const void *ptr = key;
    if (!coral_linked_stack_push(&stack, &ptr)) {
        seagrass_required_true(
                CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                == coral_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED;
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
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
    }
    seagrass_required_true(coral_linked_stack_pop(
            &stack, (void **) &ptr));
    return result;
}

/**
 * @brief Insert entry at insertion point in tree map.
 * @param [in] object tree map instance.
 * @param [in] insertion_point where entry will be inserted.
 * @param [in] entry to be inserted.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to insert entry.
 */
static bool insert(struct coral_red_black_tree_map *const object,
                   struct rock_red_black_tree_node *const insertion_point,
                   struct entry *entry) {
    assert(object);
    assert(entry);
    this = object;
    void *ptr = NULL;
    if (!coral_linked_stack_push(&stack, &ptr)) {
        seagrass_required_true(CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED
                               == coral_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    seagrass_required_true(rock_red_black_tree_insert(
            &object->tree, insertion_point, &entry->node));
    seagrass_required_true(coral_linked_stack_pop(
            &stack, (void **) &ptr));
    return true;
}

bool coral_red_black_tree_map_add(
        struct coral_red_black_tree_map *const object,
        const void *const key,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *insertion_point;
    if (find(object, key, &insertion_point)) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_ALREADY_EXISTS;
        return false;
    } else if (CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
               == coral_error) {
        return false;
    }
    seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND
                           == coral_error);
    struct entry *entry;
    {
        const int error = posix_memalign((void **) &entry, sizeof(void *),
                                         object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            coral_error =
                    CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED;
            return false;
        }
    }
    seagrass_required_true(rock_red_black_tree_node_init(&entry->node));
    memcpy(&entry->data, key, object->key);
    void *const dest = ((char *) &entry->data) + object->key + object->padding;
    memcpy(dest, value, object->value);
    const bool result = insert(object, insertion_point, entry);
    if (!result) {
        seagrass_required_true(
                CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED
                == coral_error);
        free(entry);
    }
    return result;
}

bool coral_red_black_tree_map_remove(
        struct coral_red_black_tree_map *const object,
        const void *const key) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, key, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
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

bool coral_red_black_tree_map_contains(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        bool *const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    *out = find(object, key, &node);
    if (!*out) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
    }
    return *out
           || CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND == coral_error;
}

bool coral_red_black_tree_map_set(
        struct coral_red_black_tree_map *const object,
        const void *const key,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, key, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    seagrass_required_true(coral_red_black_tree_map_entry_set_value(
            object, (struct coral_red_black_tree_map_entry *) &A->data, value));
    return true;
}

bool coral_red_black_tree_map_get(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct coral_red_black_tree_map_entry *entry;
    if (!coral_red_black_tree_map_get_entry(object, key, &entry)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    seagrass_required_true(coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return true;
}

bool coral_red_black_tree_map_ceiling(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct coral_red_black_tree_map_entry *entry;
    if (!coral_red_black_tree_map_ceiling_entry(object, key, &entry)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    seagrass_required_true(coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return true;
}

bool coral_red_black_tree_map_floor(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct coral_red_black_tree_map_entry *entry;
    if (!coral_red_black_tree_map_floor_entry(object, key, &entry)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    seagrass_required_true(coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return true;
}

bool coral_red_black_tree_map_higher(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct coral_red_black_tree_map_entry *entry;
    if (!coral_red_black_tree_map_higher_entry(object, key, &entry)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    seagrass_required_true(coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return true;
}

bool coral_red_black_tree_map_lower(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct coral_red_black_tree_map_entry *entry;
    if (!coral_red_black_tree_map_lower_entry(object, key, &entry)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    seagrass_required_true(coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return true;
}

bool coral_red_black_tree_map_first(
        const struct coral_red_black_tree_map *const object,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct coral_red_black_tree_map_entry *entry;
    if (!coral_red_black_tree_map_first_entry(object, &entry)) {
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY
                               == coral_error);
        return false;
    }
    seagrass_required_true(coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return true;
}

bool coral_red_black_tree_map_last(
        const struct coral_red_black_tree_map *const object,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    const struct coral_red_black_tree_map_entry *entry;
    if (!coral_red_black_tree_map_last_entry(object, &entry)) {
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY
                               == coral_error);
        return false;
    }
    seagrass_required_true(coral_red_black_tree_map_entry_get_value(
            object, entry, out));
    return true;
}

bool coral_red_black_tree_map_get_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, key, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND:
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
        }
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

/**
 * @brief Compare key with entry using tree map comparator.
 * @param [in] object tree map instance.
 * @param [in] first to be compared
 * @param [in] second to be compared
 * @param [out] out less than 0 if first item is less than second
 * item, 0 if they are equal and greater than 0 if second is greater than first.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED if there
 * is insufficient memory to compare the two items.
 */
static bool compare(const struct coral_red_black_tree_map *const object,
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
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED;
        return false;
    }
    *out = object->compare(first, second);
    seagrass_required_true(coral_linked_stack_pop(
            &stack, &ptr));
    return true;
}

bool coral_red_black_tree_map_ceiling_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, key, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, key, &B->data, &result)) {
                    return false;
                }
                if (result > 0) {
                    coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                    return false;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_floor_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, key, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, key, &B->data, &result)) {
                    return false;
                }
                if (result < 0) {
                    coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                    return false;
                }
            }
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_higher_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, key, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, key, &B->data, &result)) {
                    return false;
                }
                if (result > 0) {
                    coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                    return false;
                }
            }
        }
    } else if (!rock_red_black_tree_next(node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_lower_entry(
        const struct coral_red_black_tree_map *const object,
        const void *const key,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!key) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!find(object, key, &node)) {
        switch (coral_error) {
            default: {
                seagrass_required_true(false);
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED: {
                return false;
            }
            case CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND: {
                if (!node) {
                    return false;
                }
                struct entry *const B = rock_container_of(
                        node, struct entry, node);
                int result;
                if (!compare(object, key, &B->data, &result)) {
                    return false;
                }
                if (result < 0) {
                    coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
                    return false;
                }
            }
        }
    } else if (!rock_red_black_tree_prev(node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_first_entry(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_first(&object->tree, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_last_entry(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_last(&object->tree, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_remove_entry(
        struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!entry) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
        return false;
    }
    struct entry *const A = rock_container_of(entry, struct entry, data);
    seagrass_required_true(rock_red_black_tree_remove(
            &object->tree, &A->node));
    free(A);
    return true;
}

bool coral_red_black_tree_map_next_entry(
        const struct coral_red_black_tree_map_entry *const entry,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!entry) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    struct entry *const B = rock_container_of(entry, struct entry, data);
    if (!rock_red_black_tree_next(&B->node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_END_OF_SEQUENCE;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_prev_entry(
        const struct coral_red_black_tree_map_entry *const entry,
        const struct coral_red_black_tree_map_entry **const out) {
    if (!entry) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    struct rock_red_black_tree_node *node;
    struct entry *const B = rock_container_of(entry, struct entry, data);
    if (!rock_red_black_tree_prev(&B->node, &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_END_OF_SEQUENCE;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
    return true;
}

bool coral_red_black_tree_map_entry_key(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!entry) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = entry; /* address of entry is the address of the key in entry */
    return true;
}

bool coral_red_black_tree_map_entry_get_value(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry,
        const void **const out) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!entry) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = ((char *) entry) + object->key + object->padding;
    return true;
}

bool coral_red_black_tree_map_entry_set_value(
        const struct coral_red_black_tree_map *const object,
        const struct coral_red_black_tree_map_entry *const entry,
        const void *const value) {
    if (!object) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!entry) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL;
        return false;
    }
    if (!value) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL;
        return false;
    }
    void *const dest = ((char *) entry) + object->key + object->padding;
    memcpy(dest, value, object->value);
    return true;
}
