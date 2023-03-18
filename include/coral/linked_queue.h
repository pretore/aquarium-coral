#ifndef _CORAL_LINKED_QUEUE_H_
#define _CORAL_LINKED_QUEUE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>

struct rock_forward_list_node;

#define CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL \
    SEA_URCHIN_ERROR_OBJECT_IS_NULL
#define CORAL_LINKED_QUEUE_ERROR_SIZE_IS_ZERO \
    SEA_URCHIN_ERROR_VALUE_IS_ZERO
#define CORAL_LINKED_QUEUE_ERROR_SIZE_IS_TOO_LARGE \
    SEA_URCHIN_ERROR_VALUE_IS_TOO_LARGE
#define CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL
#define CORAL_LINKED_QUEUE_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define CORAL_LINKED_QUEUE_ERROR_ITEM_IS_NULL \
    SEA_URCHIN_ERROR_ITEM_IS_NULL
#define CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY \
    SEA_URCHIN_ERROR_IS_EMPTY

struct coral_linked_queue {
    struct rock_forward_list_node *head;
    struct rock_forward_list_node *tail;
    size_t size;
    size_t alloc;
    uintmax_t count;
};

/**
 * @brief Initialize linked queue.
 * @param [in] object instance to be initialized.
 * @param [in] size of item to be contained within the queue.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_SIZE_IS_ZERO if size is zero.
 * @throws CORAL_LINKED_QUEUE_ERROR_SIZE_IS_TOO_LARGE if size is too large.
 */
int coral_linked_queue_init(struct coral_linked_queue *object, size_t size);

/**
 * @brief Invalidate linked queue.
 * <p>All the items contained within the queue will have the given <i>on
 * destroy</i> callback invoked upon itself. The actual <u>queue instance
 * is not deallocated</u> since it may have been embedded in a larger
 * structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the item is to be destroyed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int coral_linked_queue_invalidate(struct coral_linked_queue *object,
                                  void (*on_destroy)(void *item));

/**
 * @brief Retrieve the size of an item.
 * @param [in] object queue instance.
 * @param [out] out receive the size of an item.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_linked_queue_size(const struct coral_linked_queue *object,
                            size_t *out);

/**
 * @brief Retrieve the count of items.
 * @param [in] object instance whose count we are to retrieve.
 * @param [out] out receive the count.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_linked_queue_count(const struct coral_linked_queue *object,
                             uintmax_t *out);

/**
 * @brief Add item to the end of the queue.
 * @param [in] object queue instance.
 * @param [in] item to add to the end of the queue.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to add item.
 */
int coral_linked_queue_add(struct coral_linked_queue *object,
                           const void *item);

/**
 * @brief Remove item from the front of the queue.
 * @param [in] object queue instance.
 * @param [in] out receive the item in the front of the queue.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY if queue is empty.
 */
int coral_linked_queue_remove(struct coral_linked_queue *object,
                              void **out);

/**
 * @brief Retrieve the item from the front of the queue without removing it.
 * @param [in] object queue instance.
 * @param [in] out receive the item in the front of the queue without
 * removing it.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY if queue is empty.
 */
int coral_linked_queue_peek(const struct coral_linked_queue *object,
                            void **out);

#endif /* _CORAL_LINKED_QUEUE_H_ */
