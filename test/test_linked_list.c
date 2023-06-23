#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <coral.h>

#include <test/cmocka.h>

static void check_free_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_free(NULL),
            CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL);
}

static void check_free(void **state) {
    struct coral_linked_list_item *item;
    assert_int_equal(coral_linked_list_alloc(1, &item), 0);
    assert_int_equal(coral_linked_list_free(item), 0);
}

static void check_alloc_error_on_size_is_zero(void **state) {
    assert_int_equal(
            coral_linked_list_alloc(0, (void *) 1),
            CORAL_LINKED_LIST_ERROR_SIZE_IS_ZERO);
}

static void check_alloc_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_alloc(1, NULL),
            CORAL_LINKED_LIST_ERROR_OUT_IS_NULL);
}

static void check_alloc(void **state) {
    struct coral_linked_list_item *item;
    assert_int_equal(coral_linked_list_alloc(1, &item), 0);
    assert_int_equal(coral_linked_list_free(item), 0);
}

static void check_alloc_error_on_size_is_too_large(void **state) {
    assert_int_equal(
            coral_linked_list_alloc(UINTMAX_MAX, (void *) 1),
            CORAL_LINKED_LIST_ERROR_SIZE_IS_TOO_LARGE);
}

static void check_alloc_error_on_memory_allocation_failed(void **state) {
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_linked_list_alloc(1, (void *) 1),
            CORAL_LINKED_LIST_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
}

static void check_invalidate_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_invalidate(NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_invalidate(void **state) {
    struct coral_linked_list object = {};
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_init_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_init(NULL),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_init(void **state) {
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    assert_null(object.list);
    assert_int_equal(object.count, 0);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_count_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_count(NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_count_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_count((void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_OUT_IS_NULL);
}

static void check_count(void **state) {
    srand(time(NULL));
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    object.count = abs(rand());
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(object.count, count);
    object.count = 0;
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_add_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_add(NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_add_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_add((void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL);
}

static void check_add(void **state) {
    srand(time(NULL));
    const uintmax_t limit = 2;
    struct coral_linked_list_item *items[limit];
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_list_alloc(
                sizeof(uintmax_t), &items[i]), 0);
    }
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_list_add(&object, items[i]), 0);
    }
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, limit);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_insert_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_insert(NULL, (void *) 1, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_insert_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_insert((void *) 1, NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL);
}

static void check_insert_error_on_value_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_insert((void *) 1, (void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_VALUE_IS_NULL);
}

static void check_insert(void **state) {
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    const uintmax_t limit = 2;
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr[2], check;
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_list_alloc(
                sizeof(uintmax_t), &ptr[i].item), 0);
    }
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    assert_int_equal(coral_linked_list_add(&object, ptr[0].item), 0);
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(coral_linked_list_insert(
            &object, ptr[0].item, ptr[1].item), 0);
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 2);
    assert_int_equal(coral_linked_list_first(&object, &check.item), 0);
    assert_ptr_equal(check.item, ptr[1].item);
    assert_int_equal(coral_linked_list_invalidate(&object, 0), 0);
}

