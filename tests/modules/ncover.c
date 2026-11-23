/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "native_idle.h"

static uint32_t probe(const bridge_api_t *api, uint32_t used, uint32_t len,
    bool lend)
{
    bridge_header_t *h = bridge_native_get_header(api);
    char buf[8] = {};

    h->scratch_used = used;
    if (lend)
        return (bridge_native_lend(api, buf, len) != 0);
    return (bridge_native_put(api, buf, len) != 0);
}

static void cover_arena(const bridge_api_t *api)
{
    bridge_header_t *h = bridge_native_get_header(api);
    uint32_t scratch = h->scratch_used;
    uint32_t big = FIT ? 1u : h->scratch_len + 1u;
    uint32_t full = FIT ? 0u : h->scratch_len + 1u;
    uint32_t slot = 0;
    bridge_line_t line = { "arena-edges", 11, 0, 0 };

    line.a0 = probe(api, 0, big, false) | probe(api, 0, big, true) << 1
        | probe(api, full, 1u, false) << 2 | probe(api, full, 1u, true) << 3;
    h->scratch_used = 0;
    line.a1 = bridge_native_put_slots(api, &slot, FIT) != 0;
    h->scratch_used = h->scratch_len + 1u;
    bridge_native_say(api, BRIDGE_LOG_ERR, "cover");
    h->scratch_used = scratch;
    bridge_native_log(api, BRIDGE_LOG_INFO, &line);
}

static void cover_log(const bridge_api_t *api)
{
    bridge_header_t *h = bridge_native_get_header(api);
    bridge_line_t line = { "cover", 5, 0, 0 };
    uint32_t head = h->ring_head;

    bridge_native_log(api, h->log_level + 1u, &line);
    h->ring_head = h->ring_tail + h->ring_len;
    bridge_native_log(api, BRIDGE_LOG_ERR, &line);
    h->ring_head = head;
}

static const bridge_api_t *first;

BRIDGE_NATIVE(init)
{
    uint64_t a = 0;

    if (api->private_base) {
        cover_arena(api);
        cover_log(api);
    }
    a = api->spawn(api);
    if (api->private_base)
        return (0);
    first = first ? first : api;
    return ((int)(a * 16u + api->spawn(api)));
}

BRIDGE_NATIVE(late)
{
    return (first && first != api ? (int)first->spawn(first) : -1);
}
