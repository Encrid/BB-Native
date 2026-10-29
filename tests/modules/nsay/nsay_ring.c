/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"

static uint32_t grew;

BRIDGE_NATIVE(full)
{
    const bridge_header_t *h = bridge_native_get_header(api);
    uint32_t i = 0;
    uint32_t before;

    while (i < h->ring_len) {
        bridge_native_say(api, BRIDGE_LOG_INFO, "line %u", i);
        i++;
    }
    before = h->scratch_used;
    bridge_native_say(api, BRIDGE_LOG_INFO, "one line too many");
    grew = h->scratch_used - before;
    return (0);
}

BRIDGE_NATIVE(after_full)
{
    bridge_native_say(api, BRIDGE_LOG_INFO,
        "the dropped line grew the arena by %u", grew);
    return (0);
}
