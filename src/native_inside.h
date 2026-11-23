/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_BACKENDS_NATIVE_INSIDE_H
    #define BRIDGE_BACKENDS_NATIVE_INSIDE_H

    #include <stddef.h>

    #include "bridge_backend.h"
    #include "bridge_native.h"
    #include "os.h"

enum native_entry : uint32_t {
    NATIVE_DECLARE,
    NATIVE_ADD_SYSTEM,
    NATIVE_OBSERVE,
    NATIVE_SPAWN,
    NATIVE_DESTROY,
    NATIVE_ATTACH,
    NATIVE_DETACH,
    NATIVE_ALLOC,
    NATIVE_ENTRIES
};

struct backend {
    char *copies;
    uint64_t generation;
};

struct backend_module {
    void *lib;
    char *copy;
};

struct backend_func {
    backend_instance_t *instance;
    int (*fn)(const bridge_api_t *);
    backend_func_t *next;
};

struct backend_instance {
    backend_module_t *m;
    bridge_api_t api;
    backend_fn_t fns[NATIVE_ENTRIES];
    void *uds[NATIVE_ENTRIES];
    uint64_t calling;
    int refused;
    backend_func_t *funcs;
};

static inline uint64_t native_enter(const bridge_api_t *api,
    enum native_entry entry, const uint64_t *args)
{
    char *base = (char *)api - offsetof(backend_instance_t, api);
    backend_instance_t *i = (backend_instance_t *)base;
    uint64_t result = 0;

    if (i->refused || !i->fns[entry])
        return (0);
    if (i->calling != os_get_thread_id())
        return (0);
    if (!i->fns[entry](i->uds[entry], args, &result))
        return (result);
    i->refused = 1;
    return (0);
}

[[nodiscard]] void *native_allocate(size_t size, const char *what,
    backend_err_t *err);

void native_fill_api(bridge_api_t *api);

[[nodiscard]] int native_copies_take(backend_t *b,
    const backend_setting_t *settings, uint32_t count, backend_err_t *err);

[[nodiscard]] void *native_copy_open(backend_t *b,
    const backend_source_t *source, char **copy, backend_err_t *err);

void native_copy_remove(char *copy);

[[nodiscard]] size_t native_name_at(const char *path);

[[nodiscard]] char *native_folder_of(const char *path, backend_err_t *err);

[[nodiscard]] char *native_join(const char *folder, const char *name,
    backend_err_t *err);

[[nodiscard]] int native_copy_file(const char *path, const char *copy);

void native_give_beside(const char *from, const char *skip,
    const char *folder);

uint64_t native_spawn(const bridge_api_t *api);
void native_destroy(const bridge_api_t *api, uint64_t id);
void native_attach(const bridge_api_t *api, uint64_t id, uint32_t slot,
    uint32_t ptr);
void native_detach(const bridge_api_t *api, uint64_t id, uint32_t slot);
uint32_t native_alloc(const bridge_api_t *api, uint32_t len);

#endif
