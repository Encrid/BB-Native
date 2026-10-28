/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "game.h"
#include "native_idle.h"

static enum bridge_on_destroy get_on_destroy(uint32_t by)
{
    if (by == KEPT_BY)
        return (BRIDGE_ON_DESTROY_KEEP);
    return (BRIDGE_ON_DESTROY_REMOVE);
}

BRIDGE_NATIVE(init)
{
    BRIDGE_NATIVE_COMPONENT(api, "st:C", marked_t, BRIDGE_DELETE,
        BRIDGE_SINGLETON, get_on_destroy(1));
    BRIDGE_NATIVE_DECLARE(api, BRIDGE_STRUCT, "st:D", marked_t, BRIDGE_DELETE,
        BRIDGE_SINGLETON, get_on_destroy(2));
    BRIDGE_NATIVE_DECLARE_TAG(api, "st:T", BRIDGE_DELETE, BRIDGE_SINGLETON,
        get_on_destroy(3));
    return (0);
}
