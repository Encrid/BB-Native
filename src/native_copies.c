/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "native_inside.h"

static bool is_copy(const char *name)
{
    size_t digits = strspn(name, "0123456789");

    return (digits && name[digits] == '-' && name[digits + 1]);
}

static void clear_copies(const char *folder)
{
    os_folder_t *f = os_folder_open(folder);
    bool is_folder = false;
    const char *name = f ? os_folder_read_entry(f, &is_folder) : nullptr;
    size_t size = 0;
    char *path = nullptr;

    for (; name; name = os_folder_read_entry(f, &is_folder)) {
        size = strlen(folder) + strlen(name) + 2;
        path = is_folder || !is_copy(name) ? nullptr : calloc(1, size);
        if (path) {
            snprintf(path, size, "%s/%s", folder, name);
            (void)os_path_remove(path);
        }
        free(path);
    }
    os_folder_close(f);
}

static int refuse(backend_err_t *err, const char *text, const char *word)
{
    snprintf(err->text, sizeof(err->text), text, word);
    return (1);
}

static int find_folder(const backend_setting_t *settings, uint32_t count,
    const char **folder, backend_err_t *err)
{
    const char *key = nullptr;

    for (uint32_t i = 0; i < count; i++) {
        key = settings[i].key ? settings[i].key : "";
        if (strcmp(key, "copies"))
            return (refuse(err, "it knows no setting '%.100s'", key));
        if (*folder)
            return (refuse(err, "it states the setting %s 2 times", key));
        if (settings[i].kind != BACKEND_STRING || !settings[i].v.s
            || !*settings[i].v.s)
            return (refuse(err, "the setting %s is a folder, as a text", key));
        *folder = settings[i].v.s;
    }
    if (!*folder)
        return (refuse(err, "it needs the setting copies: a folder for the "
            "copies of the modules%s", ""));
    return (0);
}

int native_copies_take(backend_t *b, const backend_setting_t *settings,
    uint32_t count, backend_err_t *err)
{
    const char *folder = nullptr;
    size_t size = 0;

    if (find_folder(settings, count, &folder, err))
        return (1);
    size = strlen(folder) + 1;
    b->copies = native_allocate(size, "the folder of the copies", err);
    if (!b->copies)
        return (1);
    memcpy(b->copies, folder, size);
    if (os_folder_make(folder))
        return (refuse(err, "cannot make the folder of the copies '%.180s'",
            folder));
    clear_copies(folder);
    return (0);
}
