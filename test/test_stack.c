#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <coral.h>

#include "private/stack.h"

#include <test/cmocka.h>

static void check_push_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_stack_push(NULL, (void *)1),
            CORAL_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_push_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_stack_push((void *) 1, NULL),
            CORAL_STACK_ERROR_ITEM_IS_NULL);
}

static void check_push(void **state) {
    struct coral_stack object = {};
    assert_null(object.top);
    void *item = &object;
    assert_int_equal(coral_stack_push(&object, &item), 0);
    assert_non_null(object.top);
    assert_ptr_not_equal(&object, object.top);
    free(object.top); /* clean up */
}

static void check_push_error_on_memory_allocation_failed(void **state) {
    struct coral_stack object = {};
    assert_null(object.top);
    void *item = &object;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden =
            posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_stack_push(&object, &item),
            CORAL_STACK_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden =
            posix_memalign_is_overridden = false;
}

static void check_pop_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_stack_pop(NULL, (void *)1),
            CORAL_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_pop_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_stack_pop((void *) 1, NULL),
            CORAL_STACK_ERROR_OUT_IS_NULL);
}

static void check_pop_error_on_stack_is_empty(void **state) {
    struct coral_stack object = {};
    struct coral_stack *out = NULL;
    assert_int_equal(
            coral_stack_pop(&object, (void **) &out),
            CORAL_STACK_ERROR_STACK_IS_EMPTY);
}

static void check_pop(void **state) {
    struct coral_stack object = {};
    assert_null(object.top);
    void *item = &object;
    assert_int_equal(coral_stack_push(&object, &item), 0);
    assert_non_null(object.top);
    assert_ptr_not_equal(&object, object.top);
    struct coral_stack *out = NULL;
    assert_int_equal(coral_stack_pop(&object, (void **) &out), 0);
    assert_ptr_equal(item, out);
    assert_null(object.top);
}

static void check_peer_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_stack_peek(NULL, (void *) 1),
            CORAL_STACK_ERROR_OBJECT_IS_NULL);
}

static void check_peer_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_stack_peek((void *) 1, NULL),
            CORAL_STACK_ERROR_OUT_IS_NULL);
}

static void check_peek_error_on_stack_is_empty(void **state) {
    struct coral_stack object = {};
    struct coral_stack *out = NULL;
    assert_int_equal(
            coral_stack_peek(&object, (void **) &out),
            CORAL_STACK_ERROR_STACK_IS_EMPTY);
}

static void check_peek(void **state) {
    struct coral_stack object = {};
    assert_null(object.top);
    void *item = &object;
    assert_int_equal(coral_stack_push(&object, &item), 0);
    assert_non_null(object.top);
    assert_ptr_not_equal(&object, object.top);
    struct coral_stack *out = NULL;
    assert_int_equal(coral_stack_peek(&object, (void **) &out), 0);
    assert_ptr_equal(item, out);
    assert_non_null(object.top);
    free(object.top); /* clean up */
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_push_error_on_object_is_null),
            cmocka_unit_test(check_push_error_on_item_is_null),
            cmocka_unit_test(check_push),
            cmocka_unit_test(check_push_error_on_memory_allocation_failed),
            cmocka_unit_test(check_pop_error_on_object_is_null),
            cmocka_unit_test(check_pop_error_on_out_is_null),
            cmocka_unit_test(check_pop_error_on_stack_is_empty),
            cmocka_unit_test(check_pop),
            cmocka_unit_test(check_peer_error_on_object_is_null),
            cmocka_unit_test(check_peer_error_on_out_is_null),
            cmocka_unit_test(check_peek_error_on_stack_is_empty),
            cmocka_unit_test(check_peek),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
