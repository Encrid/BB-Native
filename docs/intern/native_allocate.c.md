# src/native_allocate.c

This file takes the memory of each handle that the backend allocates, and writes why when it gets none.

## No memory

`native_allocate` takes zeroed bytes from `calloc`.
If `calloc` gives null, it writes the reason in `err`, with `what` in the text, and gives null.
`backend_create`, `module_compile`, `instance_create` and `instance_get_func` take the memory of their handle through `native_allocate`.
Each of them gives null when `native_allocate` gives null.
`readme.md`, *Messages*, tells the `what` of each entry, and the record that Bridge writes.

## Functions

| Function | Result |
| --- | --- |
| `native_allocate` | Gives `size` zeroed bytes, or null with the reason in `err`. |
