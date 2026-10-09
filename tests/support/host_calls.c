/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/support/host_calls.c -- the call a host makes to load a module,
** said as it ended.
*/

#include <stdio.h>

#include "host.h"

/*
** host_load
**
** Loads a module and says how it ended.
*/
void host_load(host_t *h, const bridge_artifact_t *a)
{
    const char *ended = bridge_load(h->b, a) ? "not loaded" : "loaded";

    printf("%s: %s\n", a->cfg.identity, ended);
}

