/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"

uint32_t add_named(const bridge_api_t *api, const char *name)
{
    return (BRIDGE_NATIVE_ADD_SYSTEM(api, name, "first", nullptr, 0, nullptr,
        0, nullptr, 0));
}
