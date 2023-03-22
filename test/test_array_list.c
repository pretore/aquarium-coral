#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <time.h>
#include <coral.h>

#include <test/cmocka.h>

static void check_invalidate_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_invalidate(NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_invalidate(void **state) {
    struct coral_array_list object = {};
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_init_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_init(NULL, sizeof(uintmax_t), 0),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_init_error_on_size_is_zero(void **state) {
    assert_int_equal(
            coral_array_list_init((void *) 1, 0, 10),
            CORAL_ARRAY_LIST_ERROR_SIZE_IS_ZERO);
}

static void check_init_error_on_memory_allocation_failed(void **state) {
    struct coral_array_list object;
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_array_list_init(&object, sizeof(uintmax_t), 10),
            CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden
            = posix_memalign_is_overridden = false;
}

static void should_not_be_called(void *item) {
    function_called();
}

static void check_init(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(
            &object, sizeof(uintmax_t), 10), 0);
    assert_int_equal(coral_array_list_invalidate(
            &object, should_not_be_called), 0);
}

static void check_capacity_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_capacity(NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_capacity_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_capacity((void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_capacity(void **state) {
    srand(time(NULL));
    struct coral_array_list object = {
            .array.capacity = rand() % UINTMAX_MAX
    };
    uintmax_t out;
    assert_int_equal(coral_array_list_capacity(&object, &out), 0);
    assert_int_equal(out, object.array.capacity);
}

static void check_get_length_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_get_length(NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_get_length_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_get_length((void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_get_length(void **state) {
    srand(time(NULL));
    struct coral_array_list object = {
            .array.length = rand() % UINTMAX_MAX
    };
    uintmax_t out;
    assert_int_equal(coral_array_list_get_length(&object, &out), 0);
    assert_int_equal(out, object.array.length);
}

static void check_set_length_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_set_length(NULL, 10),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_set_length_error_on_memory_allocation_failed(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_array_list_set_length(&object, 1),
            CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_set_length(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(object.array.length, 0);
    assert_int_equal(coral_array_list_set_length(&object, 1), 0);
    assert_int_equal(object.array.length, 1);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_size_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_size(NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_size_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_size((void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_size(void **state) {
    srand(time(NULL));
    struct coral_array_list object = {
            .array.size = rand() % UINTMAX_MAX
    };
    size_t out;
    assert_int_equal(coral_array_list_size(&object, &out), 0);
    assert_int_equal(out, object.array.size);
}

static void check_shrink_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_shrink(NULL),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_shrink(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 10), 0);
    assert_int_equal(object.array.capacity, 10);
    assert_int_equal(coral_array_list_shrink(&object), 0);
    assert_int_equal(object.array.capacity, 0);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_add_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_add(NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_add_error_on_memory_allocation_failed(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_array_list_add(&object, NULL),
            CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_add(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(object.array.length, 0);
    assert_int_equal(coral_array_list_add(&object, NULL), 0);
    assert_int_equal(object.array.length, 1);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_add_all_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_add_all(NULL, 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_add_all_error_on_count_is_zero(void **state) {
    assert_int_equal(
            coral_array_list_add_all((void *) 1, 0, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO);
}

static void check_add_all_error_on_items_is_null(void **state) {
    assert_int_equal(
            coral_array_list_add_all((void *) 1, 1, NULL),
            CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL);
}

static void check_add_all(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(object.array.length, 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    assert_int_equal(object.array.length, count);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_remove_last_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_remove_last(NULL),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_remove_last_error_on_list_is_empty(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(
            coral_array_list_remove_last(&object),
            CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_remove_last(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(object.array.length, 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    assert_int_equal(object.array.length, count);
    assert_int_equal(coral_array_list_remove_last(&object), 0);
    assert_int_equal(object.array.length, count - 1);
    uintmax_t *out;
    assert_int_equal(rock_array_last(&object.array, (void **) &out), 0);
    assert_int_equal(*out, values[0]);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_insert_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_insert(NULL, 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_insert_error_on_index_is_out_of_bounds(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(void *), 0), 0);
    assert_int_equal(
            coral_array_list_insert(&object, 0, NULL),
            CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_insert_error_on_memory_allocation_failed(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(void *), 0), 0);
    assert_int_equal(coral_array_list_set_length(&object, 1), 0);
    assert_int_equal(coral_array_list_shrink(&object), 0);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_array_list_insert(&object, 0, NULL),
            CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_insert(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(coral_array_list_set_length(&object, 1), 0);
    const uintmax_t value = rand() % UINTMAX_MAX;
    assert_int_equal(coral_array_list_insert(
            &object, 0, (const void **) &value), 0);
    uintmax_t *out;
    assert_int_equal(rock_array_get(&object.array, 0, (void **) &out), 0);
    assert_int_equal(*out, value);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_insert_all_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_insert_all(NULL, 0, 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_insert_all_error_on_count_is_zero(void **state) {
    assert_int_equal(
            coral_array_list_insert_all((void *) 1, 0, 0, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO);
}

static void check_insert_all_error_on_items_is_null(void **state) {
    assert_int_equal(
            coral_array_list_insert_all((void *) 1, 0, 1, NULL),
            CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL);
}

static void check_insert_all_error_on_index_is_out_of_bounds(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_insert_all(&object, 0, count, items),
                     CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_insert_all_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(coral_array_list_set_length(&object, 1), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_array_list_insert_all(&object, 0, count, items),
            CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_insert_all(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(coral_array_list_set_length(&object, 1), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_insert_all(&object, 0, count, items), 0);
    uintmax_t *out;
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(rock_array_get(&object.array, i, (void **) &out), 0);
        assert_int_equal(*out, values[i]);
    }
    assert_int_equal(rock_array_last(&object.array, (void **) &out), 0);
    assert_int_equal(*out, 0);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_remove_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_remove(NULL, 0),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_remove_error_on_index_is_out_of_bounds(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(
            coral_array_list_remove(&object, 0),
            CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_remove(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const void *items[] = {
            &values[0],
            &values[1],
            &values[2],
            &values[3],
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    uintmax_t *out;
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(rock_array_get(
                &object.array, i, (void **) &out), 0);
        assert_int_equal(*out, values[i]);
    }
    assert_int_equal(coral_array_list_remove(&object, 0), 0);
    for (uintmax_t i = 1; i < count; i++) {
        assert_int_equal(rock_array_get(
                &object.array, i - 1, (void **) &out), 0);
        assert_int_equal(*out, values[i]);
    }
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_remove_all_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_remove_all(NULL, 0, 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_remove_all_error_on_count_is_null(void **state) {
    assert_int_equal(
            coral_array_list_remove_all((void *) 1, 0, 0),
            CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO);
}

static void check_remove_all_error_on_index_is_out_of_bounds(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(
            coral_array_list_remove_all(&object, 0, 1),
            CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_remove_all(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const void *items[] = {
            &values[0],
            &values[1],
            &values[2],
            &values[3],
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    assert_int_equal(object.array.length, 4);
    assert_int_equal(coral_array_list_remove_all(&object, 1, 100), 0);
    assert_int_equal(object.array.length, 1);
    uintmax_t *out;
    assert_int_equal(rock_array_first(&object.array, (void **) &out), 0);
    assert_int_equal(*out, values[0]);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_get_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_get(NULL, 0, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_get_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_get((void *) 1, 0, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_get_error_on_index_is_out_of_bounds(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(
            coral_array_list_get(&object, 0, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_get(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const void *items[] = {
            &values[0],
            &values[1],
            &values[2],
            &values[3],
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    uintmax_t *out;
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_array_list_get(&object, i, (void **) &out), 0);
        assert_int_equal(*out, values[i]);
    }
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_set_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_set(NULL, 0, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_set_error_on_index_is_out_of_bounds(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(
            coral_array_list_set(&object, 0, NULL),
            CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_set(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    assert_int_equal(coral_array_list_set_length(&object, 1), 0);
    const uintmax_t value = rand() % UINTMAX_MAX;
    assert_int_equal(coral_array_list_set(&object, 0, &value), 0);
    uintmax_t *out;
    assert_int_equal(coral_array_list_get(&object, 0, (void **) &out), 0);
    assert_int_equal(*out, value);
    assert_int_equal(coral_array_list_set(&object, 0, NULL), 0);
    assert_int_equal(coral_array_list_get(&object, 0, (void **) &out), 0);
    assert_int_equal(*out, 0);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_first_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_first(NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_first_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_first((void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_first_error_on_list_is_empty(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    uintmax_t *out;
    assert_int_equal(
            coral_array_list_first(&object, (void **) &out),
            CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_first(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    uintmax_t *out;
    assert_int_equal(coral_array_list_first(&object, (void **) &out), 0);
    assert_int_equal(*out, values[0]);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_last_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_last(NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_last_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_last((void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_last_error_on_list_is_empty(void **state) {
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    uintmax_t *out;
    assert_int_equal(
            coral_array_list_last(&object, (void **) &out),
            CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_last(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    uintmax_t *out;
    assert_int_equal(coral_array_list_last(&object, (void **) &out), 0);
    assert_int_equal(*out, values[1]);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_next_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_next(NULL, (void *) 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_next_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_array_list_next((void *) 1, NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL);
}

static void check_next_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_next((void *) 1, (void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_next_error_on_item_is_out_of_bounds(void **state) {
    struct coral_array_list object = {
            .array.data = (void *) 0x1000,
            .array.length = 1
    };
    assert_int_equal(
            coral_array_list_next(&object, (void *) 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS);
}

static void check_next_error_on_end_of_sequence(void **state) {
    struct coral_array_list object = {
            .array.data = (void *) 0x1000
    };
    assert_int_equal(
            coral_array_list_next(&object, (void *) 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE);
}

static void check_next(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    uintmax_t *out;
    assert_int_equal(coral_array_list_first(&object, (void **) &out), 0);
    assert_int_equal(*out, values[0]);
    assert_int_equal(coral_array_list_next(&object, out, (void **) &out), 0);
    assert_int_equal(*out, values[1]);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_prev_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_prev(NULL, (void *) 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_prev_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_array_list_prev((void *) 1, NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL);
}

static void check_prev_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_prev((void *) 1, (void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_prev_error_on_item_is_out_of_bounds(void **state) {
    struct coral_array_list object = {
            .array.data = (void *) 0x1000,
            .array.length = 1
    };
    assert_int_equal(
            coral_array_list_prev(&object, (void *) 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS);
}

static void check_prev_error_on_end_of_sequence(void **state) {
    struct coral_array_list object = {
            .array.data = (void *) 0x1000
    };
    assert_int_equal(
            coral_array_list_prev(&object, (void *) 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE);
}

static void check_prev(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    uintmax_t *out;
    assert_int_equal(coral_array_list_last(&object, (void **) &out), 0);
    assert_int_equal(*out, values[1]);
    assert_int_equal(coral_array_list_prev(&object, out, (void **) &out), 0);
    assert_int_equal(*out, values[0]);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_at_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_array_list_at(NULL, (void *) 1, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_at_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_array_list_at((void *) 1, NULL, (void *) 1),
            CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL);
}

static void check_at_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_array_list_at((void *) 1, (void *) 1, NULL),
            CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL);
}

static void check_at(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1],
            &values[3],
            &values[4]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    void *item;
    assert_int_equal(coral_array_list_first(&object, &item), 0);
    for (uintmax_t i = 0; i < count; i++) {
        uintmax_t at;
        assert_int_equal(coral_array_list_at(&object, item, &at), 0);
        assert_int_equal(at, i);
        if (1 + i < count) {
            assert_int_equal(coral_array_list_next(&object, item, &item), 0);
        }
    }
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

static void check_at_error_on_item_is_out_of_bounds(void **state) {
    srand(time(NULL));
    struct coral_array_list object;
    assert_int_equal(coral_array_list_init(&object, sizeof(uintmax_t), 0), 0);
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const void *items[] = {
            &values[0],
            &values[1],
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_int_equal(coral_array_list_add_all(&object, count, items), 0);
    uintmax_t at;
    assert_int_equal(
            coral_array_list_at(&object, (void *) 1, &at),
            CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS);
    assert_int_equal(coral_array_list_invalidate(&object, NULL), 0);
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_invalidate_error_on_object_is_null),
            cmocka_unit_test(check_invalidate),
            cmocka_unit_test(check_init_error_on_object_is_null),
            cmocka_unit_test(check_init_error_on_size_is_zero),
            cmocka_unit_test(check_init_error_on_memory_allocation_failed),
            cmocka_unit_test(check_init),
            cmocka_unit_test(check_capacity_error_on_object_is_null),
            cmocka_unit_test(check_capacity_error_on_out_is_null),
            cmocka_unit_test(check_capacity),
            cmocka_unit_test(check_get_length_error_on_object_is_null),
            cmocka_unit_test(check_get_length_error_on_out_is_null),
            cmocka_unit_test(check_get_length),
            cmocka_unit_test(check_set_length_error_on_object_is_null),
            cmocka_unit_test(check_set_length_error_on_memory_allocation_failed),
            cmocka_unit_test(check_set_length),
            cmocka_unit_test(check_size_error_on_object_is_null),
            cmocka_unit_test(check_size_error_on_out_is_null),
            cmocka_unit_test(check_size),
            cmocka_unit_test(check_shrink_error_on_object_is_null),
            cmocka_unit_test(check_shrink),
            cmocka_unit_test(check_add_error_on_object_is_null),
            cmocka_unit_test(check_add_error_on_memory_allocation_failed),
            cmocka_unit_test(check_add),
            cmocka_unit_test(check_add_all_error_on_object_is_null),
            cmocka_unit_test(check_add_all_error_on_count_is_zero),
            cmocka_unit_test(check_add_all_error_on_items_is_null),
            cmocka_unit_test(check_add_all),
            cmocka_unit_test(check_remove_last_error_on_object_is_null),
            cmocka_unit_test(check_remove_last_error_on_list_is_empty),
            cmocka_unit_test(check_remove_last),
            cmocka_unit_test(check_insert_error_on_object_is_null),
            cmocka_unit_test(check_insert_error_on_index_is_out_of_bounds),
            cmocka_unit_test(check_insert_error_on_memory_allocation_failed),
            cmocka_unit_test(check_insert),
            cmocka_unit_test(check_insert_all_error_on_object_is_null),
            cmocka_unit_test(check_insert_all_error_on_count_is_zero),
            cmocka_unit_test(check_insert_all_error_on_items_is_null),
            cmocka_unit_test(check_insert_all_error_on_index_is_out_of_bounds),
            cmocka_unit_test(check_insert_all_error_on_memory_allocation_failed),
            cmocka_unit_test(check_insert_all),
            cmocka_unit_test(check_remove_error_on_object_is_null),
            cmocka_unit_test(check_remove_error_on_index_is_out_of_bounds),
            cmocka_unit_test(check_remove),
            cmocka_unit_test(check_remove_all_error_on_object_is_null),
            cmocka_unit_test(check_remove_all_error_on_count_is_null),
            cmocka_unit_test(check_remove_all_error_on_index_is_out_of_bounds),
            cmocka_unit_test(check_remove_all),
            cmocka_unit_test(check_get_error_on_object_is_null),
            cmocka_unit_test(check_get_error_on_out_is_null),
            cmocka_unit_test(check_get_error_on_index_is_out_of_bounds),
            cmocka_unit_test(check_get),
            cmocka_unit_test(check_set_error_on_object_is_null),
            cmocka_unit_test(check_set_error_on_index_is_out_of_bounds),
            cmocka_unit_test(check_set),
            cmocka_unit_test(check_first_error_on_object_is_null),
            cmocka_unit_test(check_first_error_on_out_is_null),
            cmocka_unit_test(check_first_error_on_list_is_empty),
            cmocka_unit_test(check_first),
            cmocka_unit_test(check_last_error_on_object_is_null),
            cmocka_unit_test(check_last_error_on_out_is_null),
            cmocka_unit_test(check_last_error_on_list_is_empty),
            cmocka_unit_test(check_last),
            cmocka_unit_test(check_next_error_on_object_is_null),
            cmocka_unit_test(check_next_error_on_item_is_null),
            cmocka_unit_test(check_next_error_on_out_is_null),
            cmocka_unit_test(check_next_error_on_item_is_out_of_bounds),
            cmocka_unit_test(check_next_error_on_end_of_sequence),
            cmocka_unit_test(check_next),
            cmocka_unit_test(check_prev_error_on_object_is_null),
            cmocka_unit_test(check_prev_error_on_item_is_null),
            cmocka_unit_test(check_prev_error_on_out_is_null),
            cmocka_unit_test(check_prev_error_on_item_is_out_of_bounds),
            cmocka_unit_test(check_prev_error_on_end_of_sequence),
            cmocka_unit_test(check_prev),
            cmocka_unit_test(check_at_error_on_object_is_null),
            cmocka_unit_test(check_at_error_on_item_is_null),
            cmocka_unit_test(check_at_error_on_out_is_null),
            cmocka_unit_test(check_at),
            cmocka_unit_test(check_at_error_on_item_is_out_of_bounds),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
