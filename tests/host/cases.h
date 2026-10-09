/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/host/cases.h -- the cases of the host program: what each file of
** them shares, and every case, which main.c names by the row it runs.
*/

#ifndef TEST_CASES_H
    #define TEST_CASES_H

    #include <stdio.h>
    #include <string.h>

    #include "host.h"

/*
** test_hear
**
** The sink of this program: prints a record with its barrier, whose it is
** and its two numbers.
*/
void test_hear(void *ud, const bridge_record_t *r);

/*
** test_set
**
** The fixture every case has: the one backend, both phases, and the sink.
*/
void test_set(host_t *h);

/*
** test_load
**
** Loads a module that mints up to `spawns` ids between two barriers, and
** says how it ended.
*/
void test_load(host_t *h, const char *identity, const char *path,
    int64_t spawns);

/*
** test_load_watcher
**
** Loads the watcher, an observer of another namespace, with its grant.
*/
void test_load_watcher(host_t *h);

/*
** The cases. Each is handed the host its fixture set, and what its row
** states.
*/
void test_run_mover(host_t *h);
void test_refuse_entry(host_t *h);
void test_enter_from_foreign_thread(host_t *h);
void test_observe(host_t *h);
void test_order_changes(host_t *h);
void test_state_untrusted(host_t *h);
void test_state_limit(host_t *h);
void test_state_no_path(host_t *h);
void test_name_no_library(host_t *h);
void test_hand_setting(host_t *h);

#endif
