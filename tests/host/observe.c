/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/host/observe.c -- what one module attaches, seen by another: an
** attach with its value, the changes a system states, and an entry reached
** from a thread nobody called on.
**
** native.md, *What the backend provides*
*/

#include "cases.h"

static const char *const TEST_GAME[2] = {
    "game:Position",
    "game:Marked"
};

static const bridge_order_t TEST_FIVE_FIRST = {
    "game:stamp5:stamp",
    "game:stamp9:stamp"
};

/*
** test_load_watcher
**
** Loads the watcher, an observer of another namespace, with its grant.
*/
void test_load_watcher(host_t *h)
{
    bridge_artifact_t a;

    host_state_native(&a, "view:watcher@0.1.0", NWATCHER_PATH);
    a.cfg.read = TEST_GAME;
    a.cfg.read_count = 2;
    host_load(h, &a);
}

/*
** test_observe
**
** One module attaches a value, and an observer of another reads it.
*/
void test_observe(host_t *h)
{
    if (host_open(h))
        return;
    test_load(h, "game:setter@0.1.0", NSETTER_PATH, 8);
    test_load_watcher(h);
    host_run_frames_unsaid(h->b, 2);
    host_close(h);
}

/*
** test_order_changes
**
** Two modules change one component of the same entities, each having
** stated that it does. The five is ordered before the nine: the nine stays.
*/
void test_order_changes(host_t *h)
{
    h->cfg.order = &TEST_FIVE_FIRST;
    h->cfg.order_count = 1;
    if (host_open(h))
        return;
    test_load(h, "game:stamp5@0.1.0", NSTAMP5_PATH, 8);
    test_load(h, "game:stamp9@0.1.0", NSTAMP9_PATH, 8);
    test_load_watcher(h);
    host_run_frames_unsaid(h->b, 2);
    host_close(h);
}

/*
** test_enter_from_foreign_thread
**
** An entry reached from a thread nobody called on.
*/
void test_enter_from_foreign_thread(host_t *h)
{
    if (host_open(h))
        return;
    test_load(h, "test:foreign@0.1.0", NFOREIGN_PATH, 8);
    host_run_frames_unsaid(h->b, 1);
    host_close(h);
}
