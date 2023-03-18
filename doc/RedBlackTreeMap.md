## Red-black Tree Map

### Overview

A map implemented via a red-black tree data structure.

### Design

The red-black tree map is implemented as a non-intrusive data structure.
Which means that it will manage its own memory and will copy passed in
keys and values.

### Initialization

To use the red-black tree set you will need an instance of ``struct
coral_red_black_tree_map``.

```c
struct student_key {
    const char *name;
    size_t length;
};

struct student_value {
    uintmax_t age;
};

struct coral_red_black_tree_map students;
seagrass_required_true(!coral_red_black_tree_map_init(
        &students,
        sizeof(struct student_key), 
        sizeof(struct student_value),
        student_compare));
```

We defined a ``struct student_key`` type that the red black tree map
will use to look up the associated ``struct student_value``.
Then we initialized the ``struct coral_red_black_tree_map`` with the size of
``struct student_key``, size of ``struct student_value`` and provide a compare 
callback to order ``struct student_key`` types.

Here is an example student key compare implementation:
```c
static int student_key_compare(const void *const a, const void *const b) {
    const struct student_key *const A = (const struct student_key *)a;
    const struct student_key *const B = (const struct student_key *)b;
    /* first compare by length ... */
    int result = seagrass_uintmax_t_compare(A->length, B->length));
    if (result) {
        return result;
    }
    /* ... then by name */
    return strncmp(A->name, B->name, A->name_length);
}
```

### Invalidation

Invalidated ``struct coral_red_black_tree_map`` instances have their contents
released. You may optionally provide an on-destroy callback to perform
cleanup on the stored types as shown below.

```c
static void student_on_destroy(void *const key, void *const value) {
    struct student_key *const A = (struct student_key *)a;
    free(A->name); /* free the heap allocated name */
}

seagrass_required_true(!coral_red_black_tree_map_invalidate(
        &students, student_on_destroy));
```

