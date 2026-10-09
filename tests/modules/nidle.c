/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/modules/nidle.c -- a shutdown and a reload that do nothing,
** built into every native test module that includes native_idle.h.
*/

#include "native_idle.h"

/*
** shutdown
**
** Does nothing.
*/
BRIDGE_NATIVE(shutdown)
{
    return (0);
}

/*
** reload
**
** Does nothing.
*/
BRIDGE_NATIVE(reload)
{
    return (0);
}
