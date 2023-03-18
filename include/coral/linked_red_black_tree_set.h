#ifndef _CORAL_LINKED_RED_BLACK_TREE_SET_H_
#define _CORAL_LINKED_RED_BLACK_TREE_SET_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>
#include <rock.h>

#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL  \
    SEA_URCHIN_ERROR_OBJECT_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_COMPARE_IS_NULL \
    SEA_URCHIN_ERROR_FUNCTION_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SIZE_IS_ZERO \
    SEA_URCHIN_ERROR_VALUE_IS_ZERO
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SIZE_IS_TOO_LARGE \
    SEA_URCHIN_ERROR_VALUE_IS_TOO_LARGE
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL \
    SEA_URCHIN_ERROR_VALUE_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS \
    SEA_URCHIN_ERROR_VALUE_ALREADY_EXISTS
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND \
    SEA_URCHIN_ERROR_VALUE_NOT_FOUND
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL \
    SEA_URCHIN_ERROR_ITEM_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND \
    SEA_URCHIN_ERROR_ITEM_NOT_FOUND
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY \
    SEA_URCHIN_ERROR_IS_EMPTY
#define CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE \
    SEA_URCHIN_ERROR_END_OF_SEQUENCE

struct coral_linked_red_black_tree_set {
    struct rock_red_black_tree tree;
    size_t size;
    size_t alloc;
    struct rock_linked_list_node *list;

    int (*compare)(const void *, const void *);
};

/**
 * @brief Initialize linked red black tree set.
 * @param [in] object instance to be initialized.
 * @param [in] size of values to be contained within the set.
 * @param [in] compare comparison which must return an integer less than,
 * equal to, or greater than zero if the <u>first value</u> is considered
 * to be respectively less than, equal to, or greater than the <u>second
 * value</u>.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_COMPARE_IS_NULL if compare is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SIZE_IS_ZERO if size is zero.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SIZE_IS_TOO_LARGE if size is
 * too large.
 */
int coral_linked_red_black_tree_set_init(
        struct coral_linked_red_black_tree_set *object,
        size_t size,
        int (*compare)(const void *first,
                       const void *second));

/**
 * @brief Invalidate linked red black tree set.
 * <p>All the items contained within the tree set will have the given <i>on
 * destroy</i> callback invoked upon itself. The actual <u>tree set instance
 * is not deallocated</u> since it may have been embedded in a larger
 * structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the item is to be destroyed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 */
int coral_linked_red_black_tree_set_invalidate(
        struct coral_linked_red_black_tree_set *object,
        void (*on_destroy)(void *item));

/**
 * @brief Retrieve the size of an item.
 * @param [in] object tree set instance.
 * @param [in] out receive the size of an item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 */
int coral_linked_red_black_tree_set_size(
        const struct coral_linked_red_black_tree_set *object,
        size_t *out);

/**
 * @brief Retrieve the count of items.
 * @param [in] object instance whose count we are to retrieve.
 * @param [out] out receive the count.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 */
int coral_linked_red_black_tree_set_count(
        const struct coral_linked_red_black_tree_set *object,
        uintmax_t *out);

/**
 * @brief Add value to the tree set.
 * @param [in] object tree set instance.
 * @param [in] value to be added.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS if value
 * is already present in the tree set.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there was not enough memory to add value to the tree set.
 */
int coral_linked_red_black_tree_set_add(
        struct coral_linked_red_black_tree_set *object,
        const void *value);

/**
 * @brief Remove value from the tree set.
 * @param [in] object tree set instance.
 * @param [in] value to be removed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_NOT_FOUND if value is
 * not in the tree set instance.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is not enough memory to find item for removal.
 */
int coral_linked_red_black_tree_set_remove(
        struct coral_linked_red_black_tree_set *object,
        const void *value);

/**
 * @brief Check if tree set contains the given value.
 * @param [in] object tree set instance.
 * @param [in] value to check if value is present.
 * @param [out] out receive true if value is present, otherwise false.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is not enough memory to find item.
 */
int coral_linked_red_black_tree_set_contains(
        const struct coral_linked_red_black_tree_set *object,
        const void *value,
        bool *out);

/**
 * @brief Retrieve item for value.
 * @param [in] object tree set instance.
 * @param [in] value to find.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND if there is no
 * item that matched value.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is not enough memory to find item.
 */
int coral_linked_red_black_tree_set_get(
        const struct coral_linked_red_black_tree_set *object,
        const void *value,
        const void **out);

/**
 * @brief Retrieve item for value or the next higher value.
 * @param [in] object tree set instance.
 * @param [in] value to find or its next higher.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND if no item
 * matched value or a higher value.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is not enough memory to find item.
 */
int coral_linked_red_black_tree_set_ceiling(
        const struct coral_linked_red_black_tree_set *object,
        const void *value,
        const void **out);

/**
 * @brief Retrieve item for value or the next lower value.
 * @param [in] object tree set instance.
 * @param [in] value to find or its next lower.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND if no item
 * matched value or a lower value.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is not enough memory to find item.
 */
