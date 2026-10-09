/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/host/kept.c -- what is kept in other layouts than a table, read
** by a module through the SDK: a singleton, a toggle, a tag, a
** direct-indexed component and a sparse set. A module of these has a
** system a phase, and a case runs the phases its row lists, in turn.
**
** native.md, *Reaching a component*
*/

#include "cases.h"

static const char *const TEST_PHASES[] = {
    "spawn", "attach_a", "attach_b", "detach_b", "destroy", "attach_b_half",
    "detach_b_half", "detach_b_absent", "value_a", "flicker", "destroy_half",
    "respawn_half", "touch_dead", "attach_a_half", "recycled", "combine",
    "look", "never", "work", "check", "attach_both", "scribble", "find",
    "value_b", "look_b", "walk", "flag", "near", "far", "one"
};

static const test_kept_case_t TEST_KEPT[] = {
    { "single", NSINGLE_PATH, BRIDGE_NONE,
        { "look", "attach_a", "look", "value_a", "look", "work", "look" } },
    { "toggle", NTOGGLE_PATH, BRIDGE_NONE,
        { "spawn", "look", "flicker", "look", "look_b", "find" } },
    { "tag", NTAG_PATH, 1, { "spawn", "look", "flicker", "attach_a", "look" } },
    { "direct", NDIRECT_PATH, 1, { "spawn", "attach_b", "look_b", "near" } },
    { "sparse", NSPARSE_PATH, 1, { "spawn", "attach_b", "look", "look_b" } },
    { "layout", NLAYOUT0_PATH, 2, { "spawn", "look" } },
};

/*
** hear_plain
**
** A sink: prints a record by the system that wrote it, and its two numbers.
*/
static void hear_plain([[maybe_unused]] void *ud, const bridge_record_t *r)
{
    printf("  %s: %.*s %u %u\n", r->system ? r->system : "-", (int)r->len,
        r->text, r->arg[0], r->arg[1]);
}

/*
** set_room
**
** Makes the host's deployment that of a case: a phase for each system of
** such a module, and the room its layouts need.
*/
static void set_room(host_t *h, const test_kept_case_t *c)
{
    h->sink = hear_plain;
    h->cfg.phases = TEST_PHASES;
    h->cfg.phase_count = sizeof(TEST_PHASES) / sizeof(*TEST_PHASES);
    h->cfg.budget.tables = 32'768;
    h->cfg.budget.columns = 64 << 20;
    h->cfg.budget.sparse_sets = c->sets;
    h->cfg.budget.direct_indexed = c->sets;
}

/*
** run_phases
**
** Runs each phase a case lists, named before what was said in it.
*/
static void run_phases(host_t *h, const test_kept_case_t *c)
{
    uint32_t i = 0;

    while (i < 12 && c->phases[i]) {
        printf("%s\n", c->phases[i]);
        if (bridge_run_phase(h->b, c->phases[i]))
            printf("  refused\n");
        i++;
    }
}

/*
** test_read_kept
**
** The case its row names: the module loaded, and its phases run.
*/
void test_read_kept(host_t *h)
{
    const test_kept_case_t *c = TEST_KEPT;
    bridge_artifact_t a;

    while (strcmp(c->what, h->arg))
        c++;
    set_room(h, c);
    if (host_open(h))
        return;
    host_state_native(&a, "st:store@0.1.0", c->path);
    a.cfg.spawns = 16;
    a.cfg.spans = 1 << 20;
    a.cfg.log_ring = 256 * 24;
    host_load(h, &a);
    run_phases(h, c);
    host_close(h);
}
