#ifndef _CORAL_RED_BLACK_TREE_MAP_H_
#define _CORAL_RED_BLACK_TREE_MAP_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <rock.h>

#define CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL               1
#define CORAL_RED_BLACK_TREE_MAP_ERROR_COMPARE_IS_NULL              2
#define CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_SIZE_IS_ZERO             3
#define CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_SIZE_IS_ZERO           4
#define CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE      5
#define CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL                  6
#define CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL                  7
#define CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL                8
#define CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL                9
#define CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_ALREADY_EXISTS           10
#define CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND                11
#define CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED     12
#define CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY                 13
#define CORAL_RED_BLACK_TREE_MAP_ERROR_END_OF_SEQUENCE              14

struct coral_red_black_tree_map {
    struct rock_red_black_tree tree;
    size_t key;
    size_t padding;
    size_t value;
    size_t alloc;

    int (*compare)(const void *, const void *);
};

struct coral_red_black_tree_map_entry;

/**
 * @brief Initialize red black tree map.
 * @param [in] object instance to be initialized.
 * @param [in] key size of key.
 * @param [in] value size of value.
 * @param [in] compare comparison which must return an integer less than,
 * equal to, or greater than zero if the <u>first key</u> is considered
 * to be respectively less than, equal to, or greater than the <u>second
 * key</u>.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_COMPARE_IS_NULL if compare is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_SIZE_IS_ZERO if key is zero.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_SIZE_IS_ZERO if value is zero.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_SIZE_IS_TOO_LARGE if the sum of
 * key size and value size is too large.
 */
bool coral_red_black_tree_map_init(struct coral_red_black_tree_map *object,
                                   size_t key,
                                   size_t value,
                                   int (*compare)(const void *first,
                                                  const void *second));

/**
 * @brief Invalidate red black tree map.
 * <p>All the entries contained within the tree map will have the given <i>on
 * destroy</i> callback invoked upon itself. The actual <u>tree map instance
 * is not deallocated</u> since it may have been embedded in a larger
 * structure.</p>
 * @param [in] object instance to be invalidated.
 * @param [in] on_destroy called just before the entry is to be destroyed.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_TREE_MAP_ERROR_OBJECT_IS_NULL if object is <i>NULL</i>.
 */
bool
coral_red_black_tree_map_invalidate(struct coral_red_black_tree_map *object,
                                    void (*on_destroy)(void *key,
                                                       void *value));

/**
 * @brief Retrieve the size of the key.
 * @param [in] object tree map instance.
 * @param [out] out receive the size of the key.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool
coral_red_black_tree_map_key_size(const struct coral_red_black_tree_map *object,
                                  size_t *out);

/**
 * @brief Retrieve the size of the value.
 * @param [in] object tree map instance.
 * @param [out] out receive the size of the value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool coral_red_black_tree_map_value_size(
        const struct coral_red_black_tree_map *object,
        size_t *out);

/**
 * @brief Retrieve the count of entries.
 * @param [in] object instance whose count we are to retrieve.
 * @param [out] out receive the entry count.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool coral_red_black_tree_map_count(
        const struct coral_red_black_tree_map *object,
        uintmax_t *out);

/**
 * @brief Add a key-value association.
 * @param [in] object tree map instance.
 * @param [in] key to which the value will be associated with.
 * @param [in] value that will returned from a lookup of key.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL if value is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_ALREADY_EXISTS if the key is
 * already present in the tree map.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MEMORY_ALLOCATION_FAILED if there is
 * not enough memory to add key-value association to the tree map.
 */
bool coral_red_black_tree_map_add(struct coral_red_black_tree_map *object,
                                  const void *key,
                                  const void *value);

/**
 * @brief Remove key-value association.
 * @param [in] object tree map instance.
 * @param [in] key for which we would like the key-value association removed.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key could not be
 * found.
 */
