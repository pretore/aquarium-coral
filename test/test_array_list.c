#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <time.h>
#include <coral.h>

#include <test/cmocka.h>

static void check_invalidate_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_invalidate(NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_invalidate(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {};
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_init(NULL, sizeof(uintmax_t), 0));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_size_is_zero(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_init((void *) 1, 0, 10));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_SIZE_IS_ZERO, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_memory_allocation_failed(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden = true;
    assert_false(coral_array_list_init(&object, sizeof(uintmax_t), 10));
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden = false;
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void should_not_be_called(void *item) {
    function_called();
}

static void check_init(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 10));
    assert_true(coral_array_list_invalidate(&object, should_not_be_called));
    coral_error = CORAL_ERROR_NONE;
}

static void check_capacity_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_capacity(NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_capacity_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_capacity((void *) 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_capacity(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {
            .array.capacity = rand() % UINTMAX_MAX
    };
    uintmax_t out;
    assert_true(coral_array_list_capacity(&object, &out));
    assert_int_equal(out, object.array.capacity);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_length_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_get_length(NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_length_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_get_length((void *) 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_length(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {
            .array.length = rand() % UINTMAX_MAX
    };
    uintmax_t out;
    assert_true(coral_array_list_get_length(&object, &out));
    assert_int_equal(out, object.array.length);
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_length_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_set_length(NULL, 10));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_length_error_on_memory_allocation_failed(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden = true;
    assert_false(coral_array_list_set_length(&object, 1));
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden = false;
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_length(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_int_equal(object.array.length, 0);
    assert_true(coral_array_list_set_length(&object, 1));
    assert_int_equal(object.array.length, 1);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_size_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_size(NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_size_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_size((void *) 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_size(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {
            .array.size = rand() % UINTMAX_MAX
    };
    size_t out;
    assert_true(coral_array_list_size(&object, &out));
    assert_int_equal(out, object.array.size);
    coral_error = CORAL_ERROR_NONE;
}

static void check_shrink_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_shrink(NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_shrink(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 10));
    assert_int_equal(object.array.capacity, 10);
    assert_true(coral_array_list_shrink(&object));
    assert_int_equal(object.array.capacity, 0);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_add(NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_memory_allocation_failed(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden = true;
    assert_false(coral_array_list_add(&object, NULL));
    malloc_is_overridden = realloc_is_overridden = calloc_is_overridden = false;
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_add(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_int_equal(object.array.length, 0);
    assert_true(coral_array_list_add(&object, NULL));
    assert_int_equal(object.array.length, 1);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_all_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_add_all(NULL, 1, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_all_error_on_count_is_zero(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_add_all((void *) 1, 0, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_all_error_on_items_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_add_all((void *) 1, 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_all(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
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
    assert_true(coral_array_list_add_all(&object, count, items));
    assert_int_equal(object.array.length, count);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_last_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_remove_last(NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_last_error_on_list_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_false(coral_array_list_remove_last(&object));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY, coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_last(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
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
    assert_true(coral_array_list_add_all(&object, count, items));
    assert_int_equal(object.array.length, count);
    assert_true(coral_array_list_remove_last(&object));
    assert_int_equal(object.array.length, count - 1);
    uintmax_t *out;
    assert_true(rock_array_last(&object.array, (void **) &out));
    assert_int_equal(*out, values[0]);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_insert(NULL, 1, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_error_on_index_is_out_of_bounds(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(void *), 0));
    assert_false(coral_array_list_insert(&object, 0, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_error_on_memory_allocation_failed(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(void *), 0));
    assert_true(coral_array_list_set_length(&object, 1));
    assert_true(coral_array_list_shrink(&object));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = true;
    assert_false(coral_array_list_insert(&object, 0, NULL));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = false;
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_true(coral_array_list_set_length(&object, 1));
    const uintmax_t value = rand() % UINTMAX_MAX;
    assert_true(coral_array_list_insert(&object, 0, (const void **) &value));
    uintmax_t *out;
    assert_true(rock_array_get(&object.array, 0, (void **) &out));
    assert_int_equal(*out, value);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_all_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_insert_all(NULL, 0, 1, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_all_error_on_count_is_zero(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_insert_all((void *) 1, 0, 0, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_all_error_on_items_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_insert_all((void *) 1, 0, 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_all_error_on_index_is_out_of_bounds(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_false(coral_array_list_insert_all(&object, 0, count, items));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_all_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_true(coral_array_list_set_length(&object, 1));
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = true;
    assert_false(coral_array_list_insert_all(&object, 0, count, items));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden = false;
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_insert_all(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_true(coral_array_list_set_length(&object, 1));
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_true(coral_array_list_insert_all(&object, 0, count, items));
    uintmax_t *out;
    for (uintmax_t i = 0; i < count; i++) {
        assert_true(rock_array_get(&object.array, i, (void **) &out));
        assert_int_equal(*out, values[i]);
    }
    assert_true(rock_array_last(&object.array, (void **) &out));
    assert_int_equal(*out, 0);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_remove(NULL, 0));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_index_is_out_of_bounds(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_false(coral_array_list_remove(&object, 0));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
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
    assert_true(coral_array_list_add_all(&object, count, items));
    uintmax_t *out;
    for (uintmax_t i = 0; i < count; i++) {
        assert_true(rock_array_get(&object.array, i, (void **) &out));
        assert_int_equal(*out, values[i]);
    }
    assert_true(coral_array_list_remove(&object, 0));
    for (uintmax_t i = 1; i < count; i++) {
        assert_true(rock_array_get(&object.array, i - 1, (void **) &out));
        assert_int_equal(*out, values[i]);
    }
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_all_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_remove_all(NULL, 0, 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_all_error_on_count_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_remove_all((void *) 1, 0, 0));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_all_error_on_index_is_out_of_bounds(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_false(coral_array_list_remove_all(&object, 0, 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_all(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
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
    assert_true(coral_array_list_add_all(&object, count, items));
    assert_int_equal(object.array.length, 4);
    assert_true(coral_array_list_remove_all(&object, 1, 100));
    assert_int_equal(object.array.length, 1);
    uintmax_t *out;
    assert_true(rock_array_first(&object.array, (void **) &out));
    assert_int_equal(*out, values[0]);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_get(NULL, 0, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_get((void *) 1, 0, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_index_is_out_of_bounds(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_false(coral_array_list_get(&object, 0, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
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
    assert_true(coral_array_list_add_all(&object, count, items));
    uintmax_t *out;
    for (uintmax_t i = 0; i < count; i++) {
        assert_true(coral_array_list_get(&object, i, (void **) &out));
        assert_int_equal(*out, values[i]);
    }
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_set(NULL, 0, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_error_on_index_is_out_of_bounds(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_false(coral_array_list_set(&object, 0, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS,
                     coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_set(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    assert_true(coral_array_list_set_length(&object, 1));
    const uintmax_t value = rand() % UINTMAX_MAX;
    assert_true(coral_array_list_set(&object, 0, &value));
    uintmax_t *out;
    assert_true(coral_array_list_get(&object, 0, (void **) &out));
    assert_int_equal(*out, value);
    assert_true(coral_array_list_set(&object, 0, NULL));
    assert_true(coral_array_list_get(&object, 0, (void **) &out));
    assert_int_equal(*out, 0);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_first(NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_first((void *) 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_error_on_list_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    uintmax_t *out;
    assert_false(coral_array_list_first(&object, (void **) &out));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY, coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_first(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_true(coral_array_list_add_all(&object, count, items));
    uintmax_t *out;
    assert_true(coral_array_list_first(&object, (void **) &out));
    assert_int_equal(*out, values[0]);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_last(NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_last((void *) 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_error_on_list_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    uintmax_t *out;
    assert_false(coral_array_list_last(&object, (void **) &out));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY, coral_error);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_last(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_true(coral_array_list_add_all(&object, count, items));
    uintmax_t *out;
    assert_true(coral_array_list_last(&object, (void **) &out));
    assert_int_equal(*out, values[1]);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_next(NULL, (void *)1, (void *)1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_error_on_item_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_next((void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_next((void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_error_on_item_is_out_of_bounds(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {
            .array.data = (void *)0x1000,
            .array.length = 1
    };
    assert_false(coral_array_list_next(&object, (void *)1, (void *)1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_error_on_end_of_sequence(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {
            .array.data = (void *)0x1000
    };
    assert_false(coral_array_list_next(&object, (void *)1, (void *)1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_next(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_true(coral_array_list_add_all(&object, count, items));
    uintmax_t *out;
    assert_true(coral_array_list_first(&object, (void **) &out));
    assert_int_equal(*out, values[0]);
    assert_true(coral_array_list_next(&object, out, (void **) &out));
    assert_int_equal(*out, values[1]);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_prev(NULL, (void *)1, (void *)1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_error_on_item_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_prev((void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_array_list_prev((void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_error_on_item_is_out_of_bounds(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {
            .array.data = (void *)0x1000,
            .array.length = 1
    };
    assert_false(coral_array_list_prev(&object, (void *)1, (void *)1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_error_on_end_of_sequence(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object = {
            .array.data = (void *)0x1000
    };
    assert_false(coral_array_list_prev(&object, (void *)1, (void *)1));
    assert_int_equal(CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_array_list object;
    assert_true(coral_array_list_init(&object, sizeof(uintmax_t), 0));
    const uintmax_t values[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX
    };
    const void *items[] = {
            &values[0],
            &values[1]
    };
    const uintmax_t count = sizeof(items) / sizeof(void *);
    assert_true(coral_array_list_add_all(&object, count, items));
    uintmax_t *out;
    assert_true(coral_array_list_last(&object, (void **) &out));
    assert_int_equal(*out, values[1]);
    assert_true(coral_array_list_prev(&object, out, (void **) &out));
    assert_int_equal(*out, values[0]);
    assert_true(coral_array_list_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
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
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
