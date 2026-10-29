/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"
#include "native_idle.h"

BRIDGE_NATIVE(init)
{
    int none = 0;

    BRIDGE_NATIVE_ADD_SYSTEM(api, "values", "lines", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "two", "lines", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "unkept", "lines", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "wrong", "lines", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "cut", "cut", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "cut_odd", "cut_odd", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "no_room", "no_room", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "full", "full", &none, 0, nullptr, 0,
        nullptr, 0);
    BRIDGE_NATIVE_ADD_SYSTEM(api, "after_full", "after_full", &none, 0,
        nullptr, 0, nullptr, 0);
    return (0);
}
