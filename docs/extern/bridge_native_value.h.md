# include/bridge_native_value.h

This header reads the value of a component of a table for 1 entity, by its id.
It also tells if a text is in the arena that a module can read.
`include/bridge_native.h` includes it.

## The value of 1 entity

`bridge_native_find_value(api, id, slot)` does the steps of `find_value`: https://github.com/Encrid/Bridge/blob/v0.9.0/docs/extern/bridge_abi.h.md#bridge_entity_t.
It gives 0 for an id whose generation is stale, and for an entity whose table has no column of `slot`.
It also gives 0 for an id whose index is outside the entity index. Thus a module can give it an id that it read from a component.

For a tag, it gives `BRIDGE_CARRIED`, as each reader of a place gives for a tag: https://github.com/Encrid/Bridge/blob/v0.9.0/docs/extern/bridge_abi.h.md#bridge_kind. A tag has no value, so a caller compares the place with `BRIDGE_CARRIED` before it reads.
It reads only components of a table. The readers of `docs/extern/bridge_native_kept.h.md` give the other storage.

## An entity that is alive

`bridge_native_is_alive(api, id)` tells if an id names a live entity. It does the first steps of `find_value`. The index of the id is inside the entity index, and its generation is the generation of that entry.
It gives false for an id whose generation is stale, and for an id whose index is outside the entity index. `bridge_native_find_value` tests the id with it.

## A text in the arena

`bridge_native_in_arena(api, text)` tells if the bytes of a `bridge_text_t` are all in the shared arena that the module can read: https://github.com/Encrid/Bridge/blob/v0.9.0/docs/extern/bridge_abi.h.md#bridge_header_t.
It gives true when `at` is at least `arena_lo` and at most `arena_hi`, and `len` is at most `arena_hi` less `at`. So a text of 0 bytes at `arena_hi` is in it.
It gives false for each other text, also when `at` plus `len` would wrap. Thus a module can test a text that it read from a component before it copies the bytes.

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_find_value` | Gives where the value of a component of a table is, for 1 entity, in the shared memory, or 0. |
| `bridge_native_is_alive` | Tells if an id names a live entity, as *An entity that is alive* tells. |
| `bridge_native_in_arena` | Tells if a text is in the shared arena that the module can read, as *A text in the arena* tells. |
