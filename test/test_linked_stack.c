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
    assert_false(coral_linked_stack_invalidate(NULL, (void *) 1));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_invalidate(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object = {};
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_init(NULL, sizeof(void *)));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_size_is_zero(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_init((void *) 1, 0));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_SIZE_IS_ZERO, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_size_is_too_large(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_init((void *) 1, SIZE_MAX));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_SIZE_IS_TOO_LARGE, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(uintmax_t)));
    assert_int_equal(object.count, 0);
    assert_null(object.top);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_size_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_size(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_size_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_size((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_size(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    const size_t SIZE = sizeof(uintmax_t) + (rand() % UINT16_MAX);
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, SIZE));
    size_t out = 0;
    assert_true(coral_linked_stack_size(&object, &out));
    assert_int_equal(out, SIZE);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_count_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_count(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_count_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_count((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_count(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(void *)));
    object.count = rand() % UINTMAX_MAX;
    uintmax_t out = 0;
    assert_true(coral_linked_stack_count(&object, &out));
    assert_int_equal(out, object.count);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_push_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_push(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_push_error_on_item_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_push((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_ITEM_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_push_error_on_memory_allocation_failed(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(uintmax_t)));
    posix_memalign_is_overridden = true;
    uintmax_t value = 1;
    assert_false(coral_linked_stack_push(&object, &value));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    posix_memalign_is_overridden = false;
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_push(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(uintmax_t)));
    assert_null(object.top);
    uintmax_t value = 1;
    assert_true(coral_linked_stack_push(&object, &value));
    assert_non_null(object.top);
    assert_int_equal(object.count, 1);
    assert_true(coral_linked_stack_push(&object, &value));
    assert_int_equal(object.count, 2);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_pop_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_pop(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_pop_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_pop((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_pop_error_on_stack_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(uintmax_t)));
    uintmax_t out;
    assert_false(coral_linked_stack_pop(&object, (void **) &out));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY, coral_error);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_pop(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(uintmax_t)));
    uintmax_t value[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const size_t limit = sizeof(value) / sizeof(uintmax_t);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(object.count, i);
        assert_true(coral_linked_stack_push(&object, &value[i]));
        assert_int_equal(object.count, 1 + i);
    }
    uintmax_t out;
    for (uintmax_t i = 1; i <= limit; i++) {
        assert_non_null(object.top);
        assert_int_equal(object.count, limit - (i - 1));
        assert_true(coral_linked_stack_pop(&object, (void **) &out));
        assert_int_equal(out, value[limit - i]);
        assert_int_equal(object.count, limit - i);
    }
    assert_null(object.top);
    assert_int_equal(object.count, 0);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_peek(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_stack_peek((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek_error_on_stack_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(uintmax_t)));
    uintmax_t out;
    assert_false(coral_linked_stack_peek(&object, (void **) &out));
    assert_int_equal(CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY, coral_error);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_stack object;
    assert_true(coral_linked_stack_init(&object, sizeof(uintmax_t)));
    uintmax_t value[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const size_t limit = sizeof(value) / sizeof(uintmax_t);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_true(coral_linked_stack_push(&object, &value[i]));
    }
    uintmax_t out;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_non_null(object.top);
        assert_true(coral_linked_stack_peek(&object, (void **) &out));
        assert_int_equal(out, value[limit - 1]);
    }
    assert_non_null(object.top);
    assert_int_equal(object.count, limit);
    assert_true(coral_linked_stack_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_invalidate_error_on_object_is_null),
            cmocka_unit_test(check_invalidate),
            cmocka_unit_test(check_init_error_on_object_is_null),
            cmocka_unit_test(check_init_error_on_size_is_zero),
            cmocka_unit_test(check_init_error_on_size_is_too_large),
            cmocka_unit_test(check_init),
            cmocka_unit_test(check_size_error_on_object_is_null),
            cmocka_unit_test(check_size_error_on_out_is_null),
            cmocka_unit_test(check_size),
            cmocka_unit_test(check_count_error_on_object_is_null),
            cmocka_unit_test(check_count_error_on_out_is_null),
            cmocka_unit_test(check_count),
            cmocka_unit_test(check_push_error_on_object_is_null),
            cmocka_unit_test(check_push_error_on_item_is_null),
            cmocka_unit_test(check_push_error_on_memory_allocation_failed),
            cmocka_unit_test(check_push),
            cmocka_unit_test(check_pop_error_on_object_is_null),
            cmocka_unit_test(check_pop_error_on_out_is_null),
            cmocka_unit_test(check_pop_error_on_stack_is_empty),
            cmocka_unit_test(check_pop),
            cmocka_unit_test(check_peek_error_on_object_is_null),
            cmocka_unit_test(check_peek_error_on_out_is_null),
            cmocka_unit_test(check_peek_error_on_stack_is_empty),
            cmocka_unit_test(check_peek),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
