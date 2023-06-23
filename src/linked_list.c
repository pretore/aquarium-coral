#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <seagrass.h>
#include <coral.h>

#ifdef TEST
#include <test/cmocka.h>
#endif

struct coral_linked_list_item {
    void *data;
};

struct item {
    struct rock_linked_list_node node;
    unsigned char data[];
};

int coral_linked_list_alloc(const size_t size,
                            struct coral_linked_list_item **const out) {
    if (!size) {
        return CORAL_LINKED_LIST_ERROR_SIZE_IS_ZERO;
    }
    if (!out) {
        return CORAL_LINKED_LIST_ERROR_OUT_IS_NULL;
    }
    int error;
    uintmax_t alloc;
    if ((error = seagrass_uintmax_t_add(size, sizeof(struct item), &alloc))
        || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_LINKED_LIST_ERROR_SIZE_IS_TOO_LARGE;
    }
    struct item *item;
    if ((error = posix_memalign((void **) &item, sizeof(void *), alloc))) {
        seagrass_required_true(ENOMEM == error);
        return CORAL_LINKED_LIST_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    seagrass_required_true(!rock_linked_list_node_init(&item->node));
    *out = (struct coral_linked_list_item *) &item->data;
    return 0;
}

int coral_linked_list_free(struct coral_linked_list_item *const item) {
    if (!item) {
        return CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL;
    }
    struct item *const A = rock_container_of(item, struct item, data);
    free(A);
    return 0;
}

int coral_linked_list_init(struct coral_linked_list *const object) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    *object = (struct coral_linked_list) {0};
    return 0;
}

int coral_linked_list_invalidate(
        struct coral_linked_list *const object,
        void (*const on_destroy)(struct coral_linked_list_item *)) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    int error;
    struct coral_linked_list_item *item;
    if ((error = coral_linked_list_first(object, &item))) {
        seagrass_required_true(
                CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY
                == error);
    } else {
        while (true) {
            struct coral_linked_list_item *next;
            if ((error = coral_linked_list_next(object, item, &next))) {
                seagrass_required_true(
                        CORAL_LINKED_LIST_ERROR_END_OF_SEQUENCE
                        == error);
            }
            seagrass_required_true(!coral_linked_list_remove(object, item));
            if (on_destroy) {
                on_destroy(item);
            }
            seagrass_required_true(!coral_linked_list_free(item));
            if (error) {
                break;
            }
            item = next;
        }
    }
    *object = (struct coral_linked_list) {0};
    return 0;
}

int coral_linked_list_count(struct coral_linked_list *const object,
                            uintmax_t *const out) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_LIST_ERROR_OUT_IS_NULL;
    }
    *out = object->count;
    return 0;
}

int coral_linked_list_add(struct coral_linked_list *const object,
                          struct coral_linked_list_item *const item) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL;
    }
    seagrass_required_true(!seagrass_uintmax_t_add(
            1, object->count, &object->count));
    struct item *const A = rock_container_of(item, struct item, data);
    if (!object->list) {
        object->list = &A->node;
    } else {
        seagrass_required_true(!rock_linked_list_insert_before(
                object->list, &A->node));
    }
    return 0;
}

int coral_linked_list_insert(struct coral_linked_list *const object,
                             struct coral_linked_list_item *const item,
                             struct coral_linked_list_item *const value) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL;
    }
    if (!value) {
        return CORAL_LINKED_LIST_ERROR_VALUE_IS_NULL;
    }
    seagrass_required_true(!seagrass_uintmax_t_add(
            1, object->count, &object->count));
    struct item *const A = rock_container_of(item, struct item, data);
    struct item *const B = rock_container_of(value, struct item, data);
    seagrass_required_true(!rock_linked_list_insert_before(
            &A->node, &B->node));
    if (object->list == &A->node) {
        object->list = &B->node;
    }
    return 0;
}

int coral_linked_list_remove(struct coral_linked_list *const object,
                             struct coral_linked_list_item *const item) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL;
    }
    struct item *const A = rock_container_of(item, struct item, data);
    if (&A->node == object->list) {
        struct rock_linked_list_node *next;
        seagrass_required_true(!rock_linked_list_next(&A->node, &next));
        if (&A->node == next) {
            object->list = NULL;
        } else {
            object->list = next;
        }
    }
    seagrass_required_true(!rock_linked_list_remove(&A->node));
    if (object->count) {
        seagrass_required_true(!seagrass_uintmax_t_subtract(
                object->count, 1, &object->count));
    }
    return 0;
}

int coral_linked_list_first(const struct coral_linked_list *const object,
                            struct coral_linked_list_item **const out) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_LIST_ERROR_OUT_IS_NULL;
    }
    if (!object->count) {
        return CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY;
    }
    const struct item *const A =
            rock_container_of(object->list, struct item, node);
    *out = (struct coral_linked_list_item *) &A->data;
    return 0;
}

int coral_linked_list_last(const struct coral_linked_list *const object,
                           struct coral_linked_list_item **const out) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_LIST_ERROR_OUT_IS_NULL;
    }
    if (!object->count) {
        return CORAL_LINKED_LIST_ERROR_LIST_IS_EMPTY;
    }
    const struct item *const B
            = rock_container_of(object->list, struct item, node);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_prev(&B->node, &node));
    const struct item *const A = rock_container_of(node, struct item, node);
    *out = (struct coral_linked_list_item *) &A->data;
    return 0;
}

int coral_linked_list_next(const struct coral_linked_list *const object,
                           const struct coral_linked_list_item *const item,
                           struct coral_linked_list_item **const out) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_LIST_ERROR_OUT_IS_NULL;
    }
    const struct item *const B = rock_container_of(item, struct item, data);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_next(&B->node, &node));
    if (&B->node == node || object->list == node) {
        return CORAL_LINKED_LIST_ERROR_END_OF_SEQUENCE;
    }
    const struct item *const A = rock_container_of(node, struct item, node);
    *out = (struct coral_linked_list_item *) &A->data;
    return 0;
}

int coral_linked_list_prev(const struct coral_linked_list *const object,
                           const struct coral_linked_list_item *const item,
                           struct coral_linked_list_item **const out) {
    if (!object) {
        return CORAL_LINKED_LIST_ERROR_OBJECT_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_LIST_ERROR_ITEM_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_LIST_ERROR_OUT_IS_NULL;
    }
    const struct item *const B = rock_container_of(item, struct item, data);
    if (&B->node == object->list) {
        return CORAL_LINKED_LIST_ERROR_END_OF_SEQUENCE;
    }
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_prev(&B->node, &node));
    if (&B->node == node) {
        return CORAL_LINKED_LIST_ERROR_END_OF_SEQUENCE;
    }
    const struct item *const A = rock_container_of(node, struct item, node);
    *out = (struct coral_linked_list_item *) &A->data;
    return 0;
}

