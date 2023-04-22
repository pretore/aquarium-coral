#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <time.h>
#include <coral.h>

#include <test/cmocka.h>

static void check_free_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_free(NULL),
            CORAL_FORWARD_QUEUE_ERROR_ITEM_IS_NULL);
}

static void check_free(void **state) {
    struct coral_forward_queue_item *item;
    assert_int_equal(coral_forward_queue_alloc(1, &item), 0);
    assert_int_equal(coral_forward_queue_free(item), 0);
}

static void check_alloc_error_on_size_is_zero(void **state) {
    assert_int_equal(
            coral_forward_queue_alloc(0, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_SIZE_IS_ZERO);
}

static void check_alloc_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_alloc(1, NULL),
            CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_alloc(void **state) {
    srand(time(NULL));
    union {
        struct coral_forward_queue_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(coral_forward_queue_alloc(
            sizeof(*ptr.value), &ptr.item), 0);
    *ptr.value = rand();
    assert_int_equal(coral_forward_queue_free(ptr.item), 0);
}

static void check_alloc_error_on_size_is_too_large(void **state) {
    assert_int_equal(
            coral_forward_queue_alloc(SIZE_MAX, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_SIZE_IS_TOO_LARGE);
}

static void check_alloc_error_on_memory_allocation_failed(void **state) {
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_forward_queue_alloc(1, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
}

static void check_invalidate_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_invalidate(NULL, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_invalidate(void **state) {
    struct coral_forward_queue object = {};
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_init_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_init(NULL),
            CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_init(void **state) {
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    assert_int_equal(object.count, 0);
    assert_null(object.head);
    assert_null(object.tail);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_count_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_count(NULL, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_count_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_count((void *) 1, NULL),
            CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_count(void **state) {
    srand(time(NULL));
    const uintmax_t check = rand();
    struct coral_forward_queue object = {
            .count = check
    };
    uintmax_t count;
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, check);
}

static void check_add_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_add(NULL, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_add_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_add((void *) 1, NULL),
            CORAL_FORWARD_QUEUE_ERROR_ITEM_IS_NULL);
}

static void check_add(void **state) {
    srand(time(NULL));
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    union {
        struct coral_forward_queue_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(coral_forward_queue_alloc(
            sizeof(*ptr.value), &ptr.item), 0);
    *ptr.value = rand();
    uintmax_t count;
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(coral_forward_queue_add(&object, ptr.item), 0);
    assert_non_null(object.head);
    assert_ptr_equal(object.head, object.tail);
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_add_multiple(void **state) {
    srand(time(NULL));
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    union {
        struct coral_forward_queue_item *item;
        uintmax_t *value;
    } ptr;
    const uintmax_t limit = 5;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_forward_queue_alloc(
                sizeof(*ptr.value), &ptr.item), 0);
        *ptr.value = rand();
        assert_int_equal(coral_forward_queue_add(&object, ptr.item), 0);
    }
    uintmax_t count;
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, limit);
    assert_non_null(object.head);
    assert_ptr_not_equal(object.head, object.tail);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_remove_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_remove(NULL, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_remove_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_remove((void *) 1, NULL),
            CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_remove_error_on_queue_is_empty(void **state) {
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    assert_int_equal(
            coral_forward_queue_remove(&object, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_QUEUE_IS_EMPTY);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_remove(void **state) {
    srand(time(NULL));
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    union {
        struct coral_forward_queue_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(coral_forward_queue_alloc(
            sizeof(*ptr.value), &ptr.item), 0);
    *ptr.value = rand();
    const uintmax_t *check = ptr.value;
    assert_int_equal(coral_forward_queue_add(&object, ptr.item), 0);
    assert_non_null(object.head);
    assert_ptr_equal(object.head, object.tail);
    ptr.item = NULL;
    uintmax_t count;
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(coral_forward_queue_remove(&object, &ptr.item), 0);
    assert_null(object.head);
    assert_ptr_equal(object.head, object.tail);
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, 0);
    assert_ptr_equal(ptr.item, check);
    assert_ptr_equal(coral_forward_queue_free(ptr.item), 0);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_remove_multiple(void **state) {
    srand(time(NULL));
    const uintmax_t limit = 5;
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    union {
        struct coral_forward_queue_item *item;
        uintmax_t *value;
    } ptr[limit];
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_forward_queue_alloc(
                sizeof(*ptr[i].value), &ptr[i].item), 0);
        *ptr[i].value = rand();
        assert_int_equal(coral_forward_queue_add(&object, ptr[i].item), 0);
    }
    uintmax_t count;
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, limit);
    assert_ptr_not_equal(object.head, object.tail);
    struct coral_forward_queue_item *item;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_forward_queue_remove(&object, &item), 0);
        assert_ptr_equal(ptr[i].item, item);
        assert_ptr_equal(coral_forward_queue_free(item), 0);
    }
    assert_null(object.head);
    assert_null(object.tail);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_peek_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_peek(NULL, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL);
}

static void check_peek_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_forward_queue_peek((void *) 1, NULL),
            CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL);
}

static void check_peek_error_on_queue_is_empty(void **state) {
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    assert_int_equal(
            coral_forward_queue_peek(&object, (void *) 1),
            CORAL_FORWARD_QUEUE_ERROR_QUEUE_IS_EMPTY);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

static void check_peek(void **state) {
    srand(time(NULL));
    struct coral_forward_queue object;
    assert_int_equal(coral_forward_queue_init(&object), 0);
    union {
        struct coral_forward_queue_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(coral_forward_queue_alloc(
            sizeof(*ptr.value), &ptr.item), 0);
    *ptr.value = rand();
    const uintmax_t *check = ptr.value;
    assert_int_equal(coral_forward_queue_add(&object, ptr.item), 0);
    ptr.item = NULL;
    uintmax_t count;
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(coral_forward_queue_peek(&object, &ptr.item), 0);
    assert_int_equal(coral_forward_queue_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_ptr_equal(ptr.item, check);
    assert_int_equal(coral_forward_queue_invalidate(&object, NULL), 0);
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_free_error_on_item_is_null),
            cmocka_unit_test(check_free),
            cmocka_unit_test(check_alloc_error_on_size_is_zero),
            cmocka_unit_test(check_alloc_error_on_out_is_null),
            cmocka_unit_test(check_alloc),
            cmocka_unit_test(check_alloc_error_on_size_is_too_large),
            cmocka_unit_test(check_alloc_error_on_memory_allocation_failed),
            cmocka_unit_test(check_invalidate_error_on_object_is_null),
            cmocka_unit_test(check_invalidate),
            cmocka_unit_test(check_init_error_on_object_is_null),
            cmocka_unit_test(check_init),
            cmocka_unit_test(check_count_error_on_object_is_null),
            cmocka_unit_test(check_count_error_on_out_is_null),
            cmocka_unit_test(check_count),
            cmocka_unit_test(check_add_error_on_object_is_null),
            cmocka_unit_test(check_add_error_on_item_is_null),
            cmocka_unit_test(check_add),
            cmocka_unit_test(check_add_multiple),
            cmocka_unit_test(check_remove_error_on_object_is_null),
            cmocka_unit_test(check_remove_error_on_out_is_null),
            cmocka_unit_test(check_remove_error_on_queue_is_empty),
            cmocka_unit_test(check_remove),
            cmocka_unit_test(check_remove_multiple),
            cmocka_unit_test(check_peek_error_on_object_is_null),
            cmocka_unit_test(check_peek_error_on_out_is_null),
            cmocka_unit_test(check_peek_error_on_queue_is_empty),
            cmocka_unit_test(check_peek),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
