#include <stdlib.h>
#include <assert.h>
#include <seagrass.h>
#include <rock.h>
#include <coral.h>

bool coral_array_list_init(struct coral_array_list *const object,
                           const size_t size,
                           const uintmax_t capacity) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!size) {
        coral_error = CORAL_ARRAY_LIST_ERROR_SIZE_IS_ZERO;
        return false;
    }
    const bool result = rock_array_init(&object->array, size, capacity);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    return result;
}

bool coral_array_list_invalidate(struct coral_array_list *const object,
                                 void (*const on_destroy)(void *)) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    seagrass_required_true(rock_array_invalidate(
            &object->array, on_destroy));
    return true;
}

static bool get_attribute(const struct coral_array_list *const object,
                          uintmax_t *const out,
                          bool (*const func)(const struct rock_array *,
                                             uintmax_t *)) {
    assert(func);
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
        return false;
    }
    seagrass_required_true(func(&object->array, out));
    return true;
}

bool coral_array_list_capacity(const struct coral_array_list *const object,
                               uintmax_t *const out) {
    return get_attribute(object, out, rock_array_capacity);
}

bool coral_array_list_get_length(const struct coral_array_list *const object,
                                 uintmax_t *const out) {
    return get_attribute(object, out, rock_array_get_length);
}

bool coral_array_list_set_length(struct coral_array_list *const object,
                                 const uintmax_t length) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    const bool result = rock_array_set_length(&object->array, length);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    return result;
}

bool coral_array_list_size(const struct coral_array_list *const object,
                           size_t *const out) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
        return false;
    }
    seagrass_required_true(rock_array_size(&object->array, out));
    return true;
}

bool coral_array_list_shrink(struct coral_array_list *const object) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    seagrass_required_true(rock_array_shrink(&object->array));
    return true;
}

bool coral_array_list_add(struct coral_array_list *const object,
                          const void *const item) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    const bool result = rock_array_add(&object->array, item);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    return result;
}

bool coral_array_list_add_all(struct coral_array_list *const object,
                              const uintmax_t count,
                              const void **const items) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!count) {
        coral_error = CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO;
        return false;
    }
    if (!items) {
        coral_error = CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL;
        return false;
    }
    const bool result = rock_array_add_all(&object->array, count, items);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    return result;
}

bool coral_array_list_remove_last(struct coral_array_list *const object) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    const bool result = rock_array_remove_last(&object->array);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_ARRAY_IS_EMPTY
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY;
    }
    return result;
}

bool coral_array_list_insert(struct coral_array_list *const object,
                             const uintmax_t at,
                             const void *const item) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    const bool result = rock_array_insert(&object->array, at, item);
    if (!result) {
        switch (rock_error) {
            case ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS: {
                coral_error = CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS;
                break;
            }
            case ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED: {
                coral_error = CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED;
                break;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    return result;
}

bool coral_array_list_insert_all(struct coral_array_list *const object,
                                 const uintmax_t at,
                                 const uintmax_t count,
                                 const void **const items) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!count) {
        coral_error = CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO;
        return false;
    }
    if (!items) {
        coral_error = CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL;
        return false;
    }
    const bool result = rock_array_insert_all(&object->array, at, count, items);
    if (!result) {
        switch (rock_error) {
            case ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS: {
                coral_error = CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS;
                break;
            }
            case ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED: {
                coral_error = CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED;
                break;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    return result;
}

bool coral_array_list_remove(struct coral_array_list *const object,
                             const uintmax_t at) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    const bool result = rock_array_remove(&object->array, at);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS;
    }
    return result;
}

bool coral_array_list_remove_all(struct coral_array_list *const object,
                                 const uintmax_t at,
                                 const uintmax_t count) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!count) {
        coral_error = CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO;
        return false;
    }
    const bool result = rock_array_remove_all(&object->array, at, count);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS;
    }
    return result;
}

bool coral_array_list_get(const struct coral_array_list *const object,
                          const uintmax_t at,
                          void **const out) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
        return false;
    }
    const bool result = rock_array_get(&object->array, at, out);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS;
    }
    return result;
}

bool coral_array_list_set(struct coral_array_list *const object,
                          const uintmax_t at,
                          const void *const item) {
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    const bool result = rock_array_set(&object->array, at, item);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS;
    }
    return result;
}

static bool retrieve_fl(const struct coral_array_list *const object,
                        void **const out,
                        bool (*const func)(const struct rock_array *,
                                           void **)) {
    assert(func);
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
        return false;
    }
    const bool result = func(&object->array, out);
    if (!result) {
        seagrass_required_true(ROCK_ARRAY_ERROR_ARRAY_IS_EMPTY
                               == rock_error);
        coral_error = CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY;
    }
    return result;
}

bool coral_array_list_first(const struct coral_array_list *const object,
                            void **const out) {
    return retrieve_fl(object, out, rock_array_first);
}

bool coral_array_list_last(const struct coral_array_list *const object,
                           void **const out) {
    return retrieve_fl(object, out, rock_array_last);
}

static bool retrieve_np(
        const struct coral_array_list *const object,
        const void *const item,
        void **const out,
        bool (*const func)(const struct rock_array *,
                           const void *const,
                           void **const)) {
    assert(func);
    if (!object) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL;
        return false;
    }
    const bool result = func(&object->array, item, out);
    if (!result) {
        switch (rock_error) {
            case ROCK_ARRAY_ERROR_ITEM_IS_OUT_OF_BOUNDS: {
                coral_error = CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS;
                break;
            }
            case ROCK_ARRAY_ERROR_END_OF_SEQUENCE: {
                coral_error = CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE;
                break;
            }
            default: {
                seagrass_required_true(false);
            }
        }
    }
    return result;
}

bool coral_array_list_next(const struct coral_array_list *const object,
                           const void *const item,
                           void **const out) {
    return retrieve_np(object, item, out, rock_array_next);
}

bool coral_array_list_prev(const struct coral_array_list *const object,
                           const void *const item,
                           void **const out) {
    return retrieve_np(object, item, out, rock_array_prev);
}
