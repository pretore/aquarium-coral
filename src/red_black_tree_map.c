#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <seagrass.h>
#include <coral.h>

#include "test/cmocka.h"

struct coral_red_black_tree_map_entry {
    void *data;
};

struct entry {
    struct rock_red_black_tree_node node;
    unsigned char data[];
};

static _Thread_local const struct coral_red_black_tree_map *this;
static _Thread_local const void *ptr;

static int entity_compare(const struct rock_red_black_tree_node *const a,
                          const struct rock_red_black_tree_node *const b) {
    if (!ptr) {
        const struct entry *const A = rock_container_of(a, struct entry, node);
        ptr = &A->data;
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
    this = object;
    on_destroy_callback = on_destroy;
    seagrass_required_true(rock_red_black_tree_invalidate(
            &object->tree, entry_on_destroy));
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
    this = object;
    ptr = key; /* delay copy until we know that key does not already exist */
    if (rock_red_black_tree_find(&object->tree,
                                 NULL,
                                 (void *) 1, /* dummy non-NULL value */
                                 &insertion_point)) {
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_ALREADY_EXISTS;
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
                    CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED;
            return false;
        }
    }
    seagrass_required_true(rock_red_black_tree_node_init(&entry->node));
    memcpy(&entry->data, key, object->key);
    void *const dest = ((char *) &entry->data) + object->key + object->padding;
    memcpy(dest, value, object->value);
    ptr = NULL; /* insert the entry into the container */
    seagrass_required_true(rock_red_black_tree_insert(
            &object->tree, insertion_point, &entry->node));
    return true;
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key for removal */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key */
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
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
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND
                               == coral_error);
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
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND
                               == coral_error);
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
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND
                               == coral_error);
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
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND
                               == coral_error);
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
        seagrass_required_true(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND
                               == coral_error);
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
        return false;
    }
    struct entry *const A = rock_container_of(node, struct entry, node);
    *out = (const struct coral_red_black_tree_map_entry *) &A->data;
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, node);
        if (object->compare(key, &B->data) > 0) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, node);
        if (object->compare(key, &B->data) < 0) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, node);
        if (object->compare(key, &B->data) > 0) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
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
    this = object;
    ptr = key; /* avoid unnecessary copying of key */
    struct rock_red_black_tree_node *node;
    if (!rock_red_black_tree_find(&object->tree,
                                  NULL,
                                  (void *) 1, /* dummy non-NULL value */
                                  &node)) {
        seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                               == rock_error);
        if (!node) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
        }
        struct entry *const B = rock_container_of(node, struct entry, node);
        if (object->compare(key, &B->data) < 0) {
            coral_error = CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND;
            return false;
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
