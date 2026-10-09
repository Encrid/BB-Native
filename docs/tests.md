# The tests

What BB-Native exposes, and the case for each. A case is a process: `python build.py --bridge DIR
test -k NAME` runs those whose name has NAME in it. A case is named for what it is of, a
hyphen, and what it shows, and is listed under that heading. `tools/cover.py` reads the
headers of the SDK, this document and the cases of `build.py`, and fails on an exposed name
with no heading, a heading with no case, and a case under no heading or the wrong one.

## The SDK

What a module is written with: `bridge_native.h`, and what it includes.

### BRIDGE_NATIVE_ATTACH

    BRIDGE_NATIVE_ATTACH-seen-by-an-observer

### BRIDGE_NATIVE_DECLARE

    BRIDGE_NATIVE_DECLARE-an-enum-a-struct-names

### BRIDGE_NATIVE_COMPONENT

    BRIDGE_NATIVE_COMPONENT-declared

### BRIDGE_NATIVE_DECLARE_TAG

    BRIDGE_NATIVE_DECLARE_TAG-carried-with-no-bytes

### BRIDGE_NATIVE_ADD_SYSTEM

    BRIDGE_NATIVE_ADD_SYSTEM-registered-and-run

### BRIDGE_NATIVE_OBSERVER

    BRIDGE_NATIVE_OBSERVER-registered-and-told

### BRIDGE_NATIVE_PUT_CHANGES

    BRIDGE_NATIVE_PUT_CHANGES-changes-stated-and-made

### bridge_native_get_query

    bridge_native_get_query-the-query-of-a-system

### bridge_native_get_span

    bridge_native_get_span-two-tables-walked

### declare

    declare-two-components-declared

### add_system

    add_system-registered-and-run

### observe

    observe-told-of-an-attach

### spawn

    spawn-a-thousand-made-at-init

### destroy

    destroy-two-entities-gone

### attach

    attach-a-value-given-again

### detach

    detach-a-component-taken

### alloc

    alloc-bytes-kept-from-init

### bridge_api_t

    bridge_api_t-entry-from-a-foreign-thread-refused

### BRIDGE_NATIVE

    BRIDGE_NATIVE-system-reads-and-writes

### BRIDGE_NATIVE_STATE

    BRIDGE_NATIVE_STATE-a-statement-put-in-scratch

### bridge_native_get_header

    bridge_native_get_header-read-through-a-pointer

### bridge_native_put

    bridge_native_put-bytes-copied-into-scratch

### bridge_native_lend

    bridge_native_lend-a-value-lent-to-attach

### bridge_native_get_single

    bridge_native_get_single-the-one-row

### bridge_native_get_cells

    bridge_native_get_cells-reached-by-an-id

### bridge_native_get_sparse_index

    bridge_native_get_sparse_index-reached-by-an-id

### bridge_native_get_bits

    bridge_native_get_bits-read-and-turned

### bridge_native_get_list

    bridge_native_get_list-triggers-of-an-observer

### bridge_line_t

    bridge_line_t-a-line-with-its-two-numbers

### bridge_native_log

    bridge_native_log-a-line-with-two-numbers

## The backend

The entries of Bridge's seam for a backend, as this one answers them.

### call

    call-an-entry-refused-fails-the-invocation

### backend_get_capabilities

    backend_get_capabilities-untrusted-refused
    backend_get_capabilities-work-limit-refused
    backend_get_capabilities-memory-limit-refused
    backend_get_capabilities-timeout-refused

### module_compile

    module_compile-no-path-refused
    module_compile-library-cannot-open

### backend_create

    backend_create-a-setting-refused

### define_func

    define_func-an-entry-the-abi-has-not-refused

### instance_get_func

    instance_get_func-no-such-export-refused

## The tree

Each tool that holds the tree, as a case.

### c23

    c23

### style

    style

### deps

    deps

### said

    said

### cover

    cover

### xref

    xref

### licence

    licence

### package

    package

### pin

    pin-another-archive-refused

### cla

    cla-the-holder-is-not-asked
    cla-a-name-on-record-passes
    cla-anybody-else-is-asked-and-fails
    cla-the-sentence-by-its-author-is-recorded
    cla-the-sentence-by-another-is-not-taken
    cla-other-words-are-not-taken
