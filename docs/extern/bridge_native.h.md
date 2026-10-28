# include/bridge_native.h

A native module compiles this header, beside `bridge_abi.h`, to reach Bridge through the backend's table.
It pulls in the table, the kept-storage readers and the log, and adds the registration macros.

## The macros

`BRIDGE_NATIVE_COMPONENT` and `BRIDGE_NATIVE_DECLARE` register a component from a type and its descriptor.
`BRIDGE_NATIVE_DECLARE_TAG` registers a tag, a component that carries no bytes.

| Macro | Parameters |
| --- | --- |
| `BRIDGE_NATIVE_DECLARE` | `api`, `kind`, `name`, `T`, `on_unload`, `storage`, `on_destroy` |
| `BRIDGE_NATIVE_COMPONENT` | `api`, `name`, `T`, `on_unload`, `storage`, `on_destroy`. The kind is `BRIDGE_STRUCT`. |
| `BRIDGE_NATIVE_DECLARE_TAG` | `api`, `name`, `on_unload`, `storage`, `on_destroy`. The kind is `BRIDGE_TAG`. |

`name` is a string literal, and `T` is a type with the descriptor `T##_desc`.
The last three parameters fill the fields of `bridge_declaration_t` with the same names.

`BRIDGE_NATIVE_ADD_SYSTEM` and `BRIDGE_NATIVE_OBSERVER` register a system and an observer, with their terms and changes.
They fill every field of the statement. `before` and `before_count` are 0, so no system must run before the system or the observer.
`BRIDGE_NATIVE_PUT_CHANGES` stages a subscriber's changed slots in the arena.

Each macro stages its statement in the arena, then reaches the matching entry.
`BRIDGE_NATIVE_ATTACH` lends a value and attaches it by slot.
`docs/extern/bridge_native_api.h.md` describes the arena.

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_get_query` | Gives a query's spans, as the last barrier left them. |
| `bridge_native_get_span` | Gives span `i` of a query, from the private base. |
