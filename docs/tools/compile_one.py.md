# tools/compile_one.py

This tool checks 1 source in C with the zig of the toolchain. It gives the errors and the status of the compiler.
A case claims with it that a source does not compile, as a refusal at compile time asks.

## Usage

`python tools/compile_one.py <source> [<include folder>...]`

It runs `zig cc -std=c23 -c` on the source, with `-I` for each include folder.
The object goes to a folder of its own, which the tool deletes after. So the tool leaves no file.
It prints what the compiler writes, and exits with the status of the compiler.
It finds zig as the toolchain does, with `zig_found` of `.toolchain/tools/fetch.py` and the table `[zig]` of `.toolchain/toolchain.toml`.

## The cases

`tests/refused/name_pointer.c` gives a pointer as the name of a system. It must not compile: `docs/extern/bridge_native.h.md`, *The texts of a statement*.
`tests/refused/name_literal.c` gives a string literal and an array of `char`, and compiles. It is the false variant.

## Messages

| Line | Cause |
| --- | --- |
| `usage: python tools/compile_one.py <source> [<include folder>...]` | No source. |
| `compile_one: zig <version> is neither fetched nor on the path` | No zig of the version of the toolchain. |
