#include <stdlib.h>
#include <errno.h>
#include <seagrass.h>
#include <rock.h>
#include <coral.h>

#ifdef TEST
#include <test/cmocka.h>
#endif

struct coral_forward_queue_item {
    void *data;
};

struct node {
    struct rock_forward_list_node node;
    unsigned char data[];
};

int coral_forward_queue_alloc(const size_t size,
                              struct coral_forward_queue_item **const out) {
    if (!size) {
        return CORAL_FORWARD_QUEUE_ERROR_SIZE_IS_ZERO;
    }
    if (!out) {
        return CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL;
    }
    int error;
    uintmax_t alloc;
    if ((error = seagrass_uintmax_t_add(size, sizeof(struct node), &alloc))
        || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_FORWARD_QUEUE_ERROR_SIZE_IS_TOO_LARGE;
    }
    struct node *item;
    if ((error = posix_memalign((void **) &item, sizeof(void *), alloc))) {
        seagrass_required_true(ENOMEM == error);
        return CORAL_FORWARD_QUEUE_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    seagrass_required_true(!rock_forward_list_node_init(&item->node));
    *out = (struct coral_forward_queue_item *) &item->data;
    return 0;
}

int coral_forward_queue_free(struct coral_forward_queue_item *const item) {
    if (!item) {
        return CORAL_FORWARD_QUEUE_ERROR_ITEM_IS_NULL;
    }
    struct node *const A = rock_container_of(item, struct node, data);
    free(A);
    return 0;
}

int coral_forward_queue_init(struct coral_forward_queue *const object) {
    if (!object) {
        return CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL;
    }
    *object = (struct coral_forward_queue) {0};
    return 0;
}

int coral_forward_queue_invalidate(
        struct coral_forward_queue *const object,
        void (*on_destroy)(struct coral_forward_queue_item *item)) {
    if (!object) {
        return CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL;
    }
    int error;
    struct rock_forward_list_node *node = object->head;
    if (node) {
        struct node *prev = NULL;
        do {
            free(prev);
            struct node *item = rock_container_of(node, struct node, node);
            if (on_destroy) {
                on_destroy((struct coral_forward_queue_item *const) &item->data);
            }
            prev = item;
        } while (!(error = rock_forward_list_next(node, &node)));
        free(prev);
        seagrass_required_true(ROCK_FORWARD_LIST_ERROR_END_OF_SEQUENCE
                               == error);
    }
    *object = (struct coral_forward_queue) {0};
    return 0;
}

int coral_forward_queue_count(const struct coral_forward_queue *const object,
                              uintmax_t *const out) {
    if (!object) {
        return CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL;
    }
    *out = object->count;
    return 0;
}

int coral_forward_queue_add(struct coral_forward_queue *const object,
                            struct coral_forward_queue_item *const item) {
    if (!object) {
        return CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_FORWARD_QUEUE_ERROR_ITEM_IS_NULL;
    }
    struct node *const node = rock_container_of(item, struct node, data);
    if (!object->tail) {
        object->head = &node->node;
    } else {
        seagrass_required_true(!rock_forward_list_insert(
                object->tail, &node->node));
    }
    object->tail = &node->node;
    seagrass_required_true(!seagrass_uintmax_t_add(
            1, object->count, &object->count));
    return 0;
}

int coral_forward_queue_remove(struct coral_forward_queue *const object,
                               struct coral_forward_queue_item **const out) {
    if (!object) {
        return CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL;
    }
    if (!object->head) {
        return CORAL_FORWARD_QUEUE_ERROR_QUEUE_IS_EMPTY;
    }
    int error;
    struct node *item = rock_container_of(object->head, struct node, node);
    if ((error = rock_forward_list_next(object->head, &object->head))) {
        seagrass_required_true(ROCK_FORWARD_LIST_ERROR_END_OF_SEQUENCE
                               == error);
        object->head = NULL;
        object->tail = NULL;
    }
    seagrass_required_true(!seagrass_uintmax_t_subtract(
            object->count, 1, &object->count));
    *out = (struct coral_forward_queue_item *) &item->data;
    return 0;
}

int coral_forward_queue_peek(const struct coral_forward_queue *const object,
                             struct coral_forward_queue_item **const out) {
    if (!object) {
        return CORAL_FORWARD_QUEUE_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_FORWARD_QUEUE_ERROR_OUT_IS_NULL;
    }
    if (!object->head) {
        return CORAL_FORWARD_QUEUE_ERROR_QUEUE_IS_EMPTY;
    }
    struct node *item = rock_container_of(object->head, struct node, node);
    *out = (struct coral_forward_queue_item *) &item->data;
    return 0;
}
