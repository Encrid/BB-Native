# src/native_copies.c

This file takes the setting `copies` of the backend, makes its folder, and clears the copies that an earlier backend left there.

## The setting

`find_folder` reads the settings that the host gave to `backend_create`.
The only key is `copies`, and its value is a text that is not empty.
Another key, a second `copies` or another value gives a reason in `err`, and the backend refuses to start.
With no `copies`, the backend refuses too, since each module loads from a copy: `docs/intern/native_copy.c.md`.

`native_copies_take` keeps the folder in the backend, from `native_allocate`, and makes it with `os_folder_make` of mosl.

## The clear

`clear_copies` reads each entry of the folder, and removes each one whose name has the form of a copy.
`is_copy` tells that form. A folder of a generation has a name of digits only. A file of `<generation>-<file name>` is a copy of BB-Native v0.6.0.
It removes no other entry, since the host names the folder.

A file that does not go, for example a library that another process still holds, stays. No line tells it, since `backend_create` has no record but its reason.

## Functions

| Function | Result |
| --- | --- |
| `is_copy(name)` | Tells if `name` has the form of a copy. |
| `clear_copies(folder)` | Removes each copy of `folder`, as *The clear* tells. |
| `refuse(err, text, word)` | Writes the reason into `err`, and gives 1. |
| `find_folder(settings, count, folder, err)` | Sets `*folder` to the value of `copies`, and gives 0, or gives 1 with the reason. |
| `native_copies_take(b, settings, count, err)` | Keeps and makes the folder, and clears it. Gives 0, or 1 with the reason in `err`. |
