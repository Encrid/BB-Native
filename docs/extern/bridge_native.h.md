# include/bridge_native.h

A native module compiles this header, beside `bridge_abi.h`, to reach Bridge through the backend's table.
It pulls in the table, the kept-storage readers, the reader of the value of 1 entity and the log, and adds the registration macros.
It also pulls in the statements from a table and the room of the changes: `docs/extern/bridge_native_entry.h.md`.

## The macros

`BRIDGE_NATIVE_COMPONENT` and `BRIDGE_NATIVE_DECLARE` register a component from a type and its descriptor.
`BRIDGE_NATIVE_DECLARE_TAG` registers a tag, a component that carries no bytes.

| Macro | Parameters |
| --- | --- |
| `BRIDGE_NATIVE_DECLARE` | `api`, `kind`, `name`, `T`, `on_unload`, `storage`, `on_destroy` |
| `BRIDGE_NATIVE_COMPONENT` | `api`, `name`, `T`, `on_unload`, `storage`, `on_destroy`. The kind is `BRIDGE_STRUCT`. |
| `BRIDGE_NATIVE_DECLARE_TAG` | `api`, `name`, `on_unload`, `storage`, `on_destroy`. The kind is `BRIDGE_TAG`. |

`T` is a type with the descriptor `T##_desc`.
The last three parameters fill the fields of `bridge_declaration_t` with the same names.

`BRIDGE_NATIVE_ADD_SYSTEM` and `BRIDGE_NATIVE_OBSERVER` register a system and an observer, with their terms, their changes and their order.

| Macro | Parameters |
| --- | --- |
| `BRIDGE_NATIVE_ADD_SYSTEM` | `api`, `name`, `phase`, `terms`, `count`, `changes`, `change_count`, `before`, `before_count` |
| `BRIDGE_NATIVE_OBSERVER` | `api`, `name`, `events`, `terms`, `count`, `changes`, `change_count`, `before`, `before_count` |

They fill every field of the statement.
`before` holds the slots of the systems of the module that run before this one, and `before_count` tells how many. For an observer, they are observers.
`BRIDGE_NATIVE_PUT_SLOTS(api, slots, count)` calls `bridge_native_put_slots`: `docs/extern/bridge_native_entry.h.md`.
The two macros stage their changes and `before` with it.

## The texts of a statement

Each text of a statement is a string literal or an array of `char`: `name`, `phase`, and the descriptor `T##_desc`.
`BRIDGE_NATIVE_TEXT_BYTES(text)` gives its bytes with no final zero, from `sizeof`.
A pointer has the size of a pointer, and not of its text. So a pointer stops the build, with the error of an array of a negative size.
The check is a `_Generic` on `&(text)`. Only an array of `char` gives a pointer to an array of the size of the text.

Each macro stages its statement in the arena, then reaches the matching entry.
`BRIDGE_NATIVE_ATTACH` lends a value and attaches it by slot.

## The value of an attach

`value` of `BRIDGE_NATIVE_ATTACH` is the component itself, and not a pointer to it.
The macro lends `sizeof(value)` bytes, and `attach` gives Bridge only their place.
Bridge reads the size of the type of the slot from that place. So a pointer lends the bytes of the pointer, and Bridge reads past them.

Neither the macro nor Bridge can test it. The macro knows the slot and not its type, and the call has no length.
An attach takes `BRIDGE_CHANGE_BYTES` of the size of its value from `changes_left`, as the header `bridge_abi.h` of Bridge gives it: https://github.com/Encrid/Bridge/blob/v0.9.0/docs/extern/bridge_abi.h.md#bridge_header_t.
`docs/extern/bridge_native_api.h.md` describes the arena.

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_get_query` | Gives a query's spans, as the last barrier left them. |
| `bridge_native_get_span` | Gives span `i` of a query, from the private base. |
| `bridge_native_get_rows` | Gives the row count of the table of a span, from the table directory in the shared memory. |
| `bridge_native_get_block` | Gives the ids of the rows of the table of a span, from the shared memory: the id of row `r` is entry `r`. |

A span holds only the id of a table. Bridge writes the row count and the block of each table once, in its table directory, at each barrier: https://github.com/Encrid/Bridge/blob/v0.9.0/docs/extern/bridge_abi.h.md#spans.
