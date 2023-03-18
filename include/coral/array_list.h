#ifndef _CORAL_ARRAY_LIST_H_
#define _CORAL_ARRAY_LIST_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>
#include <rock.h>

#define CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL \
    ROCK_ARRAY_ERROR_OBJECT_IS_NULL
#define CORAL_ARRAY_LIST_ERROR_SIZE_IS_ZERO \
    ROCK_ARRAY_ERROR_SIZE_IS_ZERO
#define CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED \
    ROCK_ARRAY_ERROR_MEMORY_ALLOCATION_FAILED
#define CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL \
    ROCK_ARRAY_ERROR_OUT_IS_NULL
#define CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS \
    ROCK_ARRAY_ERROR_INDEX_IS_OUT_OF_BOUNDS
#define CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY \
    ROCK_ARRAY_ERROR_ARRAY_IS_EMPTY
#define CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO \
    ROCK_ARRAY_ERROR_COUNT_IS_ZERO
#define CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL \
    ROCK_ARRAY_ERROR_ITEMS_IS_NULL
#define CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL \
    ROCK_ARRAY_ERROR_ITEM_IS_NULL
#define CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS \
    ROCK_ARRAY_ERROR_ITEM_IS_OUT_OF_BOUNDS
#define CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE \
    ROCK_ARRAY_ERROR_END_OF_SEQUENCE

struct coral_array_list {
    struct rock_array array;
};

/**
 * @brief Initialize array list.
 * @param [in] object instance to be initialized.
 * @param [in] size of an item in the array list.
 * @param [in] capacity set the initial capacity.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_SIZE_IS_ZERO if size is zero.
 * @throws CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to initialize the array list instance.
 */
int coral_array_list_init(struct coral_array_list *object,
                          size_t size,
                          uintmax_t capacity);

/**
 * @brief Invalidate the array list.
 * <p>All the items contained within the array list will have the given <i>on
 * destroy</i> callback invoked upon it. The actual <u>array list instance
 * is not deallocated</u> since it may have been embedded in a larger
 * structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the item is to be destroyed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int coral_array_list_invalidate(struct coral_array_list *object,
                                void (*on_destroy)(void *));

/**
 * @brief Retrieve the capacity.
 * @param [in] object array list instance.
 * @param [in] out receive the capacity.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_array_list_capacity(const struct coral_array_list *object,
                              uintmax_t *out);

/**
 * @brief Retrieve the length.
 * @param [in] object array list instance.
 * @param [in] out receive the number of items.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_array_list_get_length(const struct coral_array_list *object,
                                uintmax_t *out);

/**
 * @brief Set the length.
 * @param [in] object array list instance.
 * @param [in] length number of items in the array list.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to set the array list length.
 */
int coral_array_list_set_length(struct coral_array_list *object,
                                uintmax_t length);

/**
 * @brief Retrieve the size of an item.
 * @param [in] object array list instance.
 * @param [in] out receive the size of an item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_array_list_size(const struct coral_array_list *object,
                          size_t *out);

/**
 * @brief Release excess capacity.
 * @param [in] object array list instance.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int coral_array_list_shrink(struct coral_array_list *object);

/**
 * @brief Add an item at the end.
 * @param [in] object array list instance.
 * @param [in] item to add at the end of the array list.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to add another item.
 */
int coral_array_list_add(struct coral_array_list *object,
                         const void *item);

/**
 * @brief Append all the items.
 * @param [in] object array list instance.
 * @param [in] count number of values in items.
 * @param [in] items all of which we would like to append to the array list.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO if count is zero.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL is items is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to append all the items.
 */
int coral_array_list_add_all(struct coral_array_list *object,
                             uintmax_t count,
                             const void **items);

/**
 * @brief Remove last item.
 * @param [in] object array list instance.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY if the list is empty.
 */
int coral_array_list_remove_last(struct coral_array_list *object);

