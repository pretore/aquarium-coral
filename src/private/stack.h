#ifndef _CORAL_PRIVATE_STACK_H_
#define _CORAL_PRIVATE_STACK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>

struct rock_forward_list_node;

#define CORAL_STACK_ERROR_OBJECT_IS_NULL \
    SEA_URCHIN_ERROR_OBJECT_IS_NULL
#define CORAL_STACK_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL
#define CORAL_STACK_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define CORAL_STACK_ERROR_ITEM_IS_NULL \
    SEA_URCHIN_ERROR_ITEM_IS_NULL
#define CORAL_STACK_ERROR_STACK_IS_EMPTY \
    SEA_URCHIN_ERROR_IS_EMPTY

struct coral_stack {
    struct rock_forward_list_node *top;
};

/**
 * @brief Add item to the top of the stack.
 * @param [in] object stack instance.
 * @param [in] item to add to the top of the stack.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_STACK_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_STACK_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to add item.
 */
int coral_stack_push(struct coral_stack *object, const void *item);

/**
 * @brief Remove item from the top of the stack.
 * @param [in] object stack instance.
 * @param [in] out receive the item in the top of the stack.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_STACK_ERROR_STACK_IS_EMPTY if stack is empty.
 */
int coral_stack_pop(struct coral_stack *object, void **out);

/**
 * @brief Retrieve the item from the top of the stack without removing it.
 * @param [in] object stack instance.
 * @param [in] out receive the item in the top of the stack without removing it.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_STACK_ERROR_STACK_IS_EMPTY if stack is empty.
 */
int coral_stack_peek(struct coral_stack *object, void **out);

#endif /* _CORAL_PRIVATE_STACK_H_ */
