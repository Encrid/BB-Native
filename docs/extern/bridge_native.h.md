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

`BRIDGE_NATIVE_ADD_SYSTEM` and `BRIDGE_NATIVE_OBSERVER` register a system and an observer, with their terms, their changes and their order.

| Macro | Parameters |
| --- | --- |
| `BRIDGE_NATIVE_ADD_SYSTEM` | `api`, `name`, `phase`, `terms`, `count`, `changes`, `change_count`, `before`, `before_count` |
| `BRIDGE_NATIVE_OBSERVER` | `api`, `name`, `events`, `terms`, `count`, `changes`, `change_count`, `before`, `before_count` |

They fill every field of the statement.
`before` holds the slots of the systems of the module that run before this one, and `before_count` tells how many. For an observer, they are observers.
`BRIDGE_NATIVE_PUT_SLOTS(api, slots, count)` stages a list of slots in the arena. With a count of 0, it stages nothing and gives 0.
The two macros stage their changes and `before` with it.

Each macro stages its statement in the arena, then reaches the matching entry.
`BRIDGE_NATIVE_ATTACH` lends a value and attaches it by slot.
`docs/extern/bridge_native_api.h.md` describes the arena.

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_get_query` | Gives a query's spans, as the last barrier left them. |
| `bridge_native_get_span` | Gives span `i` of a query, from the private base. |
