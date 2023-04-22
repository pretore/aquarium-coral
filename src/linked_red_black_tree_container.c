#include <stdlib.h>
#include <errno.h>
#include <seagrass.h>
#include <coral.h>

#ifdef TEST
#include <test/cmocka.h>
#endif

struct coral_linked_red_black_tree_container_entry {
    void *data;
};

struct entry {
    struct rock_red_black_tree_node rbt_node;
    struct rock_linked_list_node ll_node;
    unsigned char data[];
};

int coral_linked_red_black_tree_container_alloc(
        const size_t size,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!size) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_ZERO;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    int error;
    uintmax_t alloc;
    if ((error = seagrass_uintmax_t_add(size, sizeof(struct entry), &alloc))
        || alloc > SIZE_MAX) {
        seagrass_required_true(SEAGRASS_UINTMAX_T_ERROR_RESULT_IS_INCONSISTENT
                               == error || !error);
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_TOO_LARGE;
    }
    struct entry *entry;
    if ((error = posix_memalign((void **) &entry, sizeof(void *), alloc))) {
        seagrass_required_true(ENOMEM == error);
        return
        CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    seagrass_required_true(!rock_red_black_tree_node_init(
            &entry->rbt_node));
    seagrass_required_true(!rock_linked_list_node_init(
            &entry->ll_node));
    *out = (struct coral_linked_red_black_tree_container_entry *) &entry->data;
    return 0;
}

int coral_linked_red_black_tree_container_free(
        struct coral_linked_red_black_tree_container_entry *const entry) {
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    struct entry *const A = rock_container_of(entry, struct entry, data);
    free(A);
    return 0;
}

static _Thread_local const struct coral_linked_red_black_tree_container *this;

static int entity_compare(const struct rock_red_black_tree_node *const a,
                          const struct rock_red_black_tree_node *const b) {
    const struct entry *const A = rock_container_of(a, struct entry, rbt_node);
    const struct entry *const B = rock_container_of(b, struct entry, rbt_node);
    return this->compare(&A->data, &B->data);
}

int coral_linked_red_black_tree_container_init(
        struct coral_linked_red_black_tree_container *const object,
        int (*const compare)(const void *first,
                             const void *second)) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!compare) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_COMPARE_IS_NULL;
    }
    *object = (struct coral_linked_red_black_tree_container) {0};
    seagrass_required_true(!rock_red_black_tree_init(
            &object->tree, entity_compare));
    object->compare = compare;
    return 0;
}

int coral_linked_red_black_tree_container_invalidate(
        struct coral_linked_red_black_tree_container *object,
        void (*const on_destroy)(
                struct coral_linked_red_black_tree_container_entry *entry)) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    int error;
    struct coral_linked_red_black_tree_container_entry *entry;
    if ((error = coral_linked_red_black_tree_container_first(object, &entry))) {
        seagrass_required_true(
                CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY
                == error);
    } else {
        while (true) {
            struct coral_linked_red_black_tree_container_entry *next;
            if ((error = coral_linked_red_black_tree_container_next(entry, &next))) {
                seagrass_required_true(
                        CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE
                        == error);
            }
            seagrass_required_true(!coral_linked_red_black_tree_container_remove(
                            object, entry));
            if (on_destroy) {
                on_destroy(entry);
            }
            seagrass_required_true(!coral_linked_red_black_tree_container_free(
                    entry));
            if (!error) {
                entry = next;
            } else {
                break;
            }
        }
    }
    seagrass_required_true(!rock_red_black_tree_invalidate(
            &object->tree, NULL));
    *object = (struct coral_linked_red_black_tree_container) {0};
    return 0;
}

int coral_linked_red_black_tree_container_count(
        const struct coral_linked_red_black_tree_container *const object,
        uintmax_t *const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    seagrass_required_true(!rock_red_black_tree_count(
            &object->tree, out));
    return 0;
}

int coral_linked_red_black_tree_container_add(
        struct coral_linked_red_black_tree_container *const object,
        const struct coral_linked_red_black_tree_container_entry *const entry) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    int error;
    struct entry *const A = rock_container_of(entry, struct entry, data);
    struct rock_red_black_tree_node *insertion_point;
    this = object;
    if (!(error = rock_red_black_tree_find(&object->tree,
                                           NULL,
                                           &A->rbt_node,
                                           &insertion_point))) {
        return CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXITS;
    }
    seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                           == error);
    seagrass_required_true(!rock_red_black_tree_insert(
            &object->tree, insertion_point, &A->rbt_node));
    if (!object->list) {
        object->list = &A->ll_node;
    } else {
        seagrass_required_true(!rock_linked_list_insert_before(
                object->list, &A->ll_node));
    }
    return 0;
}

