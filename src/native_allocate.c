/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>
#include <stdlib.h>

#include "native_inside.h"

void *native_allocate(size_t size, const char *what, backend_err_t *err)
{
    void *p = calloc(1, size);

    if (!p)
        snprintf(err->text, sizeof(err->text), "out of memory: there was "
            "none for %s", what);
    return (p);
}
