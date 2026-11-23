/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdlib.h>
#include <string.h>

#include "native_inside.h"

#if defined(_WIN32)
constexpr char LIBRARY[] = ".dll";
#elif defined(__APPLE__)
constexpr char LIBRARY[] = ".dylib";
#else
constexpr char LIBRARY[] = ".so";
#endif

static bool is_library(const char *name)
{
    size_t len = strlen(name);
    size_t suffix = sizeof(LIBRARY) - 1;

#if defined(__linux__)
    if (strstr(name, ".so."))
        return (true);
#endif
    return (len > suffix && !strcmp(name + len - suffix, LIBRARY));
}

static void give_one(const char *from, const char *folder, const char *name)
{
    backend_err_t err = {};
    char *source = native_join(from, name, &err);
    char *to = source ? native_join(folder, name, &err) : nullptr;

    if (to && os_file_link(source, to) && is_library(name))
        (void)native_copy_file(source, to);
    free(source);
    free(to);
}

void native_give_beside(const char *from, const char *skip,
    const char *folder)
{
    os_folder_t *f = os_folder_open(from);
    bool is_folder = false;
    const char *name = f ? os_folder_read_entry(f, &is_folder) : nullptr;

    for (; name; name = os_folder_read_entry(f, &is_folder))
        if (!is_folder && strcmp(name, skip))
            give_one(from, folder, name);
    os_folder_close(f);
}
