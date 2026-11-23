/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"

BRIDGE_NATIVE(say)
{
    bridge_line_t line = { "build", 5, BUILD, 0 };

    bridge_native_log(api, BRIDGE_LOG_INFO, &line);
    return (0);
}

BRIDGE_NATIVE(init)
{
    int none = 0;

    BRIDGE_NATIVE_ADD_SYSTEM(api, "say", "update", &none, 0, nullptr, 0,
        nullptr, 0);
    return (0);
}

BRIDGE_NATIVE(reload)
{
    return (bridge_init(api));
}

BRIDGE_NATIVE(shutdown)
{
    return (0);
}
