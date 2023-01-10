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

bool coral_linked_queue_init(struct coral_linked_queue *const object,
                             const size_t size) {
    if (!object) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!size) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_SIZE_IS_ZERO;
        return false;
    }
    uintmax_t alloc;
    bool result = seagrass_uintmax_t_add(
            size, sizeof(struct node), &alloc);
    if (!result || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == seagrass_error || result);
        coral_error = CORAL_LINKED_QUEUE_ERROR_SIZE_IS_TOO_LARGE;
        return false;
    }
    *object = (struct coral_linked_queue) {0};
    object->size = size;
    object->alloc = alloc;
    return true;
}

bool coral_linked_queue_invalidate(struct coral_linked_queue *const object,
                                   void (*const on_destroy)(void *)) {
    if (!object) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL;
        return false;
    }
    struct rock_forward_list_node *node = object->head;
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
    *object = (struct coral_linked_queue) {0};
    return true;
}

bool coral_linked_queue_size(const struct coral_linked_queue *const object,
                             size_t *const out) {
    if (!object) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->size;
    return true;
}

bool coral_linked_queue_count(const struct coral_linked_queue *const object,
                              uintmax_t *const out) {
    if (!object) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL;
        return false;
    }
    *out = object->count;
    return true;
}

bool coral_linked_queue_add(struct coral_linked_queue *const object,
                            const void *const item) {
    if (!object) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!item) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_ITEM_IS_NULL;
        return false;
    }
    struct node *node;
    {
        const int error = posix_memalign((void **) &node, sizeof(void *),
                                         object->alloc);
        if (error) {
            seagrass_required_true(ENOMEM == error);
            coral_error = CORAL_LINKED_QUEUE_ERROR_MEMORY_ALLOCATION_FAILED;
            return false;
        }
    }
    struct rock_forward_list_node *const fl_node = &node->node;
    seagrass_required_true(rock_forward_list_node_init(fl_node));
    memcpy(&node->data, item, object->size);
    if (!object->tail) {
        object->head = fl_node;
    } else {
        seagrass_required_true(rock_forward_list_insert(
                object->tail, fl_node));
    }
    object->tail = fl_node;
    seagrass_required_true(seagrass_uintmax_t_add(
            1, object->count, &object->count));
    return true;
}

bool coral_linked_queue_remove(struct coral_linked_queue *const object,
                               void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL;
        return false;
    }
    if (!object->head) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY;
        return false;
    }
    struct node *item = rock_container_of(object->head, struct node, node);
    if (!rock_forward_list_next(object->head, &object->head)) {
        seagrass_required_true(ROCK_FORWARD_LIST_ERROR_END_OF_SEQUENCE
                               == rock_error);
        object->head = NULL;
        object->tail = NULL;
    }
    seagrass_required_true(seagrass_uintmax_t_subtract(
            object->count, 1, &object->count));
    memcpy(out, &item->data, object->size);
    free(item);
    return true;
}

bool coral_linked_queue_peek(const struct coral_linked_queue *const object,
                             void **const out) {
    if (!object) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OBJECT_IS_NULL;
        return false;
    }
    if (!out) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_OUT_IS_NULL;
        return false;
    }
    if (!object->head) {
        coral_error = CORAL_LINKED_QUEUE_ERROR_QUEUE_IS_EMPTY;
        return false;
    }
    struct node *item = rock_container_of(object->head, struct node, node);
    memcpy(out, &item->data, object->size);
    return true;
}
