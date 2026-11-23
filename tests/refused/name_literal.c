/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "bridge_native.h"

static const char NAME[] = "found_window";

uint32_t add_named(const bridge_api_t *api)
{
    return (BRIDGE_NATIVE_ADD_SYSTEM(api, "found_request", "first", nullptr,
        0, nullptr, 0, nullptr, 0) + BRIDGE_NATIVE_ADD_SYSTEM(api, NAME,
        "first", nullptr, 0, nullptr, 0, nullptr, 0));
}
