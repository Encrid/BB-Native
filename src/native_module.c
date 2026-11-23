/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "native_inside.h"

static const char *const NATIVE_MEMORIES[2] = {
    BRIDGE_SHARED,
    BRIDGE_PRIVATE
};

BACKEND_EXPORT void memory_release([[maybe_unused]] backend_memory_t *m)
{
}

BACKEND_EXPORT backend_module_t *module_compile(backend_t *b,
    const backend_source_t *source, backend_err_t *err)
{
    const char *path = source->path;
    char *copy = nullptr;
    void *lib = path && *path ? native_copy_open(b, source, &copy, err)
        : nullptr;
    backend_module_t *m;

    if (!path || !*path)
        snprintf(err->text, sizeof(err->text), "this backend opens a "
            "module's library by its path, and was handed none");
    if (!lib)
        return (nullptr);
    m = native_allocate(sizeof(*m), "the image of a module", err);
    if (!m) {
        os_lib_close(lib);
        native_copy_remove(copy);
    } else {
        *m = (backend_module_t){ lib, copy };
    }
    return (m);
}

BACKEND_EXPORT uint32_t module_list_imports(
    [[maybe_unused]] backend_module_t *m, backend_import_t *out, uint32_t max)
{
    uint32_t i = 0;

    while (i < 2 && i < max) {
        out[i].ns = BRIDGE_NAMESPACE;
        out[i].ns_len = sizeof(BRIDGE_NAMESPACE) - 1;
        out[i].field = NATIVE_MEMORIES[i];
        out[i].field_len = strlen(NATIVE_MEMORIES[i]);
        out[i].kind = BACKEND_IMPORT_MEMORY;
        i++;
    }
    return (2);
}

BACKEND_EXPORT void module_destroy(backend_module_t *m)
{
    os_lib_close(m->lib);
    native_copy_remove(m->copy);
    free(m);
}
