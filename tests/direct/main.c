/*
** Copyright (C) 2026 BB-Native Owner
**
** This file is part of BB-Native.
**
** Under the GNU Affero General Public License, version 3 only: see license.
**
** tests/direct/main.c -- the backend, called as the library calls it, for
** what no host can ask of it through the library: an entry that is none of
** the ABI's.
**
** native.md, *What the backend provides*
*/

#include <stdio.h>

#include "backend.h"

/*
** refuse_entry
**
** Asks the backend to keep what answers an entry the ABI does not have.
** Answers 1, having said so, if it was accepted.
*/
static int refuse_entry(const backend_api_t *api, backend_module_t *m)
{
    backend_err_t err = {};
    backend_entry_t entry = { "bridge", "teleport", nullptr, nullptr,
        nullptr };
    backend_instance_t *i = api->instance_create(m, nullptr, 0, &err);
    int refused = api->define_func(i, &entry, &err);

    api->instance_destroy(i);
    if (!refused)
        return (printf("FAIL: an entry the SDK has not was kept\n"), 1);
    printf("an entry the SDK has not is refused: %s\n", err.text);
    return (0);
}

/*
** main
**
** Opens the backend, opens a module, and makes the check. Nothing is
** buffered: a crash must not swallow what was said.
*/
int main(void)
{
    backend_api_t api;
    backend_err_t err = {};
    backend_source_t source = { .path = NMOVER_PATH };
    backend_t *be;
    backend_module_t *m;

    setvbuf(stdout, nullptr, _IONBF, 0);
    if (!backend_open(NATIVE_PATH, &api, &err))
        return (printf("FAIL: %s\n", err.text), 1);
    be = api.backend_create(nullptr, 0, &err);
    m = api.module_compile(be, &source, &err);
    if (!m || refuse_entry(&api, m))
        return (printf("FAIL: %s\n", err.text), 1);
    api.module_destroy(m);
    api.backend_destroy(be);
    return (0);
}
