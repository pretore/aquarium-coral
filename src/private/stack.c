#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <seagrass.h>
#include <rock.h>

#include "stack.h"

#ifdef TEST
#include <test/cmocka.h>
#endif

struct node {
    struct rock_forward_list_node node;
    unsigned char data[];
};

int coral_stack_push(struct coral_stack *object, const void *item) {
    if (!object) {
        return CORAL_STACK_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_STACK_ERROR_ITEM_IS_NULL;
    }
    struct node *node;
    {
        const int error = posix_memalign((void **) &node, sizeof(void *),
                                         sizeof(void *) + sizeof(*node));
        if (error) {
            seagrass_required_true(ENOMEM == error);
            return CORAL_STACK_ERROR_MEMORY_ALLOCATION_FAILED;
        }
    }
    struct rock_forward_list_node *const fl_node = &node->node;
    seagrass_required_true(!rock_forward_list_node_init(fl_node));
    memcpy(&node->data, item, sizeof(void *));
    if (object->top) {
        seagrass_required_true(!rock_forward_list_insert(
                fl_node, object->top));
    }
    object->top = fl_node;
    return 0;
}

int coral_stack_pop(struct coral_stack *object, void **out) {
    if (!object) {
        return CORAL_STACK_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_STACK_ERROR_OUT_IS_NULL;
    }
    if (!object->top) {
        return CORAL_STACK_ERROR_STACK_IS_EMPTY;
    }
    int error;
    struct node *item = rock_container_of(object->top, struct node, node);
    if ((error = rock_forward_list_next(object->top, &object->top))) {
        seagrass_required_true(ROCK_FORWARD_LIST_ERROR_END_OF_SEQUENCE
                               == error);
        object->top = NULL;
    }
    memcpy(out, &item->data, sizeof(void *));
    free(item);
    return 0;
}

int coral_stack_peek(struct coral_stack *object, void **out) {
    if (!object) {
        return CORAL_STACK_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_STACK_ERROR_OUT_IS_NULL;
    }
    if (!object->top) {
        return CORAL_STACK_ERROR_STACK_IS_EMPTY;
    }
    struct node *item = rock_container_of(object->top, struct node, node);
    memcpy(out, &item->data, sizeof(void *));
    return 0;
}
