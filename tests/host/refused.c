/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/host/refused.c -- what this backend refuses, and what is refused
** of it: a module it cannot keep the limits of, a module with no path or
** with a library that cannot be opened, and a setting.
**
** native.md, *What a native module must not do*
*/

#include "cases.h"

static const bridge_setting_t TEST_SETTING[1] = {
    { "cache", BRIDGE_STRING, {}, { .s = "somewhere" } }
};

/*
** test_state_untrusted
**
** A module that is not trusted, on a backend that runs only modules that
** are: refused before any library is looked for.
*/
void test_state_untrusted(host_t *h)
{
    bridge_artifact_t a;

    if (host_open(h))
        return;
    host_state_native(&a, "game:mover@0.1.0", "no-such-library");
    a.cfg.trusted = BRIDGE_FALSE;
    host_load(h, &a);
    host_close(h);
}

/*
** test_state_limit
**
** A work limit, a memory limit or a timeout, as the case's row says, on a
** backend that keeps none.
*/
void test_state_limit(host_t *h)
{
    bridge_artifact_t a;

    if (host_open(h))
        return;
    host_state_native(&a, "game:mover@0.1.0", "no-such-library");
    if (*h->arg == 'w')
        a.cfg.work_limit = 1000;
    if (*h->arg == 'm')
        a.cfg.memory_limit = 1 << 20;
    if (*h->arg == 't')
        a.cfg.shutdown_timeout = 2000 * HOST_MS;
    host_load(h, &a);
    host_close(h);
}

/*
** test_state_no_path
**
** A module, and nowhere to find it.
*/
void test_state_no_path(host_t *h)
{
    bridge_artifact_t a;

    if (host_open(h))
        return;
    host_state_native(&a, "game:mover@0.1.0", nullptr);
    host_load(h, &a);
    host_close(h);
}

/*
** test_name_no_library
**
** A module whose library cannot be opened.
*/
void test_name_no_library(host_t *h)
{
    bridge_artifact_t a;

    if (host_open(h))
        return;
    host_state_native(&a, "game:mover@0.1.0", "no-such-library");
    host_load(h, &a);
    host_close(h);
}

/*
** test_hand_setting
**
** A setting, handed to a backend that has none: the deck is not created.
*/
void test_hand_setting(host_t *h)
{
    h->backends[0].settings = TEST_SETTING;
    h->backends[0].setting_count = 1;
    host_open_only(h);
}