bool coral_red_black_tree_map_remove(struct coral_red_black_tree_map *object,
                                     const void *key);

/**
 * @brief Check if tree map contains the given key.
 * @param [in] object tree map instance.
 * @param [in] key to check if it is present.
 * @param [out] out true if key is present, otherwise false.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool coral_red_black_tree_map_contains(
        const struct coral_red_black_tree_map *object,
        const void *key,
        bool *out);

/**
 * @brief Set the value for the given key.
 * @param [in] object tree map instance.
 * @param [in] key used for which value is to be changed.
 * @param [in] value of new association with given key.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL if value is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key could not be
 * found.
 */
bool coral_red_black_tree_map_set(struct coral_red_black_tree_map *object,
                                  const void *key,
                                  const void *value);

/**
 * @brief Retrieve the value for the given key.
 * @param [in] object tree map instance.
 * @param [in] key used to get the associated value.
 * @param [out] out receive the <u>address of</u> value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key could not be
 * found.
 */
bool coral_red_black_tree_map_get(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const void **out);

/**
 * @brief Retrieve the value for the given key or the next higher key.
 * @param [in] object tree map instance.
 * @param [in] key to find or the next higher key.
 * @param [out] out receive the <u>address of</u> value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key nor a
 * higher key is in the tree map instance.
 */
bool coral_red_black_tree_map_ceiling(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const void **out);

/**
 * @brief Retrieve the value for the given key or the next lower key.
 * @param [in] object tree map instance.
 * @param [in] key to find or the next lower key.
 * @param [out] out receive the <u>address of</u> value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key nor a lower key
 * is in the tree map instance.
 */
bool coral_red_black_tree_map_floor(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const void **out);

/**
 * @brief Retrieve the value for the next higher key.
 * @param [in] object tree map instance.
 * @param [in] key whose next higher key we are trying to find.
 * @param [out] out receive the <u>address of</u> value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if no higher key
 * is in the tree map instance.
 */
bool coral_red_black_tree_map_higher(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const void **out);

/**
 * @brief Retrieve the value for the next lower key.
 * @param [in] object tree map instance.
 * @param [in] key whose next lower key we are trying to find.
 * @param [out] out receive the <u>address of</u> value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if no lower value for
 * key is in the tree map instance.
 */
bool coral_red_black_tree_map_lower(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const void **out);

/**
 * @brief Retrieve value of first entry.
 * @param [in] object tree map instance.
 * @param [out] out receive the <u>address of</u> the first entry's value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY if tree map is empty.
 */
bool coral_red_black_tree_map_first(
        const struct coral_red_black_tree_map *object,
        const void **out);

/**
 * @brief Retrieve value of last entry.
 * @param [in] object tree map instance.
 * @param [out] out receive the <u>address of</u> the last entry's value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY if tree map is empty.
 */
bool coral_red_black_tree_map_last(
        const struct coral_red_black_tree_map *object,
        const void **out);

/**
 * @brief Retrieve entry for the given key.
 * @param [in] object tree map instance.
 * @param [in] key of the entry we are looking for.
 * @param [out] out receive the <u>address of</u> entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key could not be
 * found.
 */
