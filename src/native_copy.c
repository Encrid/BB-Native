/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>

#include "native_inside.h"

static char *make_generation(backend_t *b, backend_err_t *err)
{
    char name[24];
    char *folder = nullptr;

    snprintf(name, sizeof(name), "%llu",
        (unsigned long long)++b->generation);
    folder = native_join(b->copies, name, err);
    if (folder && os_folder_make(folder)) {
        snprintf(err->text, sizeof(err->text),
            "cannot make the folder of a copy '%.180s'", folder);
        free(folder);
        return (nullptr);
    }
    return (folder);
}

static int write_copy(const backend_source_t *source, const char *path)
{
    if (source->bytes)
        return (os_file_write(path, source->bytes, source->len, 0));
    return (native_copy_file(source->path, path));
}

void *native_copy_open(backend_t *b, const backend_source_t *source,
    char **copy, backend_err_t *err)
{
    const char *name = source->path + native_name_at(source->path);
    char *from = native_folder_of(source->path, err);
    char *path = nullptr;
    void *lib = nullptr;

    *copy = from ? make_generation(b, err) : nullptr;
    path = *copy ? native_join(*copy, name, err) : nullptr;
    if (path)
        native_give_beside(from, name, *copy);
    lib = path && !write_copy(source, path) ? os_lib_open(path) : nullptr;
    if (path && !lib)
        snprintf(err->text, sizeof(err->text),
            "cannot open the library '%.180s'", source->path);
    if (*copy && !lib)
        native_copy_remove(*copy);
    *copy = lib ? *copy : nullptr;
    free(path);
    free(from);
    return (lib);
}

void native_copy_remove(char *copy)
{
    (void)os_path_remove(copy);
    free(copy);
}
