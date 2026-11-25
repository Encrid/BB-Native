/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_NATIVE_ENTRY_H
    #define BRIDGE_NATIVE_ENTRY_H

    #include <string.h>

    #include "bridge_native_api.h"

typedef struct {
    const char *name;
    const char *phase;
    uint32_t events;
    const bridge_term_t *terms;
    uint32_t term_count;
    const uint32_t *changes;
    uint32_t change_count;
    const uint32_t *before;
    uint32_t before_count;
} bridge_native_entry_t;

static inline uint32_t bridge_native_put_slots(const bridge_api_t *api,
    const uint32_t *slots, uint32_t count)
{
    if (!count)
        return (0);
    return (bridge_native_put(api, slots, count * sizeof(uint32_t)));
}

static inline bool bridge_native_has_room(const bridge_api_t *api,
    uint32_t bytes)
{
    return (bridge_native_get_header(api)->changes_left >= bytes);
}

static inline uint32_t bridge_native_add_entry(const bridge_api_t *api,
    const bridge_native_entry_t *e)
{
    uint32_t name_len = (uint32_t)strlen(e->name);
    uint32_t name = bridge_native_put(api, e->name, name_len);
    uint32_t terms = bridge_native_put(api, e->terms,
        e->term_count * (uint32_t)sizeof(bridge_term_t));
    uint32_t changes = bridge_native_put_slots(api, e->changes,
        e->change_count);
    uint32_t before = bridge_native_put_slots(api, e->before,
        e->before_count);
    uint32_t phase_len = e->phase ? (uint32_t)strlen(e->phase) : 0;

    if (!e->phase)
        return (api->observe(api, BRIDGE_NATIVE_STATE(api,
            bridge_observer_statement_t, name, name_len, e->events, terms,
            e->term_count, changes, e->change_count, before,
            e->before_count)));
    return (api->add_system(api, BRIDGE_NATIVE_STATE(api,
        bridge_system_statement_t, name, name_len,
        bridge_native_put(api, e->phase, phase_len), phase_len, terms,
        e->term_count, changes, e->change_count, before, e->before_count)));
}

#endif
