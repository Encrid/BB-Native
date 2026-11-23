# src/native_siblings.c

This file gives the folder of a copy the other files of the folder of the source.

## The files beside

`native_give_beside` reads each entry of the folder of the source. For each file but the module itself, `give_one` makes a hard link in the folder of the copy with `os_file_link` of mosl.
A hard link needs no right of an administrator. A move onto the file of the source gives the source a new file, and the link keeps the old bytes.
So a tool replaces a library beside a module by a move, while the module is loaded. A write in place would change the bytes of the loaded copy.

If the system refuses the link, for example on 2 volumes, `give_one` copies the file only if `is_library` tells a library of the system.
Thus the assets of a folder, which can be large, are never copied.
`is_library` takes a name that ends with `.dll` on Windows, `.dylib` on macOS, or `.so` on Linux. On Linux it also takes a name that holds `.so.`, for a versioned name.
A subfolder and a file that cannot be linked or copied are skipped. The load then tells if the module needs one of them.

## Functions

| Function | Result |
| --- | --- |
| `is_library(name)` | Tells if `name` is a library of the system, as *The files beside* tells. |
| `give_one(from, folder, name)` | Links, or copies, the file `name` of `from` into `folder`. |
| `native_give_beside(from, skip, folder)` | Gives `folder` each file of `from` but `skip`. |
