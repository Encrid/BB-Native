/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "cases.h"

static const char *const TEST_SAY_PHASES[] = {
    "lines", "cut", "cut_odd", "no_room", "full", "after_full"
};

constexpr uint32_t TEST_SAY_COUNT = sizeof(TEST_SAY_PHASES)
    / sizeof(*TEST_SAY_PHASES);
constexpr uint32_t TEST_SAY_RING = 8;

void test_say(host_t *h)
{
    bridge_artifact_t a;

    h->cfg.phases = TEST_SAY_PHASES;
    h->cfg.phase_count = TEST_SAY_COUNT;
    if (host_open(h))
        return;
    host_state_native(&a, "test:say@0.1.0", NSAY_PATH);
    a.cfg.log_level = (uint8_t)(*h->arg - '0');
    a.cfg.log_ring = TEST_SAY_RING * (uint32_t)sizeof(bridge_log_record_t);
    host_load(h, &a);
    test_run_phases(h, TEST_SAY_PHASES, TEST_SAY_COUNT);
    host_close(h);
}
