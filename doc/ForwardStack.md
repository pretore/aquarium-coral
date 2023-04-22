## Forward Stack

A first-in-last-out (FILO) container backed by a forward list.

### Use

Initialise stack.

```c
struct coral_forward_stack object;
// initialise stack
seagrass_required_true(!coral_forward_stack_init(&object));
```

Invalidate stack.

```c
seagrass_required_true(!coral_forward_stack_invalidate(&object, NULL));
```

Add an item to the stack.

```c
// union type is used to inform the compiler that we intend to access pointer 
// types that point to the same item
union item {
    struct coral_forward_stack_item *item;
    uintmax_t *value;
} ptr;

// allocate memory for a new item
seagrass_required_true(!coral_forward_stack_alloc(
        sizeof(*ptr.value), &ptr.entry));
// initialise entry to a random value
*ptr.value = rand(); 
// add item to stack
seagrass_required_true(!coral_forward_stack_push(&object, ptr.entry));
```

Remove an item from the top of the stack.

```c
// union type is used to inform the compiler that we intend to access pointer 
// types that point to the same item
union item {
    struct coral_forward_stack_item *item;
    uintmax_t *value;
} ptr;

// remove an item from the top of the stack.
seahorse_required_tre(!coral_forward_queue_pop(&object, &ptr.entry));
```
