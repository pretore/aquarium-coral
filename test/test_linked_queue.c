#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <time.h>
#include <coral.h>

#include <test/cmocka.h>

static void check_invalidate_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_invalidate(NULL, (void *) 1),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_invalidate(void **state) {
    struct coral_linked_queue object = {};
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_init_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_init(NULL, sizeof(void *)),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_init_error_on_size_is_zero(void **state) {
    assert_int_equal(
            coral_linked_queue_init((void *) 1, 0),
            CORAL_LINKED_QUEUE_ERROR_SIZE_IS_ZERO);
}

static void check_init_error_on_size_is_too_large(void **state) {
    assert_int_equal(
            coral_linked_queue_init((void *) 1, SIZE_MAX),
            CORAL_LINKED_QUEUE_ERROR_SIZE_IS_TOO_LARGE);
}

static void check_init(void **state) {
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(uintmax_t)), 0);
    assert_int_equal(object.count, 0);
    assert_null(object.head);
    assert_null(object.tail);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_size_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_size(NULL, (void *)1),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_size_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_size((void *)1, NULL),
            CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_size(void **state) {
    srand(time(NULL));
    const size_t check = sizeof(uintmax_t) + (rand() % UINT16_MAX);
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, check), 0);
    size_t out = 0;
    assert_int_equal(coral_linked_queue_size(&object, &out), 0);
    assert_int_equal(out, check);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_count_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_count(NULL, (void *)1),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_count_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_count((void *)1, NULL),
            CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_count(void **state) {
    srand(time(NULL));
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(void *)), 0);
    object.count = rand() % UINTMAX_MAX;
    uintmax_t out = 0;
    assert_int_equal(coral_linked_queue_count(&object, &out), 0);
    assert_int_equal(out, object.count);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_add_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_add(NULL, (void *)1),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_add_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_add((void *)1, NULL),
            CORAL_LINKED_QUEUE_ERROR_ITEM_IS_NULL);
}

static void check_add_error_on_memory_allocation_failed(void **state) {
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t value = 1;
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(coral_linked_queue_add(&object, &value),
                     CORAL_LINKED_QUEUE_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_add(void **state) {
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(uintmax_t)), 0);
    assert_null(object.head);
    assert_null(object.tail);
    uintmax_t value = 1;
    assert_int_equal(coral_linked_queue_add(&object, &value), 0);
    assert_non_null(object.head);
    assert_ptr_equal(object.head, object.tail);
    assert_int_equal(object.count, 1);
    assert_int_equal(coral_linked_queue_add(&object, &value), 0);
    assert_non_null(object.head);
    assert_ptr_not_equal(object.head, object.tail);
    assert_int_equal(object.count, 2);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_remove_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_remove(NULL, (void *)1),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_remove_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_remove((void *)1, NULL),
            CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_remove_error_on_queue_is_empty(void **state) {
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t out;
    assert_int_equal(
            coral_linked_queue_remove(&object, (void **) &out),
            CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_remove(void **state) {
    srand(time(NULL));
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(uintmax_t)), 0);
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
        assert_int_equal(coral_linked_queue_add(&object, &value[i]), 0);
        assert_int_equal(object.count, 1 + i);
    }
    uintmax_t out;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_non_null(object.head);
        assert_non_null(object.tail);
        assert_int_equal(object.count, limit - i);
        assert_int_equal(coral_linked_queue_remove(&object, (void **) &out), 0);
        assert_int_equal(out, value[i]);
    }
    assert_null(object.head);
    assert_null(object.tail);
    assert_int_equal(object.count, 0);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_peek_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_peek(NULL, (void *)1),
            CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_peek_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_queue_peek((void *)1, NULL),
            CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_peek_error_on_queue_is_empty(void **state) {
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t out;
    assert_int_equal(
            coral_linked_queue_peek(&object, (void **) &out),
            CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
}

static void check_peek(void **state) {
    srand(time(NULL));
    struct coral_linked_queue object;
    assert_int_equal(coral_linked_queue_init(&object, sizeof(uintmax_t)), 0);
    uintmax_t value[] = {
            rand() % UINTMAX_MAX,
            rand() % UINTMAX_MAX,
    };
    const size_t limit = sizeof(value) / sizeof(uintmax_t);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_queue_add(&object, &value[i]), 0);
    }
    uintmax_t out;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_non_null(object.head);
        assert_non_null(object.tail);
        assert_int_equal(coral_linked_queue_peek(&object, (void **) &out), 0);
        assert_int_equal(out, value[0]);
    }
    assert_non_null(object.head);
    assert_non_null(object.tail);
    assert_int_equal(object.count, limit);
    assert_int_equal(coral_linked_queue_invalidate(&object, NULL), 0);
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