static void check_remove_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_remove(NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_remove_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_remove((void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL);
}

static void check_remove_case_empty_list(void **state) {
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(coral_linked_list_alloc(sizeof(*ptr.value), &ptr.item), 0);
    assert_int_equal(coral_linked_list_remove(&object, ptr.item), 0);
    assert_int_equal(coral_linked_list_free(ptr.item), 0);
    assert_int_equal(coral_linked_list_invalidate(&object, 0), 0);
}

static void check_remove(void **state) {
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(coral_linked_list_alloc(sizeof(*ptr.value), &ptr.item), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    assert_int_equal(coral_linked_list_add(&object, ptr.item), 0);
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(coral_linked_list_remove(&object, ptr.item), 0);
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    assert_int_equal(coral_linked_list_free(ptr.item), 0);
    assert_int_equal(coral_linked_list_invalidate(&object, 0), 0);
}

static void check_first_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_first(NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_first_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_first((void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_OUT_IS_NULL);
}

static void check_first_error_on_list_is_empty(void **state) {
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(
            coral_linked_list_first(&object, &ptr.item),
            CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_first(void **state) {
    srand(time(NULL));
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr, first;
    assert_int_equal(coral_linked_list_alloc(sizeof(*ptr.value), &ptr.item), 0);
    assert_int_equal(coral_linked_list_add(&object, ptr.item), 0);
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(coral_linked_list_first(&object, &first.item), 0);
    assert_ptr_equal(first.item, ptr.item);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_last_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_last(NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_last_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_last((void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_OUT_IS_NULL);
}

static void check_last_error_on_list_is_empty(void **state) {
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr;
    assert_int_equal(
            coral_linked_list_last(&object, &ptr.item),
            CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_last(void **state) {
    srand(time(NULL));
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr, first;
    assert_int_equal(coral_linked_list_alloc(sizeof(*ptr.value), &ptr.item), 0);
    assert_int_equal(coral_linked_list_add(&object, ptr.item), 0);
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 1);
    assert_int_equal(coral_linked_list_last(&object, &first.item), 0);
    assert_ptr_equal(first.item, ptr.item);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_next_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_next(NULL, (void *) 1, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_next_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_next((void *) 1, NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL);
}

static void check_next_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_next((void *) 1, (void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_OUT_IS_NULL);
}

static void check_next_error_on_end_of_sequence(void **state) {
    srand(time(NULL));
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr, item;
    assert_int_equal(coral_linked_list_alloc(sizeof(*ptr.value), &ptr.item), 0);
    assert_int_equal(coral_linked_list_add(&object, ptr.item), 0);
    assert_int_equal(coral_linked_list_first(&object, &item.item), 0);
    assert_int_equal(
            coral_linked_list_next(&object, item.item, &item.item),
            CORAL_LINKED_LIST_ERROR_END_OF_SEQUENCE);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_next(void **state) {
    srand(time(NULL));
    const uintmax_t limit = 2;
    struct coral_linked_list_item *items[limit];
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_list_alloc(
                sizeof(uintmax_t), &items[i]), 0);
    }
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_list_add(&object, items[i]), 0);
    }
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, limit);
    struct coral_linked_list_item *item;
    assert_ptr_equal(coral_linked_list_first(&object, &item), 0);
    assert_ptr_equal(coral_linked_list_next(&object, item, &item), 0);
    assert_ptr_equal(items[1], item);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_prev_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_prev(NULL, (void *) 1, (void *) 1),
            CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL);
}

static void check_prev_error_on_item_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_prev((void *) 1, NULL, (void *) 1),
            CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL);
}

static void check_prev_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_linked_list_prev((void *) 1, (void *) 1, NULL),
            CORAL_LINKED_LIST_ERROR_OUT_IS_NULL);
}

static void check_prev_error_on_end_of_sequence(void **state) {
    srand(time(NULL));
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    union {
        struct coral_linked_list_item *item;
        uintmax_t *value;
    } ptr, item;
    assert_int_equal(coral_linked_list_alloc(sizeof(*ptr.value), &ptr.item), 0);
    assert_int_equal(coral_linked_list_add(&object, ptr.item), 0);
    assert_int_equal(coral_linked_list_last(&object, &item.item), 0);
    assert_int_equal(
            coral_linked_list_prev(&object, item.item, &item.item),
            CORAL_LINKED_LIST_ERROR_END_OF_SEQUENCE);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
}

static void check_prev(void **state) {
    srand(time(NULL));
    const uintmax_t limit = 2;
    struct coral_linked_list_item *items[limit];
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_list_alloc(
                sizeof(uintmax_t), &items[i]), 0);
    }
    struct coral_linked_list object;
    assert_int_equal(coral_linked_list_init(&object), 0);
    uintmax_t count;
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, 0);
    for (uintmax_t i = 0; i < limit; i++) {
        assert_int_equal(coral_linked_list_add(&object, items[i]), 0);
    }
    assert_int_equal(coral_linked_list_count(&object, &count), 0);
    assert_int_equal(count, limit);
    struct coral_linked_list_item *item;
    assert_ptr_equal(coral_linked_list_last(&object, &item), 0);
    assert_ptr_equal(coral_linked_list_prev(&object, item, &item), 0);
    assert_ptr_equal(items[0], item);
    assert_int_equal(coral_linked_list_invalidate(&object, NULL), 0);
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
            cmocka_unit_test(check_insert_error_on_object_is_null),
            cmocka_unit_test(check_insert_error_on_item_is_null),
            cmocka_unit_test(check_insert_error_on_value_is_null),
            cmocka_unit_test(check_insert),
            cmocka_unit_test(check_remove_error_on_object_is_null),
            cmocka_unit_test(check_remove_error_on_item_is_null),
            cmocka_unit_test(check_remove_case_empty_list),
            cmocka_unit_test(check_remove),
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
            cmocka_unit_test(check_next_error_on_end_of_sequence),
            cmocka_unit_test(check_next),
            cmocka_unit_test(check_prev_error_on_object_is_null),
            cmocka_unit_test(check_prev_error_on_item_is_null),
            cmocka_unit_test(check_prev_error_on_out_is_null),
            cmocka_unit_test(check_prev_error_on_end_of_sequence),
            cmocka_unit_test(check_prev),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
