# src/native_copy.c

This file makes the folder of a copy of a module, opens the library from the copy, and removes the folder.

## The copy

A reload makes the new image while the old one is still open. The document of `bridge_backend.h` of Bridge gives that order of the calls.
An open of the same path then gives the library that is open already:

- On Windows, `LoadLibraryW` gives the same module for the same path, also after the file was moved away and written again.
- On Linux, `dlopen(3)`: "If the same shared object is opened again with dlopen(), the same object handle is returned."
- On macOS, `dlclose(3)` names cases in which "a dynamic library will never be unloaded".

Also on Windows, no program can write, remove or replace a library that is open.
Thus each image opens a copy at a new path, and the file of the host is never open.

## The folder of a generation

`make_generation` makes the folder `<copies>/<generation>`. The generation is a count of the backend, 1 more for each image.
If the folder cannot be made, it refuses with the path of the folder: `readme.md`, *Messages*.
The copy goes into it under the name of the file of the source.
`native_give_beside` links the rest of that folder beside it: `docs/intern/native_siblings.c.md`.
Thus the search of each system finds what the module needs beside the copy, as beside the source:

- On Windows, `os_lib_open` of mosl looks first in the folder of the library that a path names.
- On Linux, `$ORIGIN` of a run path "expands to the directory containing the program or shared object": `ld.so(8)`.
- On macOS, `@loader_path` is "the path to the directory containing the mach-o binary which contains the load command": `dyld(1)`.

`native_copy_open` writes the bytes of the source into the copy with `os_file_write` of mosl, if the host gave them.
Else `native_copy_file` of `src/native_files.c` reads the file of the source, and writes it.
Then `os_lib_open` opens the copy.
If the write or the open fails, it removes the folder, and writes `cannot open the library '<path>'`, with the path of the host.

`native_copy_remove` removes the folder of a generation with `os_path_remove`, and frees its path. `module_destroy` calls it after it closed the library.
A folder that does not go stays, and the next `backend_create` with that folder removes it: `docs/intern/native_copies.c.md`, *The clear*.

## Functions

| Function | Result |
| --- | --- |
| `make_generation(b, err)` | Makes the folder of the next generation. Gives its path for the caller to free. With no folder, it gives null and the reason. |
| `write_copy(source, path)` | Writes the bytes of the source, or its file, into `path`. Gives 0, or -1. |
| `native_copy_open(b, source, copy, err)` | Makes the folder, writes the copy and opens its library, as *The folder of a generation* tells. Sets `*copy` to the folder, and gives the library, or null with the reason in `err`. |
| `native_copy_remove(copy)` | Removes the folder of a generation and frees its path. |
