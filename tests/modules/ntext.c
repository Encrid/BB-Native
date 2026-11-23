/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "native_idle.h"

static uint32_t probe(const bridge_api_t *api, uint32_t at, uint32_t len,
    uint32_t bit)
{
    bridge_text_t text = { at, len + WIDER };

    return (bridge_native_in_arena(api, text) ? 1u << bit : 0u);
}

BRIDGE_NATIVE(init)
{
    const bridge_header_t *h = bridge_native_get_header(api);
    uint32_t kept = api->alloc(api, 16);
    uint32_t inside = probe(api, kept, 16, 0) | probe(api, h->arena_lo - 1u,
        1, 1) | probe(api, h->arena_hi + 1u, 0, 2);
    bridge_line_t line = { "in-arena", 8, 0, 0 };

    inside |= probe(api, h->arena_hi, 0, 3)
        | probe(api, kept, h->arena_hi - kept + 1u, 4);
    line.a0 = inside;
    line.a1 = kept != 0;
    bridge_native_log(api, BRIDGE_LOG_INFO, &line);
    return (0);
}
