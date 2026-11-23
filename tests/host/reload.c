/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include "cases.h"

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
