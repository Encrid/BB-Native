/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/modules/nalloc.c -- a module that asks for sixteen bytes of the
** persistent arena at init, and says in a system whether it was given
** them, and whether what it wrote there is still there.
*/

#include "bridge_native.h"
#include "native_idle.h"

static uint32_t kept;

/*
** say
**
** Says whether the bytes were given, and the byte written into the first
** of them at init.
*/
BRIDGE_NATIVE(say)
{
    bridge_line_t line = { "kept", 4, kept != 0, 0 };

    if (kept)
        line.a1 = api->shared_base[kept];
    bridge_native_log(api, BRIDGE_LOG_INFO, &line);
    return (0);
}

/*
** init
**
** Asks for the bytes, writes one, and declares the system.
*/
BRIDGE_NATIVE(init)
{
    int none = 0;

    kept = api->alloc(api, 16);
    if (kept)
        api->shared_base[kept] = 42;
    BRIDGE_NATIVE_ADD_SYSTEM(api, "say", "update", &none, 0, nullptr, 0);
    return (0);
}
