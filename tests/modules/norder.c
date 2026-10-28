/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "game.h"
#include "native_idle.h"

static uint32_t marked;
static uint64_t one;

BRIDGE_NATIVE(first)
{
    marked_t m = { 1 };

    BRIDGE_NATIVE_ATTACH(api, one, marked, m);
    bridge_native_say(api, BRIDGE_LOG_INFO, "ran");
    return (0);
}

BRIDGE_NATIVE(second)
{
    bridge_native_say(api, BRIDGE_LOG_INFO, "ran");
    return (0);
}

BRIDGE_NATIVE(heard_first)
{
    bridge_native_say(api, BRIDGE_LOG_INFO, "heard");
    return (0);
}

BRIDGE_NATIVE(heard_second)
{
    bridge_native_say(api, BRIDGE_LOG_INFO, "heard");
    return (0);
}

BRIDGE_NATIVE(init)
{
    bridge_term_t terms[1] = {{ 0, BRIDGE_WRITE, BRIDGE_AND, 0 }};
    uint32_t first;
    uint32_t heard_first;

    marked = BRIDGE_NATIVE_COMPONENT(api, "game:Marked", marked_t,
        BRIDGE_STRIP, BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    terms[0].slot = marked;
    first = BRIDGE_NATIVE_ADD_SYSTEM(api, "first", "update", terms, 1,
        &marked, 1, nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "second", "update", terms, 1, nullptr, 0,
        &first, SYSTEMS_ORDERED);
    heard_first = BRIDGE_NATIVE_OBSERVER(api, "heard_first", BRIDGE_ON_ENTER,
        terms, 1, nullptr, 0, nullptr, 0);
    BRIDGE_NATIVE_OBSERVER(api, "heard_second", BRIDGE_ON_ENTER, terms, 1,
        nullptr, 0, &heard_first, OBSERVERS_ORDERED);
    one = api->spawn(api);
    return (0);
}
