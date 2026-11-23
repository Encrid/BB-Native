/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "native_idle.h"

int nhelper_value(void);

BRIDGE_NATIVE(say)
{
    bridge_line_t line = { "helped", 6, (uint32_t)nhelper_value(), 0 };

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
