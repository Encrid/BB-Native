/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "native_entry.h"
#include "native_say.h"

BRIDGE_NATIVE(entry)
{
    static uint32_t calls;

    calls++;
    if (calls == 1)
        say(api, "entry ran", calls, 0);
    return (0);
}

BRIDGE_NATIVE(heard)
{
    static uint32_t calls;

    calls++;
    if (calls == 1)
        say(api, "heard ran", calls, 0);
    return (0);
}

void nentry_add(const bridge_api_t *api, uint32_t marked)
{
    bridge_term_t terms[1] = {{ marked, BRIDGE_READ, BRIDGE_AND, 0 }};
    bridge_native_entry_t entry = { .name = "entry",
        .phase = PHASE_LOST ? "nowhere" : "update" };
    bridge_native_entry_t heard = { .name = "heard", .phase = nullptr,
        .events = OBSERVE_WRONG ? BRIDGE_ON_LEAVE : BRIDGE_ON_ENTER,
        .terms = terms, .term_count = 1 };

    bridge_native_add_entry(api, &entry);
    bridge_native_add_entry(api, &heard);
}
