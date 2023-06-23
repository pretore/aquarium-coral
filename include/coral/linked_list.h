#ifndef _CORAL_LINKED_LIST_H_
#define _CORAL_LINKED_LIST_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>
#include <rock.h>

#define CORAL_LINKED_LIST_ERROR_SIZE_IS_ZERO \
    SEA_URCHIN_ERROR_VALUE_IS_ZERO
#define CORAL_LINKED_LIST_ERROR_SIZE_IS_TOO_LARGE \
    SEA_URCHIN_ERROR_VALUE_IS_TOO_LARGE
#define CORAL_LINKED_LIST_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL
#define CORAL_LINKED_LIST_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL \
    SEA_URCHIN_ERROR_ITEM_IS_NULL
#define CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL \
    SEA_URCHIN_ERROR_OBJECT_IS_NULL
#define CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY \
    SEA_URCHIN_ERROR_IS_EMPTY
#define CORAL_LINKED_LIST_ERROR_END_OF_SEQUENCE \
    SEA_URCHIN_ERROR_END_OF_SEQUENCE
#define CORAL_LINKED_LIST_ERROR_VALUE_IS_NULL \
    SEA_URCHIN_ERROR_VALUE_IS_NULL

struct coral_linked_list_item;

struct coral_linked_list {
    uintmax_t count;
    struct rock_linked_list_node *list;
};

/**
 * @brief Allocate memory for an item.
 * @param [in] size in bytes of item to create.
 * @param [out] out receive allocated item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_SIZE_IS_ZERO if size is zero.
 * @throws CORAL_LINKED_LIST_ERROR_SIZE_IS_TOO_LARGE if size is too large.
 * @throws CORAL_LINKED_LIST_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to allocate an entry.
 */
int coral_linked_list_alloc(size_t size,
                            struct coral_linked_list_item **out);

/**
 * @brief Free memory of an item.
 * @param [in] item to be freed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 */
int coral_linked_list_free(struct coral_linked_list_item *item);

/**
 * @brief Initialize linked list.
 * @param [in] object instance to be initialized.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int coral_linked_list_init(struct coral_linked_list *object);

/**
 * @brief Invalidate linked list.
 * <p>The items in the container are destroyed and each entry will have
 * the provided <i>on destroy</i> callback invoked upon itself. The actual
 * <u>container instance is not deallocated</u> since it may have been embedded
 * in a larger structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the node is to be destroyed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int coral_linked_list_invalidate(
        struct coral_linked_list *object,
        void (*on_destroy)(struct coral_linked_list_item *item));

/**
 * @brief Retrieve the count of items.
 * @param [in] object instance whose count we are to retrieve.
 * @param [out] out receive the item count.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_linked_list_count(struct coral_linked_list *object,
                            uintmax_t *out);

/**
 * @brief Add item to the end.
 * @param [in] object linked list.
 * @param [in] item to add.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 */
int coral_linked_list_add(struct coral_linked_list *object,
                          struct coral_linked_list_item *item);

/**
 * @brief Insert value at item.
 * @param [in] object linked list.
 * @param [in] item at which value will be inserted.
 * @param [in] value to insert.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_VALUE_IS_NULL if value is <i>NULL</i>.
 */
int coral_linked_list_insert(struct coral_linked_list *object,
                             struct coral_linked_list_item *item,
                             struct coral_linked_list_item *value);

/**
 * @brief Remove item.
 * @param [in] object linked list.
 * @param [in] item to be removed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 */
int coral_linked_list_remove(struct coral_linked_list *object,
                             struct coral_linked_list_item *item);

/**
 * @brief Retrieve first item.
 * @param [in] object linked list.
 * @param [out] out receive first item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY if there are no items in the
 * linked list.
 */
int coral_linked_list_first(const struct coral_linked_list *object,
                            struct coral_linked_list_item **out);

/**
 * @brief Retrieve last item.
 * @param [in] object linked list.
 * @param [out] out receive last item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY if there are no items in the
 * linked list.
 */
int coral_linked_list_last(const struct coral_linked_list *object,
                           struct coral_linked_list_item **out);

/**
 * @brief Retrieve the next item.
 * @param [in] object linked list.
 * @param [in] item current item.
 * @param [out] out receive the next item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_linked_list_next(const struct coral_linked_list *object,
                           const struct coral_linked_list_item *item,
                           struct coral_linked_list_item **out);

/**
 * @brief Retrieve the previous item.
 * @param [in] object linked list.
 * @param [in] item current item.
 * @param [out] out receive the previous item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_LINKED_LIST_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_linked_list_prev(const struct coral_linked_list *object,
                           const struct coral_linked_list_item *item,
                           struct coral_linked_list_item **out);

#endif /* _CORAL_LINKED_LIST_H_ */