int coral_linked_red_black_tree_container_remove(
        struct coral_linked_red_black_tree_container *const object,
        const struct coral_linked_red_black_tree_container_entry *const entry) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    struct entry *const A = rock_container_of(entry, struct entry, data);
    if (&A->ll_node == object->list) {
        struct rock_linked_list_node *next;
        seagrass_required_true(!rock_linked_list_next(
                &A->ll_node, &next));
        if (&A->ll_node == next) {
            object->list = NULL;
        } else {
            object->list = next;
        }
    }
    seagrass_required_true(!rock_linked_list_remove(&A->ll_node));
    seagrass_required_true(!rock_red_black_tree_remove(
            &object->tree, &A->rbt_node));
    return 0;
}

int coral_linked_red_black_tree_container_get(
        const struct coral_linked_red_black_tree_container *const object,
        const void *const key,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    int error;
    struct entry *const K = rock_container_of(key, struct entry, data);
    struct rock_red_black_tree_node *node;
    this = object;
    if ((error = rock_red_black_tree_find(&object->tree,
                                          NULL,
                                          &K->rbt_node,
                                          &node))) {
        seagrass_required_true(
                ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                == error);
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_ceiling(
        const struct coral_linked_red_black_tree_container *const object,
        const void *const key,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    int error;
    struct entry *const K = rock_container_of(key, struct entry, data);
    struct rock_red_black_tree_node *node;
    this = object;
    if ((error = rock_red_black_tree_find(&object->tree,
                                          NULL,
                                          &K->rbt_node,
                                          &node))) {
        seagrass_required_true(
                ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                == error);
        if (!node) {
            return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
        }
        struct entry *const B = rock_container_of(node, struct entry, rbt_node);
        if (entity_compare(&K->rbt_node, &B->rbt_node) > 0) {
            return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_floor(
        const struct coral_linked_red_black_tree_container *const object,
        const void *const key,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    int error;
    struct entry *const K = rock_container_of(key, struct entry, data);
    struct rock_red_black_tree_node *node;
    this = object;
    if ((error = rock_red_black_tree_find(&object->tree,
                                          NULL,
                                          &K->rbt_node,
                                          &node))) {
        seagrass_required_true(
                ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                == error);
        if (!node) {
            return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
        }
        if ((error = rock_red_black_tree_prev(node, &node))) {
            seagrass_required_true(
                    ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                    == error);
            return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
        }
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_higher(
        const struct coral_linked_red_black_tree_container *const object,
        const void *const key,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    int error;
    struct entry *const K = rock_container_of(key, struct entry, data);
    struct rock_red_black_tree_node *node;
    this = object;
    if ((error = rock_red_black_tree_find(&object->tree,
                                          NULL,
                                          &K->rbt_node,
                                          &node))) {
        seagrass_required_true(
                ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                == error);
        if (!node) {
            return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
        }
        struct entry *const B = rock_container_of(node, struct entry, rbt_node);
        if (entity_compare(&K->rbt_node, &B->rbt_node) > 0) {
            return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
        }
    } else if ((error = rock_red_black_tree_next(node, &node))) {
        seagrass_required_true(
                ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                == error);
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_lower(
        const struct coral_linked_red_black_tree_container *const object,
        const void *const key,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!key) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    int error;
    struct entry *const K = rock_container_of(key, struct entry, data);
    struct rock_red_black_tree_node *node;
    this = object;
    if ((error = rock_red_black_tree_find(&object->tree,
                                          NULL,
                                          &K->rbt_node,
                                          &node))) {
        seagrass_required_true(
                ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                == error);
        if (!node) {
            return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
        }
    }
    if ((error = rock_red_black_tree_prev(node, &node))) {
        seagrass_required_true(
                ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
                == error);
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND;
    }
    struct entry *const A = rock_container_of(node, struct entry, rbt_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_first(
        const struct coral_linked_red_black_tree_container *const object,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    if (!object->list) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY;
    }
    const struct entry *const A
            = rock_container_of(object->list, struct entry, ll_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_last(
        const struct coral_linked_red_black_tree_container *const object,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    if (!object->list) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY;
    }
    const struct entry *const B
            = rock_container_of(object->list, struct entry, ll_node);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_prev(
            &B->ll_node, &node));
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_next(
        struct coral_linked_red_black_tree_container_entry *const entry,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    const struct entry *const B = rock_container_of(entry, struct entry, data);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_next(
            &B->ll_node, &node));
    if (&B->ll_node == node) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE;
    }
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_prev(
        struct coral_linked_red_black_tree_container_entry *const entry,
        struct coral_linked_red_black_tree_container_entry **const out) {
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    if (!out) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL;
    }
    const struct entry *const B = rock_container_of(entry, struct entry, data);
    struct rock_linked_list_node *node;
    seagrass_required_true(!rock_linked_list_prev(
            &B->ll_node, &node));
    if (&B->ll_node == node) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE;
    }
    const struct entry *const A
            = rock_container_of(node, struct entry, ll_node);
    *out = (struct coral_linked_red_black_tree_container_entry *) &A->data;
    return 0;
}

int coral_linked_red_black_tree_container_insert_after(
        struct coral_linked_red_black_tree_container *const object,
        struct coral_linked_red_black_tree_container_entry *const entry,
        struct coral_linked_red_black_tree_container_entry *const item) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ITEM_IS_NULL;
    }
    int error;
    struct entry *const A = rock_container_of(item, struct entry, data);
    struct rock_red_black_tree_node *insertion_point;
    this = object;
    if (!(error = rock_red_black_tree_find(&object->tree,
                                           NULL,
                                           &A->rbt_node,
                                           &insertion_point))) {
        return CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXITS;
    }
    seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                           == error);
    seagrass_required_true(!rock_red_black_tree_insert(
            &object->tree, insertion_point, &A->rbt_node));
    struct entry *const B = rock_container_of(entry, struct entry, data);
    seagrass_required_true(!rock_linked_list_insert_after(
            &B->ll_node, &A->ll_node));
    return 0;
}

int coral_linked_red_black_tree_container_insert_before(
        struct coral_linked_red_black_tree_container *const object,
        struct coral_linked_red_black_tree_container_entry *const entry,
        struct coral_linked_red_black_tree_container_entry *const item) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    if (!item) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ITEM_IS_NULL;
    }
    int error;
    struct entry *const A = rock_container_of(item, struct entry, data);
    struct rock_red_black_tree_node *insertion_point;
    this = object;
    if (!(error = rock_red_black_tree_find(&object->tree,
                                           NULL,
                                           &A->rbt_node,
                                           &insertion_point))) {
        return CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXITS;
    }
    seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                           == error);
    seagrass_required_true(!rock_red_black_tree_insert(
            &object->tree, insertion_point, &A->rbt_node));
    struct entry *const B = rock_container_of(entry, struct entry, data);
    seagrass_required_true(!rock_linked_list_insert_before(
            &B->ll_node, &A->ll_node));
    if (&B->ll_node == object->list) {
        object->list = &A->ll_node;
    }
    return 0;
}

int coral_linked_red_black_tree_container_append(
        struct coral_linked_red_black_tree_container *const object,
        struct coral_linked_red_black_tree_container_entry *const entry) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    int error;
    if ((error = coral_linked_red_black_tree_container_add(object, entry))) {
        seagrass_required_true(
                CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXITS
                == error);
    }
    return error;
}

int coral_linked_red_black_tree_container_prepend(
        struct coral_linked_red_black_tree_container *const object,
        struct coral_linked_red_black_tree_container_entry *const entry) {
    if (!object) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL;
    }
    if (!entry) {
        return CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL;
    }
    int error;
    struct entry *const A = rock_container_of(entry, struct entry, data);
    struct rock_red_black_tree_node *insertion_point;
    this = object;
    if (!(error = rock_red_black_tree_find(&object->tree,
                                           NULL,
                                           &A->rbt_node,
                                           &insertion_point))) {
        return CORAL_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXITS;
    }
    seagrass_required_true(ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
                           == error);
    seagrass_required_true(!rock_red_black_tree_insert(
            &object->tree, insertion_point, &A->rbt_node));
    if (object->list) {
        seagrass_required_true(!rock_linked_list_insert_before(
                object->list, &A->ll_node));
    }
    object->list = &A->ll_node;
    return 0;
}