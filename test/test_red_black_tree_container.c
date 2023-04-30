#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <coral.h>
#include <seagrass.h>

#include <test/cmocka.h>

static void check_free_error_on_entry_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_free(NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL);
}

static void check_free(void **state) {
    struct coral_red_black_tree_container_entry *entry;
    assert_int_equal(coral_red_black_tree_container_alloc(1, &entry), 0);
    assert_int_equal(coral_red_black_tree_container_free(entry), 0);
}

static void check_alloc_error_on_size_is_zero(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_alloc(0, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_ZERO);
}

static void check_alloc_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_alloc(1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_alloc(void **state) {
    struct coral_red_black_tree_container_entry *entry;
    assert_int_equal(coral_red_black_tree_container_alloc(1, &entry), 0);
    assert_int_equal(coral_red_black_tree_container_free(entry), 0);
}

static void check_alloc_error_on_size_is_too_large(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_alloc(UINTMAX_MAX, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_TOO_LARGE);
}

static void check_alloc_error_on_memory_allocation_failed(void **state) {
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = true;
    assert_int_equal(
            coral_red_black_tree_container_alloc(1, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_MEMORY_ALLOCATION_FAILED);
    malloc_is_overridden = calloc_is_overridden = realloc_is_overridden
            = posix_memalign_is_overridden = false;
}

static void check_invalidate_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_invalidate(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_invalidate(void **state) {
    struct coral_red_black_tree_container object = {};
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_init_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_init(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_init_error_on_compare_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_init((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_COMPARE_IS_NULL);
}

static void check_init(void **state) {
    srand(time(NULL));
    const uintmax_t check = rand();
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(
            &object, (void *) check), 0);
    assert_int_equal(check, object.compare);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_count_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_count(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_count_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_count((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_count(void **state) {
    srand(time(NULL));
    uintmax_t check = rand();
    struct coral_red_black_tree_container object = {
            .tree.count = check
    };
    uintmax_t count;
    assert_int_equal(coral_red_black_tree_container_count(
            &object, &count), 0);
    assert_int_equal(check, count);
}

static void check_add_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_add(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_add_error_on_entry_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_add((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL);
}

static int compare(const void *a, const void *b) {
    return seagrass_uintmax_t_ptr_compare(a, b);
}

static void check_add(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item;
    assert_int_equal(coral_red_black_tree_container_alloc(
            sizeof(*item.value), &item.entry), 0);
    *item.value = 0;
    assert_int_equal(coral_red_black_tree_container_add(
            &object, item.entry), 0);
    assert_int_equal(0, *item.value);
    uintmax_t count;
    assert_int_equal(coral_red_black_tree_container_count(
            &object, &count), 0);
    assert_int_equal(1, count);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_add_error_on_entry_already_exits(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(
            &object, compare), 0);
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item;
    assert_int_equal(coral_red_black_tree_container_alloc(
            sizeof(*item.value), &item.entry), 0);
    *item.value = 0;
    assert_int_equal(
            coral_red_black_tree_container_add(&object, item.entry), 0);
    assert_int_equal(
            coral_red_black_tree_container_add(&object, item.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXISTS);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_remove_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_remove(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_remove_error_on_entry_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_remove((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL);
}

static void check_remove(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item;
    assert_int_equal(coral_red_black_tree_container_alloc(
            sizeof(*item.value), &item.entry), 0);
    *item.value = 0;
    assert_int_equal(coral_red_black_tree_container_add(
            &object, item.entry), 0);
    uintmax_t count;
    assert_int_equal(coral_red_black_tree_container_count(
            &object, &count), 0);
    assert_int_equal(1, count);
    assert_int_equal(coral_red_black_tree_container_remove(
            &object, item.entry), 0);
    assert_int_equal(coral_red_black_tree_container_count(
            &object, &count), 0);
    assert_int_equal(0, count);
    assert_int_equal(coral_red_black_tree_container_free(
            item.entry), 0);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_get_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_get(NULL, (void *) 1, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_get_error_on_key_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_get((void *) 1, NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL);
}

static void check_get_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_get((void *) 1, (void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_get(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    const uintmax_t key = *item[1].value;
    union entry other;
    assert_int_equal(coral_red_black_tree_container_get(
            &object, &key, &other.entry), 0);
    assert_int_equal(*item[1].value, *other.value);
    assert_ptr_equal(item[1].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_get_error_on_entry_not_found(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    const uintmax_t key = UINTMAX_MAX;
    union entry other;
    assert_int_equal(
            coral_red_black_tree_container_get(&object, &key, &other.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_ceiling_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_ceiling(
                    NULL, (void *) 1, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_ceiling_error_on_key_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_ceiling(
                    (void *) 1, NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL);
}

static void check_ceiling_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_ceiling(
                    (void *) 1, (void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_ceiling(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    /* case: exact match */
    uintmax_t key = *item[1].value;
    union entry other;
    assert_int_equal(coral_red_black_tree_container_ceiling(
            &object, &key, &other.entry), 0);
    assert_int_equal(*item[1].value, *other.value);
    assert_ptr_equal(item[1].entry, other.entry);
    /* case: higher match */
    key = 3;
    assert_int_equal(coral_red_black_tree_container_ceiling(
            &object, &key, &other.entry), 0);
    assert_int_equal(*item[2].value, *other.value);
    assert_ptr_equal(item[2].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_ceiling_error_on_entry_not_found(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    const uintmax_t key = UINTMAX_MAX;
    union entry other;
    assert_int_equal(
            coral_red_black_tree_container_ceiling(&object, &key, &other.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_floor_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_floor(
                    NULL, (void *) 1, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_floor_error_on_key_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_floor(
                    (void *) 1, NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL);
}

static void check_floor_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_floor(
                    (void *) 1, (void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_floor(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    /* case: exact match */
    uintmax_t key = *item[2].value;
    union entry other;
    assert_int_equal(coral_red_black_tree_container_floor(
            &object, &key, &other.entry), 0);
    assert_int_equal(*item[2].value, *other.value);
    assert_ptr_equal(item[2].entry, other.entry);
    /* case: lower match */
    key = 3;
    assert_int_equal(coral_red_black_tree_container_floor(
            &object, &key, &other.entry), 0);
    assert_int_equal(*item[1].value, *other.value);
    assert_ptr_equal(item[1].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_floor_error_on_entry_not_found(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = 1 + i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    const uintmax_t key = 0;
    union entry other;
    assert_int_equal(
            coral_red_black_tree_container_floor(&object, &key, &other.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_higher_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_higher(
                    NULL, (void *) 1, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_higher_error_on_key_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_higher(
                    (void *) 1, NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL);
}

static void check_higher_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_higher(
                    (void *) 1, (void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_higher(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    /* case: higher match */
    uintmax_t key = *item[1].value;
    union entry other;
    assert_int_equal(coral_red_black_tree_container_higher(
            &object, &key, &other.entry), 0);
    assert_int_equal(*item[2].value, *other.value);
    assert_ptr_equal(item[2].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_higher_error_on_entity_not_found(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    uintmax_t key = *item[2].value;
    union entry other;
    assert_int_equal(
            coral_red_black_tree_container_higher(&object, &key, &other.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_lower_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_lower(
                    NULL, (void *) 1, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_lower_error_on_key_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_lower(
                    (void *) 1, NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL);
}

static void check_lower_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_lower(
                    (void *) 1, (void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_lower(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    /* case: lower match */
    uintmax_t key = 3;
    union entry other;
    assert_int_equal(coral_red_black_tree_container_lower(
            &object, &key, &other.entry), 0);
    assert_int_equal(*item[1].value, *other.value);
    assert_ptr_equal(item[1].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_lower_error_on_entity_not_found(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    /* case: lower match */
    uintmax_t key = 0;
    union entry other;
    assert_int_equal(
            coral_red_black_tree_container_lower(&object, &key, &other.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_first_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_first(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_first_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_first((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_first(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    union entry other;
    assert_int_equal(coral_red_black_tree_container_first(
            &object, &other.entry), 0);
    assert_int_equal(*item[0].value, *other.value);
    assert_ptr_equal(item[0].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_first_error_on_container_is_empty(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item;
    assert_int_equal(
            coral_red_black_tree_container_first(&object, &item.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_last_error_on_object_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_last(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL);
}

static void check_last_error_on_out_is_null(void **state) {
    assert_int_equal(
            coral_red_black_tree_container_last((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_last(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    union entry other;
    assert_int_equal(coral_red_black_tree_container_last(
            &object, &other.entry), 0);
    assert_int_equal(*item[2].value, *other.value);
    assert_ptr_equal(item[2].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_last_error_on_container_is_empty(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item;
    assert_int_equal(
            coral_red_black_tree_container_last(&object, &item.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_next_error_on_entry_is_null(void **state) {
    assert_ptr_equal(
            coral_red_black_tree_container_next(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL);
}

static void check_next_error_on_out_is_null(void **state) {
    assert_ptr_equal(
            coral_red_black_tree_container_next((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_next(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    union entry other;
    assert_int_equal(coral_red_black_tree_container_first(
            &object, &other.entry), 0);
    assert_int_equal(coral_red_black_tree_container_next(
            other.entry, &other.entry), 0);
    assert_int_equal(*item[1].value, *other.value);
    assert_ptr_equal(item[1].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_next_error_on_end_of_sequence(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 1;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    union entry other;
    assert_int_equal(coral_red_black_tree_container_first(
            &object, &other.entry), 0);
    assert_int_equal(
            coral_red_black_tree_container_next(other.entry, &other.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_prev_error_on_entry_is_null(void **state) {
    assert_ptr_equal(
            coral_red_black_tree_container_prev(NULL, (void *) 1),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL);
}

static void check_prev_error_on_out_is_null(void **state) {
    assert_ptr_equal(
            coral_red_black_tree_container_prev((void *) 1, NULL),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL);
}

static void check_prev(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 3;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i * i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    union entry other;
    assert_int_equal(coral_red_black_tree_container_last(
            &object, &other.entry), 0);
    assert_int_equal(coral_red_black_tree_container_prev(
            other.entry, &other.entry), 0);
    assert_int_equal(*item[1].value, *other.value);
    assert_ptr_equal(item[1].entry, other.entry);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

static void check_prev_error_on_end_of_sequence(void **state) {
    struct coral_red_black_tree_container object;
    assert_int_equal(coral_red_black_tree_container_init(&object, compare), 0);
    const uintmax_t count = 1;
    union entry {
        struct coral_red_black_tree_container_entry *entry;
        uintmax_t *value;
    } item[count];
    for (uintmax_t i = 0; i < count; i++) {
        assert_int_equal(coral_red_black_tree_container_alloc(
                sizeof(uintmax_t), &item[i].entry), 0);
        *item[i].value = i;
        assert_int_equal(coral_red_black_tree_container_add(
                &object, item[i].entry), 0);
    }
    union entry other;
    assert_int_equal(coral_red_black_tree_container_last(
            &object, &other.entry), 0);
    assert_int_equal(
            coral_red_black_tree_container_prev(other.entry, &other.entry),
            CORAL_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE);
    assert_int_equal(coral_red_black_tree_container_invalidate(
            &object, NULL), 0);
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(check_free_error_on_entry_is_null),
            cmocka_unit_test(check_free),
            cmocka_unit_test(check_alloc_error_on_size_is_zero),
            cmocka_unit_test(check_alloc_error_on_out_is_null),
            cmocka_unit_test(check_alloc),
            cmocka_unit_test(check_alloc_error_on_size_is_too_large),
            cmocka_unit_test(check_alloc_error_on_memory_allocation_failed),
            cmocka_unit_test(check_invalidate_error_on_object_is_null),
            cmocka_unit_test(check_invalidate),
            cmocka_unit_test(check_init_error_on_object_is_null),
            cmocka_unit_test(check_init_error_on_compare_is_null),
            cmocka_unit_test(check_init),
            cmocka_unit_test(check_count_error_on_object_is_null),
            cmocka_unit_test(check_count_error_on_out_is_null),
            cmocka_unit_test(check_count),
            cmocka_unit_test(check_add_error_on_object_is_null),
            cmocka_unit_test(check_add_error_on_entry_is_null),
            cmocka_unit_test(check_add),
            cmocka_unit_test(check_add_error_on_entry_already_exits),
            cmocka_unit_test(check_remove_error_on_object_is_null),
            cmocka_unit_test(check_remove_error_on_entry_is_null),
            cmocka_unit_test(check_remove),
            cmocka_unit_test(check_get_error_on_object_is_null),
            cmocka_unit_test(check_get_error_on_key_is_null),
            cmocka_unit_test(check_get_error_on_out_is_null),
            cmocka_unit_test(check_get),
            cmocka_unit_test(check_get_error_on_entry_not_found),
            cmocka_unit_test(check_ceiling_error_on_object_is_null),
            cmocka_unit_test(check_ceiling_error_on_key_is_null),
            cmocka_unit_test(check_ceiling_error_on_out_is_null),
            cmocka_unit_test(check_ceiling),
            cmocka_unit_test(check_ceiling_error_on_entry_not_found),
            cmocka_unit_test(check_floor_error_on_object_is_null),
            cmocka_unit_test(check_floor_error_on_key_is_null),
            cmocka_unit_test(check_floor_error_on_out_is_null),
            cmocka_unit_test(check_floor),
            cmocka_unit_test(check_floor_error_on_entry_not_found),
            cmocka_unit_test(check_higher_error_on_object_is_null),
            cmocka_unit_test(check_higher_error_on_key_is_null),
            cmocka_unit_test(check_higher_error_on_out_is_null),
            cmocka_unit_test(check_higher),
            cmocka_unit_test(check_higher_error_on_entity_not_found),
            cmocka_unit_test(check_lower_error_on_object_is_null),
            cmocka_unit_test(check_lower_error_on_key_is_null),
            cmocka_unit_test(check_lower_error_on_out_is_null),
            cmocka_unit_test(check_lower),
            cmocka_unit_test(check_lower_error_on_entity_not_found),
            cmocka_unit_test(check_first_error_on_object_is_null),
            cmocka_unit_test(check_first_error_on_out_is_null),
            cmocka_unit_test(check_first),
            cmocka_unit_test(check_first_error_on_container_is_empty),
            cmocka_unit_test(check_last_error_on_object_is_null),
            cmocka_unit_test(check_last_error_on_out_is_null),
            cmocka_unit_test(check_last),
            cmocka_unit_test(check_last_error_on_container_is_empty),
            cmocka_unit_test(check_next_error_on_entry_is_null),
            cmocka_unit_test(check_next_error_on_out_is_null),
            cmocka_unit_test(check_next),
            cmocka_unit_test(check_next_error_on_end_of_sequence),
            cmocka_unit_test(check_prev_error_on_entry_is_null),
            cmocka_unit_test(check_prev_error_on_out_is_null),
            cmocka_unit_test(check_prev),
            cmocka_unit_test(check_prev_error_on_end_of_sequence),
    };
    //cmocka_set_message_output(CM_OUTPUT_XML);
    return cmocka_run_group_tests(tests, NULL, NULL);
}
