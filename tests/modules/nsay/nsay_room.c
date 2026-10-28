/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"

static void say_with_room(const bridge_api_t *api, uint32_t room)
{
    bridge_header_t *h = bridge_native_get_header(api);

    h->scratch_used = h->scratch_len - room;
    bridge_native_say(api, BRIDGE_LOG_INFO, "%s", "0123456789");
}

BRIDGE_NATIVE(cut)
{
    say_with_room(api, 8);
    return (0);
}

BRIDGE_NATIVE(cut_odd)
{
    say_with_room(api, 11);
    return (0);
}

BRIDGE_NATIVE(no_room)
{
    say_with_room(api, 0);
    return (0);
}
