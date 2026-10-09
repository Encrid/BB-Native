/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/host/shape.c -- a module that is not what a module is: a library
** with no init.
**
** native.md, *Lifecycle*
*/

#include "cases.h"

/*
** test_load_no_init
**
** A library that exports no init: the backend finds no such function, and
** the module is not loaded.
*/
void test_load_no_init(host_t *h)
{
    if (host_open(h))
        return;
    test_load(h, "test:noinit@0.1.0", NNOINIT_PATH, BRIDGE_NONE);
    host_close(h);
}
