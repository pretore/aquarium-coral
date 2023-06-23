## Linked List

A list backed by a linked list.

### Use

Initialize a list.

```c
struct coral_linked_list object;
// initialize list 
seagrass_required_true(!coral_linked_list_init(&object));
```

Invalidate list.

```c
seagrass_required_true(!coral_linked_list_invalidate(&object, NULL));
```

Add an item to the list.

```c
union {
    struct coral_linked_list_item *item;
    uintmax_t *value;
} ptr;
// allocate an item
seagrass_required_true(!coral_linked_list_alloc(sizeof(*ptr.value), 0));
// set the value
ptr.value = abs(rand());
// add item to the end of the list
seagrass_required_true(!coral_linked_list_add(&object, &value));
```

