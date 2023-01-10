#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <seagrass.h>
#include <rock.h>
#include <coral.h>

#ifdef TEST
#include <test/cmocka.h>
#endif

struct node {
    struct rock_forward_list_node node;
    unsigned char data[];
};

bool coral_linked_stack_init(struct coral_linked_stack *const object,
                             const size_t size) {
    if (!object) {
        coral_error = CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!size) {
        coral_error = CORAL_LINKED_STACK_ERROR_SIZE_IS_ZERO;
        return false;
    }
    uintmax_t alloc;
    bool result = seagrass_uintmax_t_add(
            size, sizeof(struct node), &alloc);
    if (!result || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == seagrass_error || result);
        coral_error = CORAL_LINKED_STACK_ERROR_SIZE_IS_TOO_LARGE;
        return false;
    }
    *object = (struct coral_linked_stack) {0};
    object->size = size;
    object->alloc = alloc;
    return true;
}

bool coral_linked_stack_invalidate(struct coral_linked_stack *const object,
                                   void (*const on_destroy)(void *)) {
    if (!object) {
        coral_error = CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    struct rock_forward_list_node *node = object->top;
    if (node) {
        struct node *prev = NULL;
        do {
            free(prev);
            struct node *item = rock_container_of(node, struct node, node);
            if (on_destroy) {
                on_destroy(&item->data);
            }
            prev = item;
        } while (rock_forward_list_next(node, &node));
        free(prev);
        seagrass_required_true(ROCK_FORWARD_LIST_ERROR_END_OF_SEQUENCE
                               == rock_error);
    }
    *object = (struct coral_linked_stack) {0};
    return true;
}

bool coral_linked_stack_size(const struct coral_linked_stack *const object,
                             size_t *const out) {
    if (!object) {
        coral_error = CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_STACK_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->size;
    return true;
}

bool coral_linked_stack_count(const struct coral_linked_stack *const object,
                             uintmax_t *const out) {
    if (!object) {
        coral_error = CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_STACK_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->count;
    return true;
}

bool coral_linked_stack_push(struct coral_linked_stack *const object,
                             const void *const item) {
    if (!object) {
        coral_error = CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_LINKED_STACK_ERROR_ITEM_IS_NULL;
        return false;
    }
    struct node *node;
    {
        const int error = posix_memalign((void **) &node, sizeof(void *),
                                         object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            coral_error = CORAL_LINKED_STACK_ERROR_MEMORY_ALLOCATION_FAILED;
            return false;
        }
    }
    struct rock_forward_list_node *const fl_node = &node->node;
    seagrass_required_true(rock_forward_list_node_init(fl_node));
    memcpy(&node->data, item, object->size);
    if (object->top) {
        seagrass_required_true(rock_forward_list_insert(
                fl_node, object->top));
    }
    object->top = fl_node;
    seagrass_required_true(seagrass_uintmax_t_add(
            1, object->count, &object->count));
    return true;
}

bool coral_linked_stack_pop(struct coral_linked_stack *const object,
                            void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_STACK_ERROR_OUT_IS_NULL;
        return false;
    }
    if (!object->top) {
        coral_error = CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY;
        return false;
    }
    struct node *item = rock_container_of(object->top, struct node, node);
    if (!rock_forward_list_next(object->top, &object->top)) {
        seagrass_required_true(ROCK_FORWARD_LIST_ERROR_END_OF_SEQUENCE
                               == rock_error);
        object->top = NULL;
    }
    seagrass_required_true(seagrass_uintmax_t_subtract(
            object->count, 1, &object->count));
    memcpy(out, &item->data, object->size);
    free(item);
    return true;
}

bool coral_linked_stack_peek(const struct coral_linked_stack *const object,
                             void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_STACK_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_STACK_ERROR_OUT_IS_NULL;
        return false;
    }
    if (!object->top) {
        coral_error = CORAL_LINKED_STACK_ERROR_STACK_IS_EMPTY;
        return false;
    }
    struct node *item = rock_container_of(object->top, struct node, node);
    memcpy(out, &item->data, object->size);
    return true;
}
