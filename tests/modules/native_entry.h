/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef TEST_NATIVE_ENTRY_H
    #define TEST_NATIVE_ENTRY_H

    #include "bridge_native.h"

BRIDGE_NATIVE(entry);
BRIDGE_NATIVE(heard);
void nentry_add(const bridge_api_t *api, uint32_t marked);

#endif
