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
            coral_linked_stack_invalidate(NULL, (void *) 1),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_invalidate(void **state) {
    struct coral_linked_stack object = {};
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_init_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_init(NULL, sizeof(void *)),
            CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_init_error_on_size_is_zero(void **state) {
    assert_int_equal(
            coral_linked_stack_init((void *) 1, 0),
            CORAL_LINKED_STACK_ERROR_SIZE_IS_ZERO);
}

static void check_init_error_on_size_is_too_large(void **state) {
    assert_int_equal(
            coral_linked_stack_init((void *) 1, SIZE_MAX),
            CORAL_LINKED_STACK_ERROR_SIZE_IS_TOO_LARGE);
}

static void check_init(void **state) {
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(uintmax_t)), 0);
    assert_int_equal(object.count, 0);
    assert_null(object.top);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_size_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_size(NULL, (void *)1),
            CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_size_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_size((void *)1, NULL),
            CORAL_LINKED_STACK_ERROR_OUT_IS_NULL);
}

static void check_size(void **state) {
    srand(time(NULL));
    const size_t check = sizeof(uintmax_t) + (rand() % UINT16_MAX);
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, check), 0);
    size_t out = 0;
    assert_int_equal(coral_linked_stack_size(&object, &out), 0);
    assert_int_equal(out, check);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_count_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_count(NULL, (void *)1),
            CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_count_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_count((void *)1, NULL),
            CORAL_LINKED_STACK_ERROR_OUT_IS_NULL);
}

static void check_count(void **state) {
    srand(time(NULL));
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(void *)), 0);
    object.count = rand() % UINTMAX_MAX;
    uintmax_t out = 0;
    assert_int_equal(coral_linked_stack_count(&object, &out), 0);
    assert_int_equal(out, object.count);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_push_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_push(NULL, (void *)1),
            CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_push_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_push((void *)1, NULL),
            CORAL_LINKED_STACK_ERROR_ITEM_IS_NULL);
}

static void check_push_error_on_memory_allocation_failed(void **state) {
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t value = 1;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden =
            posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_linked_stack_push(&object, &value),
            CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden =
            posix_memalign_is_overridden = false;
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_push(void **state) {
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(uintmax_t)), 0);
    assert_null(object.top);
    uintmax_t value = 1;
    assert_int_equal(coral_linked_stack_push(&object, &value), 0);
    assert_non_null(object.top);
    assert_int_equal(object.count, 1);
    assert_int_equal(coral_linked_stack_push(&object, &value), 0);
    assert_int_equal(object.count, 2);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_pop_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_pop(NULL, (void *)1),
            CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_pop_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_pop((void *)1, NULL),
            CORAL_LINKED_STACK_ERROR_OUT_IS_NULL);
}

static void check_pop_error_on_stack_is_empty(void **state) {
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t out;
    assert_int_equal(
            coral_linked_stack_pop(&object, (void **) &out),
            CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_pop(void **state) {
    srand(time(NULL));
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(uintmax_t)), 0);
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
        assert_int_equal(coral_linked_stack_push(&object, &value[i]), 0);
        assert_int_equal(object.count, 1 + i);
    }
    uintmax_t out;
    for (uintmax_t i = 1; i <= limit; i++) {
        assert_non_null(object.top);
        assert_int_equal(object.count, limit - (i - 1));
        assert_int_equal(coral_linked_stack_pop(&object, (void **) &out), 0);
        assert_int_equal(out, value[limit - i]);
        assert_int_equal(object.count, limit - i);
    }
    assert_null(object.top);
    assert_int_equal(object.count, 0);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_peek_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_peek(NULL, (void *)1),
            CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_peek_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_stack_peek((void *)1, NULL),
            CORAL_LINKED_STACK_ERROR_OUT_IS_NULL);
}

static void check_peek_error_on_stack_is_empty(void **state) {
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t out;
    assert_int_equal(
            coral_linked_stack_peek(&object, (void **) &out),
            CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
}

static void check_peek(void **state) {
    srand(time(NULL));
    struct coral_linked_stack object;
    assert_int_equal(coral_linked_stack_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t value[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const size_t limit = sizeof(value) / sizeof(uintmax_t);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_stack_push(&object, &value[i]), 0);
    }
    uintmax_t out;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_non_null(object.top);
        assert_int_equal(coral_linked_stack_peek(&object, (void **) &out), 0);
        assert_int_equal(out, value[limit - 1]);
    }
    assert_non_null(object.top);
    assert_int_equal(object.count, limit);
    assert_int_equal(coral_linked_stack_invalidate(&object, NULL), 0);
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
