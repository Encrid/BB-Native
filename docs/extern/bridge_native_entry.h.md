# include/bridge_native_entry.h

This header stages the statements of a module from a table, and tells if the changes of a module fit before the next barrier.
`include/bridge_native.h` includes it.

## A statement from a table

`bridge_native_add_entry(api, entry)` registers a system or an observer from a `bridge_native_entry_t`. It is for a module that keeps its systems in a table.
Its texts `name` and `phase` are pointers to texts that end with a zero, and it takes their bytes with `strlen`.

A `phase` of `nullptr` makes it an observer, with `events`. Each other phase makes it a system, and `events` is not read.
It stages `terms`, `changes` and `before` as `BRIDGE_NATIVE_ADD_SYSTEM` does, with `term_count`, `change_count` and `before_count`.
It gives the result of `add_system` or of `observe`.

A statement with no term stages 0 bytes, so `terms` can be `nullptr` with a `term_count` of 0.

`bridge_native_put_slots(api, slots, count)` stages a list of slots in the arena. With a count of 0, it stages nothing and gives 0. `BRIDGE_NATIVE_PUT_SLOTS` of `include/bridge_native.h` calls it.

## The room of the changes

`bridge_native_has_room(api, bytes)` tells if `changes_left` is at least `bytes`.
A module gives it the sum of `BRIDGE_CHANGE_BYTES` of each change that it writes before the next barrier. It writes them only when the result is true.
`BRIDGE_CHANGE_BYTES` is in the header `bridge_abi.h` of Bridge: https://github.com/Encrid/Bridge/blob/v0.9.0/docs/extern/bridge_abi.h.md#bridge_header_t.

## Types

| Type | Fields |
| --- | --- |
| `bridge_native_entry_t` | `name`, `phase`, `events`, `terms`, `term_count`, `changes`, `change_count`, `before`, `before_count` |

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_put_slots` | Stages the slots of `BRIDGE_NATIVE_PUT_SLOTS`, which calls it with the same parameters. |
| `bridge_native_has_room` | Tells if `changes_left` holds `bytes`, as *The room of the changes* tells. |
| `bridge_native_add_entry` | Registers a system or an observer from a `bridge_native_entry_t`, and gives the result of its entry of Bridge. |
