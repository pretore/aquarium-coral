#ifndef _CORAL_FORWARD_STACK_H_
#define _CORAL_FORWARD_STACK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>

struct rock_forward_list_node;

#define CORAL_FORWARD_STACK_ERROR_OBJECT_IS_NULL \
    SEA_URCHIN_ERROR_OBJECT_IS_NULL
#define CORAL_FORWARD_STACK_ERROR_SIZE_IS_ZERO \
    SEA_URCHIN_ERROR_VALUE_IS_ZERO
#define CORAL_FORWARD_STACK_ERROR_SIZE_IS_TOO_LARGE \
    SEA_URCHIN_ERROR_VALUE_IS_TOO_LARGE
#define CORAL_FORWARD_STACK_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL
#define CORAL_FORWARD_STACK_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define CORAL_FORWARD_STACK_ERROR_ITEM_IS_NULL \
    SEA_URCHIN_ERROR_ITEM_IS_NULL
#define CORAL_FORWARD_STACK_ERROR_STACK_IS_EMPTY \
    SEA_URCHIN_ERROR_IS_EMPTY

struct coral_forward_stack_item;

struct coral_forward_stack {
    struct rock_forward_list_node *top;
    uintmax_t count;
};

/**
 * @brief Allocate memory for an item.
 * @param [in] size in bytes of item to create.
 * @param [out] out receive allocated entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_FORWARD_STACK_ERROR_SIZE_IS_ZERO if size if zero.
 * @throws CORAL_FORWARD_STACK_ERROR_SIZE_IS_TOO_LARGE if size is too large.
 * @throws CORAL_FORWARD_STACK_ERROR_MEMORY_ALLOCATION_FAILED if there is not
 * enough memory to allocate an item.
 */
int coral_forward_stack_alloc(size_t size,
                              struct coral_forward_stack_item **out);

/**
 * @brief Free memory of an item.
 * @param [in] item to be freed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 */
int coral_forward_stack_free(struct coral_forward_stack_item *item);

/**
 * @brief Initialise forward stack.
 * @param [in] object instance to be initialised.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int coral_forward_stack_init(struct coral_forward_stack *object);

/**
 * @brief Invalidate forward stack.
 * <p>All the items contained within the stack will have the given <i>on
 * destroy</i> callback invoked upon itself. The actual <u>stack instance
 * is not deallocated</u> since it may have been embedded in a larger
 * structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the item is to be destroyed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
int coral_forward_stack_invalidate(
        struct coral_forward_stack *object,
        void (*on_destroy)(struct coral_forward_stack_item *));

/**
 * @brief Retrieve the count of items.
 * @param [in] object instance whose count we are to retrieve.
 * @param [out] out receive the count.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_FORWARD_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
int coral_forward_stack_count(const struct coral_forward_stack *object,
                              uintmax_t *out);

/**
 * @brief Add item to the top of the stack.
 * @param [in] object stack instance.
 * @param [in] item to add to the top of the stack.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_FORWARD_STACK_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 */
int coral_forward_stack_push(struct coral_forward_stack *object,
                             struct coral_forward_stack_item *item);

/**
 * @brief Remove item from the top of the stack.
 * @param [in] object stack instance.
 * @param [in] out receive the item in the top of the stack.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_FORWARD_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_FORWARD_STACK_ERROR_STACK_IS_EMPTY if stack is empty.
 */
int coral_forward_stack_pop(struct coral_forward_stack *object,
                            struct coral_forward_stack_item **out);

/**
 * @brief Retrieve the item from the top of the stack without removing it.
 * @param [in] object stack instance.
 * @param [in] out receive the item in the top of the stack without removing it.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_FORWARD_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_FORWARD_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_FORWARD_STACK_ERROR_STACK_IS_EMPTY if stack is empty.
 */
int coral_forward_stack_peek(const struct coral_forward_stack *object,
                             struct coral_forward_stack_item **out);

#endif /* _CORAL_FORWARD_STACK_H_ */
