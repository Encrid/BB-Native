/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_NATIVE_H
    #define BRIDGE_NATIVE_H

    #include "bridge_native_api.h"
    #include "bridge_native_entry.h"
    #include "bridge_native_kept.h"
    #include "bridge_native_log.h"
    #include "bridge_native_value.h"

    #define BRIDGE_NATIVE_TEXT_BYTES(text) \
        (sizeof(char [_Generic(&(text), char (*)[sizeof(text)]: 1, \
            const char (*)[sizeof(text)]: 1, default: -1)]) * 0 \
            + sizeof(text) - 1)

    #define BRIDGE_NATIVE_ATTACH(api, id, slot, value) \
        bridge_native_attach(api, id, slot, \
            (bridge_native_bytes_t){ &(value), sizeof(value) })

    #define BRIDGE_NATIVE_DECLARE(api, kind, name, T, on_unload, storage, \
        on_destroy) \
        (api)->declare(api, BRIDGE_NATIVE_STATE(api, bridge_declaration_t, \
            bridge_native_put(api, name, BRIDGE_NATIVE_TEXT_BYTES(name)), \
            BRIDGE_NATIVE_TEXT_BYTES(name), \
            kind, sizeof(T), alignof(T), \
            bridge_native_put(api, T##_desc, \
                BRIDGE_NATIVE_TEXT_BYTES(T##_desc)), \
            BRIDGE_NATIVE_TEXT_BYTES(T##_desc), on_unload, storage, on_destroy))
    #define BRIDGE_NATIVE_COMPONENT(api, name, T, on_unload, storage, \
        on_destroy) \
        BRIDGE_NATIVE_DECLARE(api, BRIDGE_STRUCT, name, T, on_unload, \
            storage, on_destroy)

    #define BRIDGE_NATIVE_DECLARE_TAG(api, name, on_unload, storage, \
        on_destroy) \
        (api)->declare(api, BRIDGE_NATIVE_STATE(api, bridge_declaration_t, \
            bridge_native_put(api, name, BRIDGE_NATIVE_TEXT_BYTES(name)), \
            BRIDGE_NATIVE_TEXT_BYTES(name), \
            BRIDGE_TAG, 0, 1, 0, 0, on_unload, storage, on_destroy))

    #define BRIDGE_NATIVE_ADD_SYSTEM(api, name, phase, terms, count, changes, \
        change_count, before, before_count) \
        (api)->add_system(api, BRIDGE_NATIVE_STATE(api, \
            bridge_system_statement_t, \
            bridge_native_put(api, name, BRIDGE_NATIVE_TEXT_BYTES(name)), \
            BRIDGE_NATIVE_TEXT_BYTES(name), \
            bridge_native_put(api, phase, BRIDGE_NATIVE_TEXT_BYTES(phase)), \
            BRIDGE_NATIVE_TEXT_BYTES(phase), \
            bridge_native_put(api, terms, (count) * sizeof(bridge_term_t)), \
            count, BRIDGE_NATIVE_PUT_SLOTS(api, changes, change_count), \
            change_count, BRIDGE_NATIVE_PUT_SLOTS(api, before, before_count), \
            before_count))
    #define BRIDGE_NATIVE_OBSERVER(api, name, events, terms, count, changes, \
        change_count, before, before_count) \
        (api)->observe(api, BRIDGE_NATIVE_STATE(api, \
            bridge_observer_statement_t, \
            bridge_native_put(api, name, BRIDGE_NATIVE_TEXT_BYTES(name)), \
            BRIDGE_NATIVE_TEXT_BYTES(name), \
            events, \
            bridge_native_put(api, terms, (count) * sizeof(bridge_term_t)), \
            count, BRIDGE_NATIVE_PUT_SLOTS(api, changes, change_count), \
            change_count, BRIDGE_NATIVE_PUT_SLOTS(api, before, before_count), \
            before_count))
    #define BRIDGE_NATIVE_PUT_SLOTS(api, slots, count) \
        bridge_native_put_slots(api, slots, count)

static inline const bridge_query_t *bridge_native_get_query(
    const bridge_api_t *api, uint32_t query)
{
    const uint8_t *queries = api->private_base
        + bridge_native_get_header(api)->queries;

    return ((const bridge_query_t *)queries + query);
}

static inline const uint32_t *bridge_native_get_span(const bridge_api_t *api,
    const bridge_query_t *q, uint32_t i)
{
    return ((const uint32_t *)(api->private_base + q->at
        + i * BRIDGE_SPAN_SIZE));
}

static inline uint32_t bridge_native_get_rows(const bridge_api_t *api,
    const uint32_t *span)
{
    const uint8_t *rows = api->shared_base
        + bridge_native_get_header(api)->rows;

    return (((const uint32_t *)rows)[span[0]]);
}

static inline const uint64_t *bridge_native_get_block(const bridge_api_t *api,
    const uint32_t *span)
{
    const uint8_t *blocks = api->shared_base
        + bridge_native_get_header(api)->blocks;

    return ((const uint64_t *)(api->shared_base
        + ((const uint32_t *)blocks)[span[0]]));
}

#endif
