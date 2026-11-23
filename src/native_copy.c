/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "native_inside.h"

static size_t name_at(const char *path)
{
    const char *slash = strrchr(path, '/');
    const char *back = strrchr(path, '\\');
    const char *last = slash > back ? slash : back;

    return (last ? (size_t)(last - path) + 1 : 0);
}

static char *copy_path(backend_t *b, const char *path, backend_err_t *err)
{
    const char *name = path + name_at(path);
    size_t size = strlen(b->copies) + strlen(name) + 24;
    char *copy = native_allocate(size, "the path of a copy", err);

    if (copy)
        snprintf(copy, size, "%s/%llu-%s", b->copies,
            (unsigned long long)++b->generation, name);
    return (copy);
}

static int write_from_path(const char *path, const char *copy)
{
    size_t cut = name_at(path);
    char *folder = calloc(1, cut + 2);
    os_folder_t *f = nullptr;
    char *bytes = nullptr;
    size_t len = 0;
    int wrote = -1;

    if (folder && cut)
        memcpy(folder, path, cut);
    else if (folder)
        folder[0] = '.';
    f = folder ? os_folder_open(folder) : nullptr;
    bytes = f ? os_folder_read_file(f, path + cut, &len) : nullptr;
    if (bytes)
        wrote = os_file_write(copy, bytes, len, 0);
    os_folder_close(f);
    free(folder);
    return (wrote);
}

void *native_copy_open(backend_t *b, const backend_source_t *source,
    char **copy, backend_err_t *err)
{
    void *lib = nullptr;
    int wrote = -1;

    *copy = copy_path(b, source->path, err);
    if (!*copy)
        return (nullptr);
    wrote = source->bytes ? os_file_write(*copy, source->bytes, source->len,
        0) : write_from_path(source->path, *copy);
    lib = wrote ? nullptr : os_lib_open(*copy);
    if (!lib) {
        snprintf(err->text, sizeof(err->text),
            "cannot open the library '%.180s'", source->path);
        native_copy_remove(*copy);
        *copy = nullptr;
    }
    return (lib);
}

void native_copy_remove(char *copy)
{
    (void)os_path_remove(copy);
    free(copy);
}