/**
 * @brief Insert an item at index.
 * @param [in] object array list instance.
 * @param [in] at index where item is to be inserted.
 * @param [in] item to be inserted.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS if at does not refer
 * to an item contained within the array list.
 * @throws CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to add another item.
 */
int coral_array_list_insert(struct coral_array_list *object,
                            uintmax_t at,
                            const void *item);

/**
 * @brief Insert all the items at index.
 * @param [in] object array list instance.
 * @param [in] at index where items are to be inserted.
 * @param [in] count number of values in items.
 * @param [in] items all of which we would like to insert.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS if at does not refer
 * to an item contained within the array list.
 * @throws CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO if count is zero.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEMS_IS_NULL if items is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to add all the items to the array list.
 */
int coral_array_list_insert_all(struct coral_array_list *object,
                                uintmax_t at,
                                uintmax_t count,
                                const void **items);

/**
 * @brief Remove an item at the given index.
 * @param [in] object array list instance.
 * @param [in] at index of item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS if at does not refer
 * to an item contained within the array list.
 */
int coral_array_list_remove(struct coral_array_list *object,
                            uintmax_t at);

/**
 * @brief Remove all the items from the given index up to count items.
 * @param [in] object array list instance.
 * @param [in] at index of item.
 * @param [in] count of items to be removed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_COUNT_IS_ZERO if count is zero.
 * @throws CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS if at does not refer
 * to an item contained within the array list.
 */
int coral_array_list_remove_all(struct coral_array_list *object,
                                uintmax_t at,
                                uintmax_t count);

/**
 * @brief Retrieve the item at the given index.
 * @param [in] object array list instance.
 * @param [in] at index of item.
 * @param [out] out receive the <u>address of</u> the item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS if at does not refer
 * to an item contained within the array list.
 */
int coral_array_list_get(const struct coral_array_list *object,
                         uintmax_t at,
                         void **out);

/**
 * @brief Set the item at the given index.
 * @param [in] object array list instance.
 * @param [in] at index where item should be stored.
 * @param [in] item whose <u>pointed to contents</u> will be copied into the
 * array list unless it is <i>NULL</i> then that index's contents will be
 * zeroed out.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_INDEX_IS_OUT_OF_BOUNDS if at does not refer
 * to an item contained within the array list.
 */
int coral_array_list_set(struct coral_array_list *object,
                         uintmax_t at,
                         const void *item);

/**
 * @brief First item of the array list.
 * @param [in] object array list instance.
 * @param [out] out receive first item in the array list.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY if the array list is empty.
 */
int coral_array_list_first(const struct coral_array_list *object,
                           void **out);

/**
 * @brief Last item of the array list.
 * @param [in] object array list instance.
 * @param [out] out receive last item in the array list.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_LIST_IS_EMPTY if the array list is empty.
 */
int coral_array_list_last(const struct coral_array_list *object,
                          void **out);

/**
 * @brief Retrieve next item.
 * @param [in] object array list instance.
 * @param [in] item current item.
 * @param [out] out receive the next item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS if item is not
 * contained within the array list.
 * @throws CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE if there is no next item.
 */
int coral_array_list_next(const struct coral_array_list *object,
                          const void *item,
                          void **out);

/**
 * @brief Retrieve previous item.
 * @param [in] object array list instance.
 * @param [in] item current item.
 * @param [out] out receive the previous item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS if item is not
 * contained within the array list.
 * @throws CORAL_ARRAY_LIST_ERROR_END_OF_SEQUENCE if there is no previous item.
 */
int coral_array_list_prev(const struct coral_array_list *object,
                          const void *item,
                          void **out);

/**
 * @brief Receive the index of item.
 * @param [in] object array list instance.
 * @param [in] item whose index we would like.
 * @param [out] out receive the index of item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_ARRAY_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_ARRAY_LIST_ERROR_ITEM_IS_OUT_OF_BOUNDS if item is not
 * contained within the array list.
 */
int coral_array_list_at(const struct coral_array_list *object,
                        const void *item,
                        uintmax_t *out);

#endif /* _CORAL_ARRAY_LIST_H_ */
