# src/native_files.c

This file holds the paths and the read of a file that the copy of a module needs.

`native_name_at` finds the file name after the last `/` or `\`. `native_folder_of` gives the folder of a path with its separator, or `.` for a bare name.
`native_join` joins a folder and a name from `native_allocate`, with `the path of a copy` as its `what`.
`native_copy_file` reads a file with `os_folder_read_file` of mosl, which shares the delete on Windows, and writes it with `os_file_write`.

## Functions

| Function | Result |
| --- | --- |
| `native_name_at(path)` | Gives the place of the file name in `path`. |
| `native_folder_of(path, err)` | Gives the folder of `path` for the caller to free. With no memory, it gives null and the reason. |
| `native_join(folder, name, err)` | Gives `<folder>/<name>` for the caller to free, or null and the reason, as `native_folder_of` does. |
| `native_copy_file(path, copy)` | Writes the file of `path` into `copy`. Gives 0, or -1. |
