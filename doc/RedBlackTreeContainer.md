## Red Black Tree Container

Container backed by a red black tree.

### Use

Initialise container.

```c
// compare function to say in which order uintmax_t are to be stored
static int compare(const void *a, const void *b) {
    return seagrass_uintmax_t_ptr_compare(a, b);
}

struct coral_red_black_tree_container object;
// initialise container
seagrass_required_true(!coral_red_black_tree_container_init(&object, compare));
```

Invalidate container.

```c
seagrass_required_true(!coral_red_black_tree_container_invalidate(
        &object, NULL));
```

Add an entry to the container.

```c
// union type is used to inform the compiler that we intend to access pointer 
// types that point to the same item
union item {
    struct coral_red_black_tree_container_entry *entry;
    uintmax_t *value;
} ptr;

// allocate memory for a new entry
seagrass_required_true(!coral_red_black_tree_container_alloc(
        sizeof(*ptr.value), &ptr.entry));
// initialise entry to a random value
*ptr.value = rand(); 
// add entry to container
seagrass_required_true(!coral_red_black_tree_container_add(
        &object, ptr.entry));
```
