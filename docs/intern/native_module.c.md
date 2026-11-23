# src/native_module.c

On this backend a module is a shared library, so its image is an opened library.
This file opens that library from a copy, lists the memories a module imports, and closes the library.

## Opening a library

`module_compile` takes the path from the source, and opens a copy of the library with `native_copy_open`: `docs/intern/native_copy.c.md`.
It refuses an empty path, and refuses a library that does not copy or that the operating system will not open.
Then it takes the image from `native_allocate`. If that gives null, it closes the library again, removes the copy, and refuses.
A library has no import section, so `module_list_imports` always names the shared and the private memory.

## Functions

| Function | Result |
| --- | --- |
| `memory_release` | Does nothing, since the handle is the block, which Bridge owns. |
| `module_compile` | Opens a copy of the library of the path. Gives the image, or null with `err` that tells why. |
| `module_list_imports` | Writes up to `max` imports and gives two: the shared memory and the private memory. |
| `module_destroy` | Closes the library, removes its copy, and frees the image. |