bool coral_red_black_tree_map_get_entry(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve the entry for the given key or the next higher key.
 * @param [in] object tree map instance.
 * @param [in] key to find or the next higher key.
 * @param [out] out receive the <u>address of</u> entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key nor a higher
 * key is in the tree map instance.
 */
bool coral_red_black_tree_map_ceiling_entry(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve the entry for the given key or the next lower key.
 * @param [in] object tree map instance.
 * @param [in] key to find or the next lower key.
 * @param [out] out receive the <u>address of</u> entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if key nor a lower key
 * is in the tree map instance.
 */
bool coral_red_black_tree_map_floor_entry(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve the entry for the next higher key.
 * @param [in] object tree map instance.
 * @param [in] key whose next higher key we are trying to find.
 * @param [out] out receive the <u>address of</u> entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_NOT_FOUND if no higher key
 * is in the tree map instance.
 */
bool coral_red_black_tree_map_higher_entry(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve the entry for the next lower key.
 * @param [in] object tree map instance.
 * @param [in] key whose next lower key we are trying to find.
 * @param [out] out receive the <u>address of</u> entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_KEY_IS_NULL if key is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ITEM_NOT_FOUND if no lower value for
 * key is in the tree map instance.
 */
bool coral_red_black_tree_map_lower_entry(
        const struct coral_red_black_tree_map *object,
        const void *key,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve first entry.
 * @param [in] object tree map instance.
 * @param [out] out receive the <u>address of</u> the first entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY if tree map is empty.
 */
bool coral_red_black_tree_map_first_entry(
        const struct coral_red_black_tree_map *object,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve last entry.
 * @param [in] object tree map instance.
 * @param [out] out receive the <u>address of</u> the last entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_MAP_IS_EMPTY if tree map is empty.
 */
bool coral_red_black_tree_map_last_entry(
        const struct coral_red_black_tree_map *object,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Remove entry.
 * @param [in] object tree map instance.
 * @param [in] entry <u>address of</u> entry to be removed.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL if entry is <i>NULL</i>.
 */
bool coral_red_black_tree_map_remove_entry(
        struct coral_red_black_tree_map *object,
        const struct coral_red_black_tree_map_entry *entry);

/**
 * @brief Retrieve the next entry.
 * @param [in] entry <u>address of</u> current entry.
 * @param [out] out receive the <u>address of</u> the next entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL if entry is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_END_OF_SEQUENCE if there is no next
 * entry.
 */
bool coral_red_black_tree_map_next_entry(
        const struct coral_red_black_tree_map_entry *entry,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve the previous entry.
 * @param [in] entry <u>address of</u> current entry.
 * @param [out] out receive the <u>address of</u> the previous entry.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL if entry is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_END_OF_SEQUENCE if there is no
 * previous entry.
 */
bool coral_red_black_tree_map_prev_entry(
        const struct coral_red_black_tree_map_entry *entry,
        const struct coral_red_black_tree_map_entry **out);

/**
 * @brief Retrieve the entry's key.
 * @param [in] object tree map instance.
 * @param [in] entry <u>address of</u> entry contained within the tree map
 * instance.
 * @param [out] out receive the <u>address of</u> the entry's key.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL if entry is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool coral_red_black_tree_map_entry_key(
        const struct coral_red_black_tree_map *object,
        const struct coral_red_black_tree_map_entry *entry,
        const void **out);

/**
 * @brief Retrieve the entry's value.
 * @param [in] object tree map instance.
 * @param [in] entry <u>address of</u> entry contained within the tree map
 * instance.
 * @param [out] out receive the <u>address of</u> the entry's value.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL if entry is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OUT_IS_NULL if out is <i>NULL</i>.
 */
bool coral_red_black_tree_map_entry_get_value(
        const struct coral_red_black_tree_map *object,
        const struct coral_red_black_tree_map_entry *entry,
        const void **out);

/**
 * @brief Set the entry's value.
 * @param [in] object tree map instance.
 * @param [in] entry <u>address of</u> entry contained within the tree map
 * instance.
 * @param [in] value to set entry's value to.
 * @return On success true, otherwise false if an error has occurred.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_OBJECT_IS_NULL if object is
 * <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_ENTRY_IS_NULL if entry is <i>NULL</i>.
 * @throws CORAL_RED_BLACK_TREE_MAP_ERROR_VALUE_IS_NULL if value is <i>NULL</i>.
 */
bool coral_red_black_tree_map_entry_set_value(
        const struct coral_red_black_tree_map *object,
        const struct coral_red_black_tree_map_entry *entry,
        const void *value);

#endif /* _CORAL_RED_BLACK_TREE_MAP_H_ */
