## Forward Queue

A first-in-first-out (FIFO) container backed by a forward list.

### Use

Initialise queue.

```c
struct coral_forward_queue object;
// initialise queue
seagrass_required_true(!coral_forward_queue_init(&object));
```

Invalidate queue.

```c
seagrass_required_true(!coral_forward_queue_invalidate(&object, NULL));
```

Add an item to the queue.

```c
// union type is used to inform the compiler that we intend to access pointer 
// types that point to the same item
union item {
    struct coral_forward_queue_item *item;
    uintmax_t *value;
} ptr;

// allocate memory for a new item
seagrass_required_true(!coral_forward_queue_alloc(
        sizeof(*ptr.value), &ptr.entry));
// initialise entry to a random value
*ptr.value = rand(); 
// add item to queue
seagrass_required_true(!coral_forward_queue_add(&object, ptr.entry));
```

Remove an item from the front of the queue.

```c
// union type is used to inform the compiler that we intend to access pointer 
// types that point to the same item
union item {
    struct coral_forward_queue_item *item;
    uintmax_t *value;
} ptr;

// remove an item from the front of the queue.
seahorse_required_tre(!coral_forward_queue_remove(&object, &ptr.entry));
```
