/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_NATIVE_LOG_H
    #define BRIDGE_NATIVE_LOG_H

    #include <stdarg.h>
    #include <stdio.h>

    #include "bridge_native_api.h"

typedef struct {
    const char *text;
    uint32_t len;
    uint32_t a0;
    uint32_t a1;
} bridge_line_t;

static inline bridge_log_record_t *bridge_native_take_record(
    const bridge_api_t *api, uint32_t level)
{
    bridge_header_t *h = bridge_native_get_header(api);
    bridge_log_record_t *r = (bridge_log_record_t *)(api->private_base
        + h->ring);
    uint32_t head = h->ring_head;

    if (level > h->log_level)
        return (nullptr);
    h->ring_head = head + 1u;
    if (head - h->ring_tail >= h->ring_len)
        return (nullptr);
    r += head % h->ring_len;
    r->barriers = h->barriers;
    r->place = h->place;
    r->system = h->system;
    r->observer = (uint8_t)h->observer;
    r->level = (uint8_t)level;
    return (r);
}

static inline void bridge_native_log(const bridge_api_t *api, uint32_t level,
    const bridge_line_t *line)
{
    bridge_log_record_t *r = bridge_native_take_record(api, level);

    if (!r)
        return;
    r->text = bridge_native_put(api, line->text, line->len);
    r->len = line->len;
    r->arg[0] = line->a0;
    r->arg[1] = line->a1;
}

static inline void bridge_native_say(const bridge_api_t *api, uint32_t level,
    const char *format, ...)
{
    bridge_header_t *h = bridge_native_get_header(api);
    bridge_log_record_t *r = bridge_native_take_record(api, level);
    uint32_t used = (h->scratch_used + 7u) & ~7u;
    uint32_t at = used < h->scratch_len ? used : h->scratch_len;
    uint32_t room = h->scratch_len - at;
    uint32_t most = room ? room - 1u : 0;
    va_list values;
    int full;

    if (!r)
        return;
    va_start(values, format);
    full = vsnprintf((char *)api->private_base + h->scratch + at, room,
        format, values);
    va_end(values);
    r->text = h->scratch + at;
    r->arg[0] = full < 0 ? 0 : (uint32_t)full;
    r->arg[1] = 0;
    r->len = r->arg[0] < most ? r->arg[0] : most;
    h->scratch_used = at + r->len;
}

#endif
