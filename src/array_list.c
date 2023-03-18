#include <stdlib.h>
#include <assert.h>
#include <seagrass.h>
#include <rock.h>
#include <coral.h>

#ifdef TEST
#include <test/cmocka.h>
#endif

int coral_array_list_init(struct coral_array_list *const object,
                          const size_t size,
                          const uintmax_t capacity) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!size) {
        return CORAL_ARRAY_LIST_ERROR_SIZE_IS_ZERO;
    }
    int error;
    if ((error = rock_array_init(&object->array, size, capacity))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == error);
    }
    return error;
}

int coral_array_list_invalidate(struct coral_array_list *const object,
                                void (*const on_destroy)(void *)) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    seagrass_required_true(!rock_array_invalidate(
            &object->array, on_destroy));
    return 0;
}

static int attribute(const struct coral_array_list *const object,
                     uintmax_t *const out,
                     int (*const func)(const struct rock_array *,
                                       uintmax_t *)) {
    assert(func);
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
    }
    seagrass_required_true(!func(&object->array, out));
    return 0;
}

int coral_array_list_capacity(const struct coral_array_list *const object,
                              uintmax_t *const out) {
    return attribute(object, out, rock_array_capacity);
}

int coral_array_list_get_length(const struct coral_array_list *const object,
                                uintmax_t *const out) {
    return attribute(object, out, rock_array_get_length);
}

int coral_array_list_set_length(struct coral_array_list *const object,
                                const uintmax_t length) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    int error;
    if ((error = rock_array_set_length(&object->array, length))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == error);
    }
    return error;
}

int coral_array_list_size(const struct coral_array_list *const object,
                          size_t *const out) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
    }
    seagrass_required_true(!rock_array_size(&object->array, out));
    return 0;
}

int coral_array_list_shrink(struct coral_array_list *const object) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    seagrass_required_true(!rock_array_shrink(&object->array));
    return 0;
}

int coral_array_list_add(struct coral_array_list *const object,
                          const void *const item) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    int error;
    if ((error = rock_array_add(&object->array, item))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == error);
    }
    return error;
}

int coral_array_list_add_all(struct coral_array_list *const object,
                             const uintmax_t count,
                             const void **const items) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!count) {
        return CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO;
    }
    if (!items) {
        return CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL;
    }
    int error;
    if ((error = rock_array_add_all(&object->array, count, items))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == error);
    }
    return error;
}

int coral_array_list_remove_last(struct coral_array_list *const object) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    int error;
    if ((error = rock_array_remove_last(&object->array))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_ARRAY_IS_EMPTY
                               == error);
    }
    return error;
}

int coral_array_list_insert(struct coral_array_list *const object,
                            const uintmax_t at,
                            const void *const item) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    int error;
    if ((error = rock_array_insert(&object->array, at, item))) {
        switch (error) {
            case ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED:
            case ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS: {
                break;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    return error;
}

int coral_array_list_insert_all(struct coral_array_list *const object,
                                const uintmax_t at,
                                const uintmax_t count,
                                const void **const items) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!count) {
        return CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO;
    }
    if (!items) {
        return CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL;
    }
    int error;
    if ((error = rock_array_insert_all(&object->array, at, count, items))) {
        switch (error) {
            case ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS:
            case ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED: {
                break;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    return error;
}

int coral_array_list_remove(struct coral_array_list *const object,
                            const uintmax_t at) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    int error;
    if ((error = rock_array_remove(&object->array, at))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == error);
    }
    return error;
}

int coral_array_list_remove_all(struct coral_array_list *const object,
                                const uintmax_t at,
                                const uintmax_t count) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!count) {
        return CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO;
    }
    int error;
    if ((error = rock_array_remove_all(&object->array, at, count))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == error);
    }
    return error;
}

int coral_array_list_get(const struct coral_array_list *const object,
                         const uintmax_t at,
                         void **const out) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
    }
    int error;
    if ((error = rock_array_get(&object->array, at, out))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == error);
    }
    return error;
}

int coral_array_list_set(struct coral_array_list *const object,
                         const uintmax_t at,
                         const void *const item) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    int error;
    if ((error = rock_array_set(&object->array, at, item))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == error);
    }
    return error;
}

static int retrieve_fl(const struct coral_array_list *const object,
                       void **const out,
                       int (*const func)(const struct rock_array *,
                                         void **)) {
    assert(func);
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
    }
    int error;
    if ((error = func(&object->array, out))) {
        seagrass_required_true(ROCK_ARRAY_ERROR_ARRAY_IS_EMPTY
                               == error);
    }
    return error;
}

int coral_array_list_first(const struct coral_array_list *const object,
                           void **const out) {
    return retrieve_fl(object, out, rock_array_first);
}

int coral_array_list_last(const struct coral_array_list *const object,
                          void **const out) {
    return retrieve_fl(object, out, rock_array_last);
}

static int retrieve_np(
        const struct coral_array_list *const object,
        const void *const item,
        void **const out,
        int (*const func)(const struct rock_array *,
                          const void *const,
                          void **const)) {
    assert(func);
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
    }
    int error;
    if ((error = func(&object->array, item, out))) {
        switch (error) {
            default: {
                seagrass_required_true(false);
            }
            case ROCK_ARRAY_ERROR_ITEM_IS_OUT_OF_BOUNDS:
            case ROCK_ARRAY_ERROR_END_OF_SEQUENCE: {
                break;
            }
        }
    }
    return error;
}

int coral_array_list_next(const struct coral_array_list *const object,
                          const void *const item,
                          void **const out) {
    return retrieve_np(object, item, out, rock_array_next);
}

int coral_array_list_prev(const struct coral_array_list *const object,
                          const void *const item,
                          void **const out) {
    return retrieve_np(object, item, out, rock_array_prev);
}

int coral_array_list_at(const struct coral_array_list *const object,
                        const void *const item,
                        uintmax_t *const out) {
    if (!object) {
        return CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
    }
    int error;
    if ((error = rock_array_at(&object->array, item, out))) {
        seagrass_required_true(
                ROCK_ARRAY_ERROR_ITEM_IS_OUT_OF_BOUNDS
                == error);
    }
    return error;
}
