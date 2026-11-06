/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "game.h"
#include "native_idle.h"
#include "native_say.h"

static uint32_t position;
static uint32_t marked;
static uint32_t runs;
static uint64_t live;
static uint64_t bare;
static uint64_t stale;

static uint32_t value_of(const bridge_api_t *api, uint64_t id)
{
    uint32_t at = bridge_native_find_value(api, id, marked);

    if (!at)
        return (0);
    return (((const marked_t *)(api->shared_base + at))->by);
}

BRIDGE_NATIVE(find)
{
    runs++;
    if (runs != 1)
        return (0);
    say(api, "find live", value_of(api, live), 0);
    say(api, "find missing", value_of(api, bare), 0);
    say(api, "find stale", value_of(api, stale), 0);
    say(api, "find far", value_of(api, NEAR ? live : UINT32_MAX), 0);
    return (0);
}

static uint64_t spawn_marked(const bridge_api_t *api, bool with_mark)
{
    position_t p = { 1.0f, 2.0f };
    marked_t m = { LIVE };
    uint64_t id = api->spawn(api);

    BRIDGE_NATIVE_ATTACH(api, id, position, p);
    if (with_mark)
        BRIDGE_NATIVE_ATTACH(api, id, marked, m);
    return (id);
}

BRIDGE_NATIVE(init)
{
    int none = 0;

    position = BRIDGE_NATIVE_COMPONENT(api, "game:Position", position_t,
        BRIDGE_DELETE, BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    marked = BRIDGE_NATIVE_COMPONENT(api, "game:Marked", marked_t,
        BRIDGE_STRIP, BRIDGE_TABLE, BRIDGE_ON_DESTROY_REMOVE);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "find", "update", &none, 0, nullptr, 0,
        nullptr, 0);
    live = spawn_marked(api, true);
    bare = spawn_marked(api, MISSING);
    stale = spawn_marked(api, true);
    if (!KEPT)
        api->destroy(api, stale);
    return (0);
}
