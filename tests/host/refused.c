/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "cases.h"

static const bridge_setting_t TEST_SETTING[1] = {
    { "cache", BRIDGE_STRING, {}, { .s = "somewhere" } }
};

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
    if (*h->arg == 'e')
        a.cfg.element_limit = 1 << 16;
    host_load(h, &a);
    host_close(h);
}

void test_load_path(host_t *h)
{
    bridge_artifact_t a;

    if (host_open(h))
        return;
    host_state_native(&a, "game:mover@0.1.0", h->arg);
    host_load(h, &a);
    host_close(h);
}

void test_hand_setting(host_t *h)
{
    h->backends[0].settings = TEST_SETTING;
    h->backends[0].setting_count = (uint32_t)(*h->arg - '0');
    host_open_only(h);
}
