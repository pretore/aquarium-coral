#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <time.h>
#include <seagrass.h>
#include <coral.h>

#include "test/cmocka.h"

static void check_invalidate_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_invalidate(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_invalidate(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object = {};
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_init(NULL,
                                              sizeof(uintmax_t),
                                              sizeof(uintmax_t),
                                              (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_key_size_is_zero(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_init((void *) 1,
                                              0,
                                              sizeof(uintmax_t),
                                              (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_SIZE_IS_ZERO,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_value_size_is_zero(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_init((void *) 1,
                                              sizeof(uintmax_t),
                                              0,
                                              (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_SIZE_IS_ZERO,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_compare_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_init((void *) 1,
                                              sizeof(uintmax_t),
                                              sizeof(uintmax_t),
                                              NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_COMPARE_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_entry_size_is_too_large(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_init((void *) 1,
                                              SIZE_MAX,
                                              sizeof(uintmax_t),
                                              (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_init((void *) 1,
                                              sizeof(uintmax_t),
                                              SIZE_MAX,
                                              (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_init((void *) 1,
                                              SIZE_MAX,
                                              SIZE_MAX,
                                              (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static int uintmax_t_ptr_compare(const void *const a, const void *const b) {
    const uintmax_t A = *(uintmax_t *) a;
    const uintmax_t B = *(uintmax_t *) b;
    return seagrass_uintmax_t_compare(A, B);
}

static void check_init(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    assert_int_equal(object.key, sizeof(uintmax_t));
    assert_int_equal(object.value, sizeof(uintmax_t));
    assert_ptr_equal(object.compare, uintmax_t_ptr_compare);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_with_padding(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(char),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    assert_int_equal(object.key, sizeof(char));
    assert_int_equal(object.value, sizeof(uintmax_t));
    assert_int_equal(object.padding, sizeof(void *) - object.key);
    assert_ptr_equal(object.compare, uintmax_t_ptr_compare);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_key_size_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_key_size(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_key_size_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_key_size((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_key_size(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    const uintmax_t key = 1 + (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_init(
            &object,
            key,
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t out;
    assert_true(coral_red_black_tree_map_key_size(&object, &out));
    assert_int_equal(out, key);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_value_size_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_value_size(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_value_size_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_value_size((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_value_size(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    const uintmax_t value = 1 + (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            value,
            uintmax_t_ptr_compare));
    uintmax_t out;
    assert_true(coral_red_black_tree_map_value_size(&object, &out));
    assert_int_equal(out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_count_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_count(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_count_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_count((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_count(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t out;
    assert_true(coral_red_black_tree_map_count(&object, &out));
    assert_int_equal(object.tree.count, out);
    object.tree.count = (rand() % UINTPTR_MAX);
    assert_true(coral_red_black_tree_map_count(&object, &out));
    assert_int_equal(object.tree.count, out);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_add(NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_add((void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_value_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_add((void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    posix_memalign_is_overridden = false;
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_add(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    key += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(2, object.tree.count);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_key_already_exists(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = (rand() % UINTMAX_MAX);
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    value += (rand() % UINTMAX_MAX);
    assert_false(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_ALREADY_EXISTS,
                     coral_error);
    assert_int_equal(1, object.tree.count);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_remove(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_remove((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    assert_false(coral_red_black_tree_map_remove(&object, &key));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND, coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    assert_true(coral_red_black_tree_map_remove(&object, &key));
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_memory_allocation_failed(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_remove(&object, &key));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_contains_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_contains(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_contains_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_contains(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_contains_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_contains(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_contains(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    bool result;
    key += 1;
    assert_true(coral_red_black_tree_map_contains(&object, &key, &result));
    assert_false(result);
    key -= 1;
    assert_true(coral_red_black_tree_map_contains(&object, &key, &result));
    assert_true(result);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_contains_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    bool result;
    key += 1;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_contains(&object, &key, &result));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
                = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_get(NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_get((void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_get((void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTPTR_MAX);
    const uintmax_t *out;
    assert_false(coral_red_black_tree_map_get(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND, coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_get(
            &object, &key, (const void **) &out));
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    const uintmax_t *out;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_get(
            &object, &key, (const void **) &out));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
                = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_ceiling(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_ceiling(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_ceiling(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const uintmax_t *out;
    assert_false(coral_red_black_tree_map_ceiling(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND, coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key++;
    assert_false(coral_red_black_tree_map_ceiling(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND, coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_ceiling(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    key = 227;
    value = key + delta;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 101;
    assert_true(coral_red_black_tree_map_ceiling(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_ceiling(
            &object, &key, (const void **) &out));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_floor(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_floor(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_floor(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const uintmax_t *out;
    assert_false(coral_red_black_tree_map_floor(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key--;
    assert_false(coral_red_black_tree_map_floor(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_floor(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    key = 21;
    value = key + delta;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 99;
    assert_true(coral_red_black_tree_map_floor(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_floor(
            &object, &key, (const void **) &out));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_higher(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_higher(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_higher(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const uintmax_t *out;
    assert_false(coral_red_black_tree_map_higher(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_false(coral_red_black_tree_map_higher(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 99;
    assert_true(coral_red_black_tree_map_higher(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    key = 227;
    value = key + delta;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 101;
    assert_true(coral_red_black_tree_map_higher(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 99;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_higher(
            &object, &key, (const void **) &out));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_lower(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_lower(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_lower(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const uintmax_t *out;
    assert_false(coral_red_black_tree_map_lower(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_false(coral_red_black_tree_map_lower(
            &object, &key, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 345;
    assert_true(coral_red_black_tree_map_lower(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    key = 21;
    value = key + delta;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 99;
    assert_true(coral_red_black_tree_map_lower(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t delta = (rand() % UINTMAX_MAX);
    uintmax_t key = 100;
    uintmax_t value = key + delta;
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key = 345;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_lower(
            &object, &key, (const void **) &out));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_set(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_set(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_error_on_value_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_set(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_false(coral_red_black_tree_map_set(&object, &key, &value));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND, coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_set(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    const uintmax_t *out = NULL;
    assert_true(coral_red_black_tree_map_get(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_set(&object, &key, &value));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_set_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_int_equal(0, object.tree.count);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_int_equal(1, object.tree.count);
    const uintmax_t *out = NULL;
    assert_true(coral_red_black_tree_map_get(
            &object, &key, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    value += (rand() % UINTMAX_MAX);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_set(&object, &key, &value));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
                = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_first(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_first((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_error_on_map_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t *out;
    assert_false(coral_red_black_tree_map_first(
            &object, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_first(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_first(
            &object, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    key = 43;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_first(
            &object, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_last(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_last((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_error_on_map_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t *out;
    assert_false(coral_red_black_tree_map_last(
            &object, (const void **) &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY, coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_last(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_last(
            &object, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    key = 243;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_last(
            &object, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_get_entry(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_entry_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_get_entry(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_get_entry(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_entry_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTPTR_MAX);
    const struct coral_red_black_tree_map_entry *out;
    assert_false(coral_red_black_tree_map_get_entry(&object, &key, &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTPTR_MAX);
    const uintmax_t value = (rand() % UINTPTR_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_get_entry(&object, &key, &entry));
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(&key, out);
    assert_int_equal(key, *out);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(&value, out);
    assert_int_equal(value, *out);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_get_entry_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTPTR_MAX);
    const uintmax_t value = (rand() % UINTPTR_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_get_entry(&object, &key, &entry));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_ceiling_entry(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_entry_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_ceiling_entry(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_ceiling_entry(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_entry_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_false(coral_red_black_tree_map_ceiling_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key++;
    assert_false(coral_red_black_tree_map_ceiling_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_ceiling_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_ceiling_entry(
            &object, &key, &entry));
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    key = 2431;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const uintmax_t find = 101;
    assert_true(coral_red_black_tree_map_ceiling_entry(
            &object, &find, &entry));
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void
check_ceiling_entry_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_ceiling_entry(
            &object, &key, &entry));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_floor_entry(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_entry_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_floor_entry(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_floor_entry(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_entry_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_false(coral_red_black_tree_map_floor_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    key--;
    assert_false(coral_red_black_tree_map_floor_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_floor_entry(
            &object, &key, &entry));
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    key = 34;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const uintmax_t find = 99;
    assert_true(coral_red_black_tree_map_floor_entry(
            &object, &find, &entry));
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_floor_entry_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_floor_entry(
            &object, &key, &entry));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_higher_entry(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_entry_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_higher_entry(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_higher_entry(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_entry_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_false(coral_red_black_tree_map_higher_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_false(coral_red_black_tree_map_higher_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_higher_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    uintmax_t find = key - 1;
    assert_true(coral_red_black_tree_map_higher_entry(
            &object, &find, &entry));
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    key = 2431;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    find = 101;
    assert_true(coral_red_black_tree_map_higher_entry(
            &object, &find, &entry));
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void
check_higher_entry_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    uintmax_t find = key - 1;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_higher_entry(
            &object, &find, &entry));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_lower_entry(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_entry_error_on_key_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_lower_entry(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_lower_entry(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_entry_error_on_key_not_found(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    const uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_false(coral_red_black_tree_map_lower_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_false(coral_red_black_tree_map_lower_entry(
            &object, &key, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    uintmax_t find = 1 + key;
    assert_true(coral_red_black_tree_map_lower_entry(
            &object, &find, &entry));
    const uintmax_t *out;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    key = 34;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    find = 99;
    assert_true(coral_red_black_tree_map_lower_entry(
            &object, &find, &entry));
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &key);
    assert_int_equal(*out, key);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &out));
    assert_ptr_not_equal(out, &value);
    assert_int_equal(*out, value);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_lower_entry_error_on_memory_allocation_failed(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    uintmax_t find = 1 + key;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_false(coral_red_black_tree_map_lower_entry(
            &object, &find, &entry));
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_first_entry(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_first_entry((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_entry_error_on_map_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const struct coral_red_black_tree_map_entry *out;
    assert_false(coral_red_black_tree_map_first_entry(&object, &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_first_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_first_entry(&object, &entry));
    const uintmax_t *check;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&key, check);
    assert_int_equal(key, *check);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    key = 43;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_first_entry(&object, &entry));
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&key, check);
    assert_int_equal(key, *check);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_last_entry(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_last_entry((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_entry_error_on_map_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const struct coral_red_black_tree_map_entry *out;
    assert_false(coral_red_black_tree_map_last_entry(&object, &out));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_last_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 100;
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_last_entry(&object, &entry));
    const uintmax_t *check;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&key, check);
    assert_int_equal(key, *check);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    key = 243;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_last_entry(&object, &entry));
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&key, check);
    assert_int_equal(key, *check);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_entry_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_remove_entry(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_entry_error_on_entry_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_remove_entry((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    for (uintmax_t i = 0; i < 3; i++) {
        uintmax_t value = (rand() % UINTMAX_MAX);
        assert_true(coral_red_black_tree_map_add(
                &object, &i, &value));
    }
    const struct coral_red_black_tree_map_entry *entry;
    uintmax_t key = 1;
    assert_true(coral_red_black_tree_map_get_entry(&object, &key, &entry));
    uintmax_t count;
    assert_true(coral_red_black_tree_map_count(&object, &count));
    assert_int_equal(count, 3);
    assert_true(coral_red_black_tree_map_remove_entry(&object, entry));
    assert_true(coral_red_black_tree_map_count(&object, &count));
    assert_int_equal(count, 2);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_entry_error_on_entry_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_next_entry(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_next_entry((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_entry_error_end_of_sequence(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_last_entry(&object, &entry));
    assert_false(coral_red_black_tree_map_next_entry(entry, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_END_OF_SEQUENCE,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_next_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 10;
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_last_entry(&object, &entry));
    key = 23;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_next_entry(entry, &entry));
    const uintmax_t *check;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&key, check);
    assert_int_equal(key, *check);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_entry_error_on_entry_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_prev_entry(NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_entry_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_prev_entry((void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_entry_error_end_of_sequence(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_first_entry(&object, &entry));
    assert_false(coral_red_black_tree_map_prev_entry(entry, &entry));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_END_OF_SEQUENCE,
                     coral_error);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_prev_entry(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    uintmax_t key = 34;
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_first_entry(&object, &entry));
    key = 12;
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    assert_true(coral_red_black_tree_map_prev_entry(entry, &entry));
    const uintmax_t *check;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&key, check);
    assert_int_equal(key, *check);
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_key_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_key(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_key_error_on_entry_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_key(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_key_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_key(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_key(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_first_entry(&object, &entry));
    const uintmax_t *check;
    assert_true(coral_red_black_tree_map_entry_key(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&key, check);
    assert_int_equal(key, *check);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_get_value_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_get_value(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_get_value_error_on_entry_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_get_value(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_get_value_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_get_value(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_get_value(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    const uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_first_entry(&object, &entry));
    const uintmax_t *check;
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_set_value_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_set_value(
            NULL, (void *) 1, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_set_value_error_on_entry_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_set_value(
            (void *) 1, NULL, (void *) 1));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_set_value_error_on_value_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_red_black_tree_map_entry_set_value(
            (void *) 1, (void *) 1, NULL));
    assert_int_equal(CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL,
                     coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_entry_set_value(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_red_black_tree_map object;
    assert_true(coral_red_black_tree_map_init(
            &object,
            sizeof(uintmax_t),
            sizeof(uintmax_t),
            uintmax_t_ptr_compare));
    const uintmax_t key = (rand() % UINTMAX_MAX);
    uintmax_t value = (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_add(&object, &key, &value));
    const struct coral_red_black_tree_map_entry *entry;
    assert_true(coral_red_black_tree_map_first_entry(&object, &entry));
    const uintmax_t *check;
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    value += (rand() % UINTMAX_MAX);
    assert_true(coral_red_black_tree_map_entry_set_value(
            &object, entry, &value));
    assert_true(coral_red_black_tree_map_entry_get_value(
            &object, entry, (const void **) &check));
    assert_ptr_not_equal(&value, check);
    assert_int_equal(value, *check);
    assert_true(coral_red_black_tree_map_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_invalidate_error_on_object_is_null),
            cmocka_unit_test(check_invalidate),
            cmocka_unit_test(check_init_error_on_object_is_null),
            cmocka_unit_test(check_init_error_on_key_size_is_zero),
            cmocka_unit_test(check_init_error_on_value_size_is_zero),
            cmocka_unit_test(check_init_error_on_compare_is_null),
            cmocka_unit_test(check_init_error_on_entry_size_is_too_large),
            cmocka_unit_test(check_init),
            cmocka_unit_test(check_init_with_padding),
            cmocka_unit_test(check_key_size_error_on_object_is_null),
            cmocka_unit_test(check_key_size_error_on_out_is_null),
            cmocka_unit_test(check_key_size),
            cmocka_unit_test(check_value_size_error_on_object_is_null),
            cmocka_unit_test(check_value_size_error_on_out_is_null),
            cmocka_unit_test(check_value_size),
            cmocka_unit_test(check_count_error_on_object_is_null),
            cmocka_unit_test(check_count_error_on_out_is_null),
            cmocka_unit_test(check_count),
            cmocka_unit_test(check_add_error_on_object_is_null),
            cmocka_unit_test(check_add_error_on_key_is_null),
            cmocka_unit_test(check_add_error_on_value_is_null),
            cmocka_unit_test(check_add_error_on_memory_allocation_failed),
            cmocka_unit_test(check_add),
            cmocka_unit_test(check_add_error_on_key_already_exists),
            cmocka_unit_test(check_remove_error_on_object_is_null),
            cmocka_unit_test(check_remove_error_on_key_is_null),
            cmocka_unit_test(check_remove_error_on_key_not_found),
            cmocka_unit_test(check_remove),
            cmocka_unit_test(check_remove_error_on_memory_allocation_failed),
            cmocka_unit_test(check_contains_error_on_object_is_null),
            cmocka_unit_test(check_contains_error_on_key_is_null),
            cmocka_unit_test(check_contains_error_on_out_is_null),
            cmocka_unit_test(check_contains),
            cmocka_unit_test(check_contains_error_on_memory_allocation_failed),
            cmocka_unit_test(check_get_error_on_object_is_null),
            cmocka_unit_test(check_get_error_on_key_is_null),
            cmocka_unit_test(check_get_error_on_out_is_null),
            cmocka_unit_test(check_get_error_on_key_not_found),
            cmocka_unit_test(check_get),
            cmocka_unit_test(check_get_error_on_memory_allocation_failed),
            cmocka_unit_test(check_ceiling_error_on_object_is_null),
            cmocka_unit_test(check_ceiling_error_on_key_is_null),
            cmocka_unit_test(check_ceiling_error_on_out_is_null),
            cmocka_unit_test(check_ceiling_error_on_key_not_found),
            cmocka_unit_test(check_ceiling),
            cmocka_unit_test(check_ceiling_error_on_memory_allocation_failed),
            cmocka_unit_test(check_floor_error_on_object_is_null),
            cmocka_unit_test(check_floor_error_on_key_is_null),
            cmocka_unit_test(check_floor_error_on_out_is_null),
            cmocka_unit_test(check_floor_error_on_key_not_found),
            cmocka_unit_test(check_floor),
            cmocka_unit_test(check_floor_error_on_memory_allocation_failed),
            cmocka_unit_test(check_higher_error_on_object_is_null),
            cmocka_unit_test(check_higher_error_on_key_is_null),
            cmocka_unit_test(check_higher_error_on_out_is_null),
            cmocka_unit_test(check_higher_error_on_key_not_found),
            cmocka_unit_test(check_higher),
            cmocka_unit_test(check_higher_error_on_memory_allocation_failed),
            cmocka_unit_test(check_lower_error_on_object_is_null),
            cmocka_unit_test(check_lower_error_on_key_is_null),
            cmocka_unit_test(check_lower_error_on_out_is_null),
            cmocka_unit_test(check_lower_error_on_key_not_found),
            cmocka_unit_test(check_lower),
            cmocka_unit_test(check_lower_error_on_memory_allocation_failed),
            cmocka_unit_test(check_set_error_on_object_is_null),
            cmocka_unit_test(check_set_error_on_key_is_null),
            cmocka_unit_test(check_set_error_on_value_is_null),
            cmocka_unit_test(check_set_error_on_key_not_found),
            cmocka_unit_test(check_set),
            cmocka_unit_test(check_set_error_on_memory_allocation_failed),
            cmocka_unit_test(check_first_error_on_object_is_null),
            cmocka_unit_test(check_first_error_on_out_is_null),
            cmocka_unit_test(check_first_error_on_map_is_empty),
            cmocka_unit_test(check_first),
            cmocka_unit_test(check_last_error_on_object_is_null),
            cmocka_unit_test(check_last_error_on_out_is_null),
            cmocka_unit_test(check_last_error_on_map_is_empty),
            cmocka_unit_test(check_last),
            cmocka_unit_test(check_get_entry_error_on_object_is_null),
            cmocka_unit_test(check_get_entry_error_on_key_is_null),
            cmocka_unit_test(check_get_entry_error_on_out_is_null),
            cmocka_unit_test(check_get_entry_error_on_key_not_found),
            cmocka_unit_test(check_get_entry),
            cmocka_unit_test(check_get_entry_error_on_memory_allocation_failed),
            cmocka_unit_test(check_ceiling_entry_error_on_object_is_null),
            cmocka_unit_test(check_ceiling_entry_error_on_key_is_null),
            cmocka_unit_test(check_ceiling_entry_error_on_out_is_null),
            cmocka_unit_test(check_ceiling_entry_error_on_key_not_found),
            cmocka_unit_test(check_ceiling_entry),
            cmocka_unit_test(check_ceiling_entry_error_on_memory_allocation_failed),
            cmocka_unit_test(check_floor_entry_error_on_object_is_null),
            cmocka_unit_test(check_floor_entry_error_on_key_is_null),
            cmocka_unit_test(check_floor_entry_error_on_out_is_null),
            cmocka_unit_test(check_floor_entry_error_on_key_not_found),
            cmocka_unit_test(check_floor_entry),
            cmocka_unit_test(check_floor_entry_error_on_memory_allocation_failed),
            cmocka_unit_test(check_higher_entry_error_on_object_is_null),
            cmocka_unit_test(check_higher_entry_error_on_key_is_null),
            cmocka_unit_test(check_higher_entry_error_on_out_is_null),
            cmocka_unit_test(check_higher_entry_error_on_key_not_found),
            cmocka_unit_test(check_higher_entry),
            cmocka_unit_test(check_higher_entry_error_on_memory_allocation_failed),
            cmocka_unit_test(check_lower_entry_error_on_object_is_null),
            cmocka_unit_test(check_lower_entry_error_on_key_is_null),
            cmocka_unit_test(check_lower_entry_error_on_out_is_null),
            cmocka_unit_test(check_lower_entry_error_on_key_not_found),
            cmocka_unit_test(check_lower_entry),
            cmocka_unit_test(check_lower_entry_error_on_memory_allocation_failed),
            cmocka_unit_test(check_first_entry_error_on_object_is_null),
            cmocka_unit_test(check_first_entry_error_on_out_is_null),
            cmocka_unit_test(check_first_entry_error_on_map_is_empty),
            cmocka_unit_test(check_first_entry),
            cmocka_unit_test(check_last_entry_error_on_object_is_null),
            cmocka_unit_test(check_last_entry_error_on_out_is_null),
            cmocka_unit_test(check_last_entry_error_on_map_is_empty),
            cmocka_unit_test(check_last_entry),
            cmocka_unit_test(check_remove_entry_error_on_object_is_null),
            cmocka_unit_test(check_remove_entry_error_on_entry_is_null),
            cmocka_unit_test(check_remove_entry),
            cmocka_unit_test(check_next_entry_error_on_entry_is_null),
            cmocka_unit_test(check_next_entry_error_on_out_is_null),
            cmocka_unit_test(check_next_entry_error_end_of_sequence),
            cmocka_unit_test(check_next_entry),
            cmocka_unit_test(check_prev_entry_error_on_entry_is_null),
            cmocka_unit_test(check_prev_entry_error_on_out_is_null),
            cmocka_unit_test(check_prev_entry_error_end_of_sequence),
            cmocka_unit_test(check_prev_entry),
            cmocka_unit_test(check_entry_key_error_on_object_is_null),
            cmocka_unit_test(check_entry_key_error_on_entry_is_null),
            cmocka_unit_test(check_entry_key_error_on_out_is_null),
            cmocka_unit_test(check_entry_key),
            cmocka_unit_test(check_entry_get_value_error_on_object_is_null),
            cmocka_unit_test(check_entry_get_value_error_on_entry_is_null),
            cmocka_unit_test(check_entry_get_value_error_on_out_is_null),
            cmocka_unit_test(check_entry_get_value),
            cmocka_unit_test(check_entry_set_value_error_on_object_is_null),
            cmocka_unit_test(check_entry_set_value_error_on_entry_is_null),
            cmocka_unit_test(check_entry_set_value_error_on_value_is_null),
            cmocka_unit_test(check_entry_set_value)
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
