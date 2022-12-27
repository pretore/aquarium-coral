#ifndef _CORAL_LINKED_STACK_H_
#define _CORAL_LINKED_STACK_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct rock_forward_list_node;

#define CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL                 1
#define CORAL_LINKED_STACK_ERROR_SIZE_IS_ZERO                   2
#define CORAL_LINKED_STACK_ERROR_SIZE_IS_TOO_LARGE              3
#define CORAL_LINKED_STACK_ERROR_OUT_IS_NULL                    4
#define CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED       5
#define CORAL_LINKED_STACK_ERROR_ITEM_IS_NULL                   6
#define CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY                 7

struct coral_linked_stack {
    struct rock_forward_list_node *top;
    size_t size;
    size_t alloc;
    uintmax_t count;
};

/**
 * @brief Initialize linked stack.
 * @param [in] object instance to be initialized.
 * @param [in] size of item to be contained within the stack.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_SIZE_IS_ZERO if size is zero.
 * @throws CORAL_LINKED_STACK_ERROR_SIZE_IS_TOO_LARGE if size is too large.
 */
bool coral_linked_stack_init(struct coral_linked_stack *object,
                             size_t size);

/**
 * @brief Invalidate linked stack.
 * <p>All the items contained within the stack will have the given <i>on
 * destroy</i> callback invoked upon itself. The actual <u>stack instance
 * is not deallocated</u> since it may have been embedded in a larger
 * structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the item is to be destroyed.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
bool coral_linked_stack_invalidate(struct coral_linked_stack *object,
                                   void (*on_destroy)(void *item));

/**
 * @brief Retrieve the size of an item.
 * @param [in] object stack instance.
 * @param [out] out receive the size of an item.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool coral_linked_stack_size(const struct coral_linked_stack *object,
                             size_t *out);

/**
 * @brief Retrieve the count of items.
 * @param [in] object instance whose count we are to retrieve.
 * @param [out] out receive the count.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool coral_linked_stack_count(const struct coral_linked_stack *object,
                              uintmax_t *out);

/**
 * @brief Add item to the top of the stack.
 * @param [in] object stack instance.
 * @param [in] item to add to the top of the stack.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_ITEM_IS_NULL if item is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * insufficient memory to add item.
 */
bool coral_linked_stack_push(struct coral_linked_stack *object,
                             const void *item);

/**
 * @brief Remove item from the top of the stack.
 * @param [in] object stack instance.
 * @param [in] out receive the item in the top of the stack.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY if stack is empty.
 */
bool coral_linked_stack_pop(struct coral_linked_stack *object,
                            void **out);

/**
 * @brief Retrieve the item from the top of the stack without removing it.
 * @param [in] object stack instance.
 * @param [in] out receive the item in the top of the stack without
 * removing it.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY if stack is empty.
 */
bool coral_linked_stack_peek(const struct coral_linked_stack *object,
                             void **out);

#endif /* _CORAL_LINKED_STACK_H_ */
