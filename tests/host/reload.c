/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <direct.h>
    #include <windows.h>
#else
    #include <sys/stat.h>
#endif
#include "cases.h"
#include "sweep.h"

static const char *const LEFT[] = {
    "7-old.module", "keep.txt", "8-", "x-y", "77x", "12/f", "9-dir/f", "dir/f"
};

static int copy_file(const char *from, const char *to)
{
    char bytes[4096];
    FILE *in = fopen(from, "rb");
    FILE *out = in ? fopen(to, "wb") : nullptr;
    size_t got = 0;
    int failed = !out;

    while (out && (got = fread(bytes, 1, sizeof(bytes), in)) > 0)
        failed |= fwrite(bytes, 1, got, out) != got;
    if (out)
        failed |= fclose(out) != 0;
    if (in)
        fclose(in);
    return (failed ? -1 : 0);
}

static void *read_file(const char *path, size_t *len)
{
    static char bytes[1 << 20];
    FILE *in = fopen(path, "rb");

    *len = in ? fread(bytes, 1, sizeof(bytes), in) : 0;
    if (in)
        fclose(in);
    return (bytes);
}

static void make_folder(const char *path)
{
#ifdef _WIN32
    (void)_mkdir(path);
#else
    (void)mkdir(path, 0777);
#endif
}

static char *in_copies(const char *name)
{
    static char path[512];

    snprintf(path, sizeof(path), "%s/%s", getenv("TEST_COPIES"), name);
    return (path);
}

void test_clear_copies(host_t *h)
{
    FILE *made = nullptr;

    if (!getenv("TEST_COPIES"))
        return;
    make_folder(getenv("TEST_COPIES"));
    make_folder(in_copies("9-dir"));
    make_folder(in_copies("12"));
    make_folder(in_copies("dir"));
    for (size_t i = 0; i < sizeof(LEFT) / sizeof(*LEFT); i++) {
        made = fopen(in_copies(LEFT[i]), "wb");
        if (made)
            fclose(made);
    }
    host_open_only(h);
    for (size_t i = 0; i < sizeof(LEFT) / sizeof(*LEFT); i++) {
        made = fopen(in_copies(LEFT[i]), "rb");
        printf("%s %s\n", LEFT[i], made ? "stays" : "gone");
        if (made)
            fclose(made);
    }
}

static int move_onto(const char *from, const char *to)
{
#ifdef _WIN32
    return (MoveFileExA(from, to, MOVEFILE_REPLACE_EXISTING) ? 0 : -1);
#else
    return (rename(from, to));
#endif
}

static void place(const char *from, const char *folder, const char *name)
{
    char to[512];

    snprintf(to, sizeof(to), "%s/%s", folder, name);
    if (copy_file(from, to))
        printf("cannot place %s\n", name);
}

static void move_helper(const char *folder, bool none)
{
    char from[512];
    char to[512];

    snprintf(from, sizeof(from), "%s/nhelper.new", folder);
    snprintf(to, sizeof(to), "%s/nhelper%s", folder, BUILT_SUFFIX);
    if (!none)
        place(NHELPER8_PATH, folder, "nhelper.new");
    printf("move: %d\n", move_onto(from, to));
}

static void make_empty(const char *folder, const char *name)
{
    char path[600];
    FILE *made = nullptr;

    snprintf(path, sizeof(path), "%s/%s", folder, name);
    made = fopen(path, "wb");
    if (made)
        fclose(made);
}

static void fill_sweep(const char *folder, bool helped)
{
    char sub[600];

    make_folder(folder);
    place(NIMPORTS_PATH, folder, "nimports.module");
    if (helped)
        place(NHELPER_PATH, folder, "nhelper" BUILT_SUFFIX);
    make_empty(folder, "libextra.so.1");
    make_empty(folder, "notes.txt");
    make_empty(folder, "x");
    snprintf(sub, sizeof(sub), "%s/sub", folder);
    make_folder(sub);
}

void test_sweep_links(host_t *h)
{
    char folder[512];
    char module[512];
    bridge_artifact_t a;
    unsigned at = 0;
    unsigned loaded = 0;

    if (!getenv("TEST_IMPORTS"))
        return;
    snprintf(folder, sizeof(folder), "%s-%s", getenv("TEST_IMPORTS"), h->arg);
    snprintf(module, sizeof(module), "%s/nimports.module", folder);
    fill_sweep(folder, !strcmp(h->arg, "sweep"));
    do {
        sweep_refuse("os_file_link", ++at);
        if (host_open(h))
            return;
        host_state_native(&a, "test:imports@0.1.0", module);
        loaded += !bridge_load(h->w, &a);
        host_run_frames_unsaid(h->w, 1);
        (void)bridge_destroy(h->w);
    } while (sweep_is_refused());
    sweep_plan("");
    printf("links: %u refused, %u loads of %u\n", at - 1, loaded, at);
}

void test_run_mover_alone(host_t *h)
{
    char folder[512];
    char module[512];

    if (!getenv("TEST_ALONE"))
        return;
    snprintf(folder, sizeof(folder), "%s-%s", getenv("TEST_ALONE"), h->what);
    snprintf(module, sizeof(module), "%s/nmover.module", folder);
    make_folder(folder);
    place(NMOVER_PATH, folder, "nmover.module");
    if (host_open(h))
        return;
    test_load(h, "game:mover@0.1.0", module, 1024);
    host_run_frames_unsaid(h->w, 2);
    host_close(h);
}

void test_run_imports(host_t *h)
{
    char folder[512];
    char module[512];
    bridge_artifact_t a;

    if (!getenv("TEST_IMPORTS"))
        return;
    snprintf(folder, sizeof(folder), "%s-%s", getenv("TEST_IMPORTS"), h->arg);
    snprintf(module, sizeof(module), "%s/nimports.module", folder);
    make_folder(folder);
    place(NIMPORTS_PATH, folder, "nimports.module");
    if (strcmp(h->arg, "alone"))
        place(NHELPER_PATH, folder, "nhelper" BUILT_SUFFIX);
    if (host_open(h))
        return;
    host_state_native(&a, "test:imports@0.1.0", module);
    host_load(h, &a);
    host_run_frames_unsaid(h->w, 1);
    if (!strncmp(h->arg, "move", 4))
        move_helper(folder, !strcmp(h->arg, "move-none"));
    host_close(h);
}

static void state_build(bridge_artifact_t *a, const char *path)
{
    host_state_native(a, "test:build@0.1.0", path);
    a->cfg.reloadable = BRIDGE_TRUE;
    a->cfg.hot_reload = BRIDGE_TRUE;
}

static void reload_build(host_t *h, const char *content, const char *build,
    int at)
{
    bridge_artifact_t a;

    printf("write %d: %d\n", at, copy_file(build, content));
    state_build(&a, content);
    if (!strcmp(h->arg, "bytes"))
        a.bytes = read_file(build, &a.len);
    printf("reload %d: %d\n", at, bridge_reload(h->w, &a, 1));
    host_run_frames_unsaid(h->w, 1);
}

void test_reload_builds(host_t *h)
{
    const char *content = getenv("TEST_CONTENT");
    bool same = !strcmp(h->arg, "same");
    bridge_artifact_t a;

    if (!content || host_open(h))
        return;
    printf("write 1: %d\n", copy_file(NBUILD_PATH, content));
    state_build(&a, content);
    host_load(h, &a);
    host_run_frames_unsaid(h->w, 1);
    reload_build(h, content, same ? NBUILD_PATH : NBUILD2_PATH, 2);
    reload_build(h, content, same ? NBUILD_PATH : NBUILD3_PATH, 3);
    host_close(h);
}
