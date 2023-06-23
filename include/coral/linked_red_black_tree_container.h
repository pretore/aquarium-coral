#ifndef _CORAL_LINKED_RED_BLACK_TREE_CONTAINER_H_
#define _CORAL_LINKED_RED_BLACK_TREE_CONTAINER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sea-urchin.h>
#include <rock.h>

#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_ZERO \
    SEA_URCHIN_ERROR_VALUE_IS_ZERO
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_TOO_LARGE \
    SEA_URCHIN_ERROR_VALUE_IS_TOO_LARGE
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL \
    SEA_URCHIN_ERROR_OUT_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_MEMORY_ALLOCATION_FAILED \
    SEA_URCHIN_ERROR_MEMORY_ALLOCATION_FAILED
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL \
    SEA_URCHIN_ERROR_ITEM_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL \
    ROCK_RED_BLACK_TREE_ERROR_OBJECT_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_COMPARE_IS_NULL \
    ROCK_RED_BLACK_TREE_ERROR_COMPARE_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXISTS \
    ROCK_RED_BLACK_TREE_ERROR_NODE_ALREADY_EXISTS
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL \
    SEA_URCHIN_ERROR_KEY_IS_NULL
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND \
    ROCK_RED_BLACK_TREE_ERROR_NODE_NOT_FOUND
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY \
    ROCK_RED_BLACK_TREE_ERROR_TREE_IS_EMPTY
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE \
    ROCK_RED_BLACK_TREE_ERROR_END_OF_SEQUENCE
#define CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ITEM_IS_NULL \
    SEA_URCHIN_ERROR_VALUE_IS_NULL

struct coral_linked_red_black_tree_container_entry;

struct coral_linked_red_black_tree_container {
    struct rock_red_black_tree tree;
    struct rock_linked_list_node *list;

    int (*compare)(const void *, const void *);
};

/**
 * @brief Allocate memory for an entry.
 * @param [in] size in bytes of entry to create.
 * @param [out] out receive allocated entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_ZERO if size
 * is zero.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_SIZE_IS_TOO_LARGE if
 * size is too large.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_MEMORY_ALLOCATION_FAILED
 * if there is not enough memory to allocate an entry.
 */
