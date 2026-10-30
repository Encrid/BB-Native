/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>

#include "native_inside.h"

BACKEND_EXPORT backend_t *backend_create(const backend_setting_t *settings,
    uint32_t count, backend_err_t *err)
{
    const char *key = count && settings[0].key ? settings[0].key : "";

    if (!count)
        return (native_allocate(sizeof(backend_t), "the backend", err));
    snprintf(err->text, sizeof(err->text), "it knows no setting '%.100s'", key);
    return (nullptr);
}

BACKEND_EXPORT void backend_destroy(backend_t *b)
{
    free(b);
}

BACKEND_EXPORT enum backend_capability backend_get_capabilities(
    [[maybe_unused]] backend_t *b)
{
    return (BACKEND_RELOAD | BACKEND_UNLOAD);
}

BACKEND_EXPORT void memory_get_shape([[maybe_unused]] backend_t *b,
    uint64_t *reservation, uint64_t *guard, uint64_t *granule)
{
    *reservation = 0;
    *guard = 0;
    *granule = 0;
}

BACKEND_EXPORT backend_memory_t *memory_adopt([[maybe_unused]] backend_t *b,
    void *ptr, [[maybe_unused]] uint64_t len,
    [[maybe_unused]] backend_err_t *err)
{
    return ((backend_memory_t *)ptr);
}
