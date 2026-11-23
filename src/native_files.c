/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "native_inside.h"

size_t native_name_at(const char *path)
{
    const char *slash = strrchr(path, '/');
    const char *back = strrchr(path, '\\');
    const char *last = slash > back ? slash : back;

    return (last ? (size_t)(last - path) + 1 : 0);
}

char *native_folder_of(const char *path, backend_err_t *err)
{
    size_t cut = native_name_at(path);
    char *folder = native_allocate(cut + 2, "the path of a copy", err);

    if (folder && cut)
        memcpy(folder, path, cut);
    else if (folder)
        folder[0] = '.';
    return (folder);
}

char *native_join(const char *folder, const char *name, backend_err_t *err)
{
    size_t size = strlen(folder) + strlen(name) + 2;
    char *path = native_allocate(size, "the path of a copy", err);

    if (path)
        snprintf(path, size, "%s/%s", folder, name);
    return (path);
}

int native_copy_file(const char *path, const char *copy)
{
    backend_err_t err = {};
    char *folder = native_folder_of(path, &err);
    os_folder_t *f = folder ? os_folder_open(folder) : nullptr;
    size_t len = 0;
    char *bytes = f ? os_folder_read_file(f, path + native_name_at(path),
        &len) : nullptr;
    int wrote = bytes ? os_file_write(copy, bytes, len, 0) : -1;

    os_folder_close(f);
    free(folder);
    return (wrote);
}
