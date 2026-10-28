/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <wchar.h>

#include "bridge_native.h"

BRIDGE_NATIVE(values)
{
    bridge_native_say(api, BRIDGE_LOG_WARN, "%s has %u rows and %d left",
        "look", 6u, -2);
    return (0);
}

BRIDGE_NATIVE(two)
{
    bridge_native_say(api, BRIDGE_LOG_INFO, "first %u", 1u);
    bridge_native_say(api, BRIDGE_LOG_INFO, "second %u", 2u);
    return (0);
}

BRIDGE_NATIVE(unkept)
{
    const bridge_header_t *h = bridge_native_get_header(api);
    uint32_t before = h->scratch_used;

    bridge_native_say(api, BRIDGE_LOG_DEBUG, "a detail %u", 7u);
    bridge_native_say(api, BRIDGE_LOG_INFO, "the arena grew by %u",
        h->scratch_used - before);
    return (0);
}

BRIDGE_NATIVE(wrong)
{
    bridge_native_say(api, BRIDGE_LOG_INFO, "%lc", (wint_t)0xD800);
    return (0);
}