int coral_linked_red_black_tree_container_alloc(
        size_t size,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Free memory of an entry.
 * @param [in] entry to be freed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 */
int coral_linked_red_black_tree_container_free(
        struct coral_linked_red_black_tree_container_entry *entry);

/**
 * @brief Initialize red black tree container.
 * @param [in] object instance to be initialized.
 * @param [in] compare comparison which must return an integer less than,
 * equal to, or greater than zero if the <u>first key</u> is considered
 * to be respectively less than, equal to, or greater than the <u>second
 * key</u>.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_COMPARE_IS_NULL if
 * compare is <i>NULL</i>
 */
int coral_linked_red_black_tree_container_init(
        struct coral_linked_red_black_tree_container *object,
        int (*compare)(const void *first,
                       const void *second));

/**
 * @brief Invalidate red black tree container.
 * <p>The entries in the container are destroyed and each entry will have
 * the provided <i>on destroy</i> callback invoked upon itself. The actual
 * <u>container instance is not deallocated</u> since it may have been embedded
 * in a larger structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the node is to be destroyed.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 */
int coral_linked_red_black_tree_container_invalidate(
        struct coral_linked_red_black_tree_container *object,
        void (*on_destroy)(
                struct coral_linked_red_black_tree_container_entry *entry));

/**
 * @brief Retrieve the count of entries.
 * @param [in] object instance whose count we are to retrieve.
 * @param [out] out receive the entry count.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 */
int coral_linked_red_black_tree_container_count(
        const struct coral_linked_red_black_tree_container *object,
        uintmax_t *out);

/**
 * @brief Add entry.
 * @param [in] object container instance.
 * @param [in] entry to add.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXITS if
 * entry is already present in container.
 */
int coral_linked_red_black_tree_container_add(
        struct coral_linked_red_black_tree_container *object,
        const struct coral_linked_red_black_tree_container_entry *entry);

/**
 * @brief Remove entry.
 * @param [in] object container instance.
 * @param [in] entry to remove.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 */
int coral_linked_red_black_tree_container_remove(
        struct coral_linked_red_black_tree_container *object,
        const struct coral_linked_red_black_tree_container_entry *entry);

/**
 * @brief Retrieve entry for key.
 * @param [in] object container instance.
 * @param [in] key on which to find entry.
 * @param [out] out receive matching entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL if key is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND if
 * matching entry was not found.
 */
int coral_linked_red_black_tree_container_get(
        const struct coral_linked_red_black_tree_container *object,
        const void *key,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve entry or next higher for key.
 * @param [in] object container instance.
 * @param [in] key on which to find entry or next higher.
 * @param [out] out receive matching entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL if key is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND if
 * matching entry or next higher entry was not found.
 */
int coral_linked_red_black_tree_container_ceiling(
        const struct coral_linked_red_black_tree_container *object,
        const void *key,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve entry or next lower for key.
 * @param [in] object container instance.
 * @param [in] key on which to find entry or next lower.
 * @param [out] out receive matching entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL if key is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND if
 * matching entry or next lower entry was not found.
 */
int coral_linked_red_black_tree_container_floor(
        const struct coral_linked_red_black_tree_container *object,
        const void *key,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve next higher entry for key.
 * @param [in] object container instance.
 * @param [in] key on which to find next higher entry.
 * @param [out] out receive matching entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL if key is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND if next
 * higher entry was not found.
 */
int coral_linked_red_black_tree_container_higher(
        const struct coral_linked_red_black_tree_container *object,
        const void *key,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve next lower entry for key.
 * @param [in] object container instance.
 * @param [in] key on which to find next lower entry.
 * @param [out] out receive matching entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_KEY_IS_NULL if key is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_NOT_FOUND if next
 * lower entry was not found.
 */
int coral_linked_red_black_tree_container_lower(
        const struct coral_linked_red_black_tree_container *object,
        const void *key,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Receive sorted first entry.
 * @param [in] object container instance.
 * @param [out] out receive sorted first entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY if
 * container is empty.
 */
int coral_linked_red_black_tree_container_sorted_first(
        const struct coral_linked_red_black_tree_container *object,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Receive sorted last entry.
 * @param [in] object container instance.
 * @param [out] out receive sorted last entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY if
 * container is empty.
 */
int coral_linked_red_black_tree_container_sorted_last(
        const struct coral_linked_red_black_tree_container *object,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve the next sorted entry.
 * @param [in] entry current one.
 * @param [out] out receive the next sorted entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE if there
 * are no next entries.
 */
int coral_linked_red_black_tree_container_sorted_next(
        const struct coral_linked_red_black_tree_container_entry *entry,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve the previous sorted entry.
 * @param [in] entry current one.
 * @param [out] out receive the previous sorted entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE if there
 * are no previous entries.
 */
int coral_linked_red_black_tree_container_sorted_prev(
        const struct coral_linked_red_black_tree_container_entry *entry,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Receive first entry.
 * @param [in] object container instance.
 * @param [out] out receive first entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY if
 * container is empty.
 */
int coral_linked_red_black_tree_container_first(
        const struct coral_linked_red_black_tree_container *object,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Receive last entry.
 * @param [in] object container instance.
 * @param [out] out receive last entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_CONTAINER_IS_EMPTY if
 * container is empty.
 */
int coral_linked_red_black_tree_container_last(
        const struct coral_linked_red_black_tree_container *object,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve the next entry.
 * @param [in] object container instance.
 * @param [in] entry current one.
 * @param [out] out receive the next entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE if there
 * are no next entries.
 */
int coral_linked_red_black_tree_container_next(
        const struct coral_linked_red_black_tree_container *object,
        const struct coral_linked_red_black_tree_container_entry *entry,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Retrieve the previous entry.
 * @param [in] object container instance.
 * @param [in] entry current one.
 * @param [out] out receive the previous entry.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OUT_IS_NULL if out is
 * <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_END_OF_SEQUENCE if there
 * are no previous entries.
 */
int coral_linked_red_black_tree_container_prev(
        const struct coral_linked_red_black_tree_container *object,
        const struct coral_linked_red_black_tree_container_entry *entry,
        struct coral_linked_red_black_tree_container_entry **out);

/**
 * @brief Insert item at entry.
 * @param [in] object container instance.
 * @param [in] entry at which to insert.
 * @param [in] item to insert.
 * @return On success <i>0</i>, otherwise an error code.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_OBJECT_IS_NULL if
 * object is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_IS_NULL if entry
 * is <i>NULL</i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ITEM_IS_NULL if item
 * is <i>NULL/i>.
 * @throws CORAL_LINKED_RED_BLACK_TREE_CONTAINER_ERROR_ENTRY_ALREADY_EXITS if
 * entry is already present in container.
 */
int coral_linked_red_black_tree_container_insert(
        struct coral_linked_red_black_tree_container *object,
        struct coral_linked_red_black_tree_container_entry *entry,
        struct coral_linked_red_black_tree_container_entry *item);

#endif /* _CORAL_LINKED_RED_BLACK_TREE_CONTAINER_H_ */
