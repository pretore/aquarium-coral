## Array List

A list backed by an array.

### Use

Initialize a list.

```c
struct coral_array_list object;
// initialize list for uintmax_t types
seagrass_required_true(!coral_array_list_init(&object, sizeof(uintmax_t), 0));
```

Invalidate list.

```c
seagrass_required_true(!coral_array_list_invalidate(&object, NULL));
```

Add an item to the list.

```c
// add value to list
const uintmax_t value = rand();
seagrass_required_true(!coral_array_list_add(&object, &value));
```
