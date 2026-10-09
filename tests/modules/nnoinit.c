/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/modules/nnoinit.c -- a module with no init: what it exports is
** called something else.
*/

#include "bridge_native.h"

/*
** start
**
** Answers 0. Nothing asks for it.
*/
BRIDGE_NATIVE(start)
{
    return (0);
}
