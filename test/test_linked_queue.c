#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <time.h>
#include <coral.h>

#include "test/cmocka.h"

static void check_invalidate_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_invalidate(NULL, (void *) 1));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_invalidate(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object = {};
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_init(NULL, sizeof(void *)));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_size_is_zero(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_init((void *) 1, 0));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_SIZE_IS_ZERO, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init_error_on_size_is_too_large(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_init((void *) 1, SIZE_MAX));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_SIZE_IS_TOO_LARGE, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_init(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(uintmax_t)));
    assert_int_equal(object.count, 0);
    assert_null(object.head);
    assert_null(object.tail);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_size_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_size(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_size_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_size((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_size(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    const size_t SIZE = sizeof(uintmax_t) + (rand() % UINT16_MAX);
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, SIZE));
    size_t out = 0;
    assert_true(coral_linked_queue_size(&object, &out));
    assert_int_equal(out, SIZE);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_count_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_count(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_count_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_count((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_count(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(void *)));
    object.count = rand() % UINTMAX_MAX;
    uintmax_t out = 0;
    assert_true(coral_linked_queue_count(&object, &out));
    assert_int_equal(out, object.count);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_add(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_item_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_add((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_ITEM_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_add_error_on_memory_allocation_failed(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(uintmax_t)));
    posix_memalign_is_overridden = true;
    uintmax_t value = 1;
    assert_false(coral_linked_queue_add(&object, &value));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_MEMORY_ALLOCATION_FAILED,
                     coral_error);
    posix_memalign_is_overridden = false;
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_add(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(uintmax_t)));
    assert_null(object.head);
    assert_null(object.tail);
    uintmax_t value = 1;
    assert_true(coral_linked_queue_add(&object, &value));
    assert_non_null(object.head);
    assert_ptr_equal(object.head, object.tail);
    assert_int_equal(object.count, 1);
    assert_true(coral_linked_queue_add(&object, &value));
    assert_non_null(object.head);
    assert_ptr_not_equal(object.head, object.tail);
    assert_int_equal(object.count, 2);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_remove(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_remove((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove_error_on_queue_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(uintmax_t)));
    uintmax_t out;
    assert_false(coral_linked_queue_remove(&object, (void **) &out));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY, coral_error);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_remove(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(uintmax_t)));
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
        assert_true(coral_linked_queue_add(&object, &value[i]));
        assert_int_equal(object.count, 1 + i);
    }
    uintmax_t out;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_non_null(object.head);
        assert_non_null(object.tail);
        assert_int_equal(object.count, limit - i);
        assert_true(coral_linked_queue_remove(&object, (void **) &out));
        assert_int_equal(out, value[i]);
    }
    assert_null(object.head);
    assert_null(object.tail);
    assert_int_equal(object.count, 0);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek_error_on_object_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_peek(NULL, (void *)1));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek_error_on_out_is_null(void **state) {
    coral_error = CORAL_ERROR_NONE;
    assert_false(coral_linked_queue_peek((void *)1, NULL));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL, coral_error);
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek_error_on_queue_is_empty(void **state) {
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(uintmax_t)));
    uintmax_t out;
    assert_false(coral_linked_queue_peek(&object, (void **) &out));
    assert_int_equal(CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY, coral_error);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
    coral_error = CORAL_ERROR_NONE;
}

static void check_peek(void **state) {
    srand(time(NULL));
    coral_error = CORAL_ERROR_NONE;
    struct coral_linked_queue object;
    assert_true(coral_linked_queue_init(&object, sizeof(uintmax_t)));
    uintmax_t value[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const size_t limit = sizeof(value) / sizeof(uintmax_t);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_true(coral_linked_queue_add(&object, &value[i]));
    }
    uintmax_t out;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_non_null(object.head);
        assert_non_null(object.tail);
        assert_true(coral_linked_queue_peek(&object, (void **) &out));
        assert_int_equal(out, value[0]);
    }
    assert_non_null(object.head);
    assert_non_null(object.tail);
    assert_int_equal(object.count, limit);
    assert_true(coral_linked_queue_invalidate(&object, NULL));
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
            cmocka_unit_test(check_add_error_on_object_is_null),
            cmocka_unit_test(check_add_error_on_item_is_null),
            cmocka_unit_test(check_add_error_on_memory_allocation_failed),
            cmocka_unit_test(check_add),
            cmocka_unit_test(check_remove_error_on_object_is_null),
            cmocka_unit_test(check_remove_error_on_out_is_null),
            cmocka_unit_test(check_remove_error_on_queue_is_empty),
            cmocka_unit_test(check_remove),
            cmocka_unit_test(check_peek_error_on_object_is_null),
            cmocka_unit_test(check_peek_error_on_out_is_null),
            cmocka_unit_test(check_peek_error_on_queue_is_empty),
            cmocka_unit_test(check_peek),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
