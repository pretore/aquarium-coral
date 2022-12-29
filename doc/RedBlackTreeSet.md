## Red-black Tree Set

### Overview

A set implemented via a red-black tree data structure.

### Design

The red-black tree set is implemented as a non-intrusive data structure.
Which means that it will manage its own memory and will copy passed in
items.

### Initialization

To use the red-black tree set you will need an instance of ``struct
coral_red_black_tree_set``.

```c
struct student {
    const char *name;
    size_t name_length;
    uintmax_t age;
};

struct coral_red_black_tree_set students;
seagrass_required_true(coral_red_black_tree_set_init(
        &students, sizeof(struct student), student_compare));
```

We defined a ``struct student`` type that the red black tree set will store.
Then we initialized the ``struct coral_red_black_tree_set`` with the size of
``struct student`` and provide a compare callback to order ``struct student`` 
types.

Here is an example student compare implementation:
```c
static int student_compare(const void *const a, const void *const b) {
    const struct student *const A = (const struct student *)a;
    const struct student *const B = (const struct student *)b;
    /* first compare by name length ... */
    int result = seagrass_uintmax_t_compare(A->name_length, B->name_length));
    if (result) {
        return result;
    }
    /* ... then by name ... */
    result = strncmp(A->name, B->name, A->name_length);
    if (result) {
        return result;
    }
    /* ... and finally by age. */
    seagrass_required_true(seagrass_uintmax_t_compare(
            A->age, B->age, &result));
    return result;
}
```

### Invalidation

Invalidated ``struct coral_red_black_tree_set`` instances have their contents
released. You may optionally provide an on-destroy callback to perform
cleanup on the stored types as shown below.

```c
static void student_on_destroy(void *const a) {
    struct student *const A = (struct student *)a;
    free(A->name); /* free the heap allocated name */
}

seagrass_required_true(coral_red_black_tree_set_invalidate(
        &students, student_on_destroy));
```

