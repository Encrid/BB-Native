# include/bridge_native_value.h

This header reads the value of a component of a table for 1 entity, by its id.
`include/bridge_native.h` includes it.

## The value of 1 entity

`bridge_native_find_value(api, id, slot)` does the steps of `find_value`: https://github.com/Encrid/Bridge/blob/v0.5.0/docs/extern/bridge_abi.h.md#bridge_entity_t.
It gives 0 for an id whose generation is stale, and for an entity whose table has no column of `slot`.
It also gives 0 for an id whose index is outside the entity index. Thus a module can give it an id that it read from a component.
It reads only components of a table. The readers of `docs/extern/bridge_native_kept.h.md` give the other storage.

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_find_value` | Gives where the value of a component of a table is, for 1 entity, in the shared memory, or 0. |