int coral_linked_red_black_tree_set_floor(
        const struct coral_linked_red_black_tree_set *object,
        const void *value,
        const void **out);

/**
 * @brief Retrieve item for next higher value.
 * @param [in] object tree set instance.
 * @param [in] value whose next higher value we are trying to find.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND if there is no
 * greater value.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is not enough memory to find item.
 */
int coral_linked_red_black_tree_set_higher(
        const struct coral_linked_red_black_tree_set *object,
        const void *value,
        const void **out);

/**
 * @brief Retrieve item for the next lower value.
 * @param [in] object tree set instance.
 * @param [in] value whose next lower value we are trying to find.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_NOT_FOUND if there is no
 * lower value.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there is not enough memory to find item.
 */
int coral_linked_red_black_tree_set_lower(
        const struct coral_linked_red_black_tree_set *object,
        const void *value,
        const void **out);

/**
 * @brief Retrieve item with the lowest value.
 * @param [in] object tree set instance.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY if tree set is
 * empty.
 */
int coral_linked_red_black_tree_set_lowest(
        const struct coral_linked_red_black_tree_set *object,
        const void **out);

/**
 * @brief Retrieve item with the highest value.
 * @param [in] object tree set instance.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY if tree set is
 * empty.
 */
int coral_linked_red_black_tree_set_highest(
        const struct coral_linked_red_black_tree_set *object,
        const void **out);

/**
 * @brief First item of the tree set.
 * @param [in] object tree set instance.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY if tree set is
 * empty.
 */
int coral_linked_red_black_tree_set_first(
        const struct coral_linked_red_black_tree_set *object,
        const void **out);

/**
 * @brief Last item of the tree set.
 * @param [in] object tree set instance.
 * @param [out] out receive the <u>address of</u> item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_SET_IS_EMPTY if tree set is
 * empty.
 */
int coral_linked_red_black_tree_set_last(
        const struct coral_linked_red_black_tree_set *object,
        const void **out);

/**
 * @brief Remove item.
 * @param [in] object tree set instance.
 * @param [out] item <u>address of</u> item to be removed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL if item is
 * <i>NULL</i>.
 */
int coral_linked_red_black_tree_set_remove_item(
        struct coral_linked_red_black_tree_set *object,
        const void *item);

/**
 * @brief Retrieve next item.
 * @param [in] object tree set instance.
 * @param [in] item current <u>address of</u> item.
 * @param [out] out receive the <u>address of</u> the next item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if item is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL if item is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE if there is no
 * next item.
 */
int coral_linked_red_black_tree_set_next(
        const struct coral_linked_red_black_tree_set *object,
        const void *item,
        const void **out);

/**
 * @brief Retrieve previous item.
 * @param [in] object tree set instance.
 * @param [in] item current <u>address of</u> item.
 * @param [out] out receive the <u>address of</u> the previous item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if item is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL if item is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_END_OF_SEQUENCE if there is no
 * previous item.
 */
int coral_linked_red_black_tree_set_prev(
        const struct coral_linked_red_black_tree_set *object,
        const void *item,
        const void **out);

/**
 * @brief Insert value after item.
 * @param [in] object tree set instance.
 * @param [in] item current <u>address of</u> item after which <b>value</b>
 * will be inserted.
 * @param [in] value to be inserted.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL if item is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS if value
 * is already present in the tree set.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there was not enough memory to add value to the tree set.
 */
int coral_linked_red_black_tree_set_insert_after(
        struct coral_linked_red_black_tree_set *object,
        const void *item,
        const void *value);

/**
 * @brief Insert value before item.
 * @param [in] object tree set instance.
 * @param [in] item current <u>address of</u> item before which <b>value</b>
 * will be inserted.
 * @param [in] value to be inserted.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_ITEM_IS_NULL if item is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS if value
 * is already present in the tree set.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there was not enough memory to add value to the tree set.
 */
int coral_linked_red_black_tree_set_insert_before(
        struct coral_linked_red_black_tree_set *object,
        const void *item,
        const void *value);

/**
 * @brief Add value to the end.
 * @param [in] object tree set instance.
 * @param [in] value which will be added after all values.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS if value
 * is already present in the tree set.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there was not enough memory to add value to the tree set.
 */
int coral_linked_red_black_tree_set_append(
        struct coral_linked_red_black_tree_set *object,
        const void *value);

/**
 * @brief Add value to the beginning.
 * @param [in] object tree set instance.
 * @param [in] value which will be added before all other values.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_IS_NULL if value is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_VALUE_ALREADY_EXISTS if value
 * is already present in the tree set.
 * @throws CORAL_LINKED_RED_BLACK_TREE_SET_ERROR_MEMORY_ALLOCATION_FAILED if
 * there was not enough memory to add value to the tree set.
 */
int coral_linked_red_black_tree_set_prepend(
        struct coral_linked_red_black_tree_set *object,
        const void *value);

#endif /* _CORAL_LINKED_RED_BLACK_TREE_SET_H_ */
