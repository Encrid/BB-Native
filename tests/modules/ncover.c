/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "native_idle.h"

static void cover_arena(const bridge_api_t *api)
{
    bridge_header_t *h = bridge_native_get_header(api);
    char buf[8] = {};
    uint32_t scratch = h->scratch_used;

    h->scratch_used = 0;
    bridge_native_put(api, buf, h->scratch_len + 1u);
    bridge_native_lend(api, buf, h->scratch_len + 1u);
    h->scratch_used = h->scratch_len + 1u;
    bridge_native_put(api, buf, 1u);
    bridge_native_lend(api, buf, 1u);
    bridge_native_say(api, BRIDGE_LOG_ERR, "cover");
    h->scratch_used = scratch;
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

BRIDGE_NATIVE(init)
{
    if (api->private_base) {
        cover_arena(api);
        cover_log(api);
    }
    (void)api->spawn(api);
    return (0);
}
