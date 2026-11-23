# src/native_copy.c

This file writes the copy of a module, opens the library from the copy, and removes the copy.

## The copy

A reload makes the new image while the old one is still open. The document of `bridge_backend.h` of Bridge gives that order of the calls.
An open of the same path then gives the library that is open already:

- On Windows, `LoadLibraryW` gives the same module for the same path, also after the file was moved away and written again.
- On Linux, `dlopen(3)`: "If the same shared object is opened again with dlopen(), the same object handle is returned."
- On macOS, `dlclose(3)` names cases in which "a dynamic library will never be unloaded".

Also on Windows, no program can write, remove or replace a library that is open.
Thus each image opens a copy at a new path, and the file of the host is never open.

`copy_path` gives that path: `<copies>/<generation>-<file name>`. The generation is a count of the backend, 1 more for each copy.
`name_at` finds the file name after the last `/` or `\`.

`native_copy_open` writes the bytes of the source into the copy with `os_file_write` of mosl, if the host gave them.
Else `write_from_path` reads the file of `path` with `os_folder_read_file` of mosl, and writes it.
Then `os_lib_open` opens the copy.
If the read, the write or the open fails, it removes the copy, and writes `cannot open the library '<path>'`, with the path of the host.

`native_copy_remove` removes the copy with `os_path_remove`, and frees its path. `module_destroy` calls it after it closed the library.
A copy that does not go stays, and the next `backend_create` with that folder removes it: `docs/intern/native_copies.c.md`, *The clear*.

## Functions

| Function | Result |
| --- | --- |
| `name_at(path)` | Gives the place of the file name in `path`. |
| `copy_path(b, path, err)` | Gives the path of the next copy, which the caller frees, or null with the reason in `err`. |
| `write_from_path(path, copy)` | Writes the file of `path` into `copy`. Gives 0, or -1. |
| `native_copy_open(b, source, copy, err)` | Writes the copy and opens its library, as *The copy* tells. Sets `*copy`, and gives the library, or null with the reason in `err`. |
| `native_copy_remove(copy)` | Removes the copy and frees its path. A null `copy` does nothing. |
