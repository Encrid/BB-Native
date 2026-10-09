/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/host/run.c -- what every case has, and a module run: one that
** reads and writes through pointers, one an entry refuses, and one that
** reaches an entry from a thread of its own.
**
** native.md, *The invariant*
*/

#include "cases.h"

/*
** test_hear
**
** The sink of this program: prints a record with its barrier, whose it is
** and its two numbers.
*/
void test_hear([[maybe_unused]] void *ud, const bridge_record_t *r)
{
    const char *identity = r->identity ? r->identity : "-";
    const char *system = r->system ? r->system : "-";

    printf("record[%d] b%u %s/%s: %.*s %u %u\n", r->level, r->when >> 16,
        identity, system, (int)r->len, r->text, r->arg[0], r->arg[1]);
}

/*
** test_set
**
** The fixture every case has: the one backend, both phases, and the sink.
*/
void test_set(host_t *h)
{
    h->sink = test_hear;
}

/*
** test_load
**
** Loads a module that mints up to `spawns` ids between two barriers, and
** says how it ended.
*/
void test_load(host_t *h, const char *identity, const char *path,
    int64_t spawns)
{
    bridge_artifact_t a;

    host_state_native(&a, identity, path);
    a.cfg.spawns = spawns;
    host_load(h, &a);
}

/*
** test_run_mover
**
** The mover: a system that reads and writes through pointers, and says
** what its query matched.
*/
void test_run_mover(host_t *h)
{
    if (host_open(h))
        return;
    test_load(h, "game:mover@0.1.0", NMOVER_PATH, 1024);
    host_run_frames_unsaid(h->b, 2);
    host_close(h);
}

/*
** test_refuse_entry
**
** A module an entry refuses: the invocation fails when it returns.
*/
void test_refuse_entry(host_t *h)
{
    if (host_open(h))
        return;
    test_load(h, "test:flood@0.1.0", NFLOOD_PATH, 1024);
    (void)bridge_run_phase(h->b, "update");
    host_close(h);
}
