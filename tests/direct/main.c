/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bridge_backend.h"
#include "rows.h"

static int calls;
static bool refusing;

static int spawn_stub([[maybe_unused]] void *ud,
    [[maybe_unused]] const uint64_t *args, uint64_t *result)
{
    calls++;
    *result = 5;
    return (refusing && calls == 1 ? -1 : 0);
}

static backend_instance_t *made(const backend_api_t *api, backend_module_t *m,
    bool spawns)
{
    backend_err_t err = {};
    backend_entry_t spawn = { "bridge", "spawn", spawn_stub, nullptr,
        nullptr };
    backend_instance_t *i = api->instance_create(m, nullptr, 0, &err);

    if (i && spawns)
        (void)api->define_func(i, &spawn, &err);
    return (i);
}

static int32_t called(const backend_api_t *api, backend_instance_t *i,
    const char *name)
{
    backend_err_t err = {};
    backend_func_t *f = i ? api->instance_get_func(i, name, &err) : nullptr;
    int32_t status = -2;

    if (f)
        (void)api->call(f, &status, &err);
    return (status);
}

static int enter(const backend_api_t *api, backend_t *be, const char *mode)
{
    backend_err_t err = {};
    backend_source_t source = { .path = NCOVER_PATH };
    backend_module_t *m = api->module_compile(be, &source, &err);
    backend_instance_t *one = nullptr;
    backend_instance_t *two = nullptr;

    calls = 0;
    refusing = !strcmp(mode, "enter-refused");
    one = m ? made(api, m, strcmp(mode, "enter-unset")) : nullptr;
    printf("%s: init %d, calls %d\n", mode, called(api, one, "init"), calls);
    if (m && !strcmp(mode, "enter-late")) {
        two = made(api, m, true);
        printf("late %d, calls %d\n", called(api, two, "late"), calls);
    }
    if (two)
        api->instance_destroy(two);
    if (one)
        api->instance_destroy(one);
    if (m)
        api->module_destroy(m);
    return (!m);
}

static int refuse_entry(const backend_api_t *api, backend_module_t *m,
    const char *name)
{
    backend_err_t err = {};
    backend_entry_t entry = { "bridge", name, nullptr, nullptr, nullptr };
    backend_instance_t *i = api->instance_create(m, nullptr, 0, &err);
    int refused = api->define_func(i, &entry, &err);

    api->instance_destroy(i);
    if (!refused)
        return (printf("FAIL: an entry the SDK has not was kept\n"), 1);
    printf("an entry the SDK has not is refused: %s\n", err.text);
    return (0);
}

static void poke_edges(const backend_api_t *api, backend_t *be,
    backend_module_t *m)
{
    backend_err_t err = {};
    backend_setting_t keyless = {};
    backend_entry_t wrong = { "other", "spawn", nullptr, nullptr, nullptr };
    backend_source_t empty = { .path = "" };
    backend_source_t missing = { .path = "no-such-library" };
    backend_import_t out[2] = {};
    backend_instance_t *i = api->instance_create(m, nullptr, 0, &err);

    (void)api->backend_create(&keyless, 1, &err);
    (void)api->module_compile(be, &empty, &err);
    (void)api->module_compile(be, &missing, &err);
    (void)api->module_list_imports(m, out, 1);
    if (api->define_func(i, &wrong, &err))
        printf("another namespace is refused: %s\n", err.text);
    api->instance_set_work_limit(i, 1);
    api->instance_set_timeout(i, 1);
    api->instance_set_memory_limit(i, 1);
    api->instance_set_element_limit(i, 1);
    api->instance_destroy(i);
}

static void poke_missing(const backend_api_t *api, backend_t *be)
{
    backend_err_t err = {};
    backend_source_t source = { .path = NCOVER_PATH };
    backend_module_t *m = api->module_compile(be, &source, &err);
    backend_instance_t *i = api->instance_create(m, nullptr, 0, &err);
    backend_func_t *init = api->instance_get_func(i, "init", &err);
    int32_t status = 0;

    (void)api->call(init, &status, &err);
    api->instance_destroy(i);
    api->module_destroy(m);
}

static void poke_all(const backend_api_t *api, backend_t *be,
    backend_module_t *m)
{
    poke_edges(api, be, m);
    poke_missing(api, be);
}

static int run(int argc, char **argv)
{
    backend_api_t api;
    backend_err_t err = {};
    backend_source_t source = { .path = NMOVER_PATH };
    backend_setting_t copies = { "copies", BACKEND_STRING, {},
        { .s = getenv("TEST_COPIES") } };
    backend_t *be;
    backend_module_t *m;
    void *lib = backend_open(BRIDGE_NATIVE_PATH, &api, &err);
    int failed;

    if (!lib)
        return (printf("FAIL: %s\n", err.text), 1);
    be = api.backend_create(&copies, 1, &err);
    if (argc > 1 && !strncmp(argv[1], "enter-", 6)) {
        failed = enter(&api, be, argv[1]);
        api.backend_destroy(be);
        backend_close(lib);
        return (failed);
    }
    m = api.module_compile(be, &source, &err);
    failed = !m || refuse_entry(&api, m, argc > 1 ? argv[1] : "");
    if (failed)
        printf("FAIL: %s\n", err.text);
    else
        poke_all(&api, be, m);
    if (m)
        api.module_destroy(m);
    api.backend_destroy(be);
    backend_close(lib);
    return (failed);
}

int main(int argc, char **argv)
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    return (argc > 1 ? run(argc, argv) : rows_run(run));
}
