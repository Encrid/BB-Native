/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** src/native_abi.c -- the backend seam this backend was built
** for. It is a file of its own so that a test can link the backend with
** another answer.
*/

#include "native_inside.h"

/*
** backend_get_abi
**
** Answers the backend seam this backend was built for.
*/
BACKEND_EXPORT uint32_t backend_get_abi(void)
{
    return (BACKEND_ABI);
}
