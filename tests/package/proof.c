/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#include <stdio.h>

#include "bridge_native.h"

BRIDGE_NATIVE(init)
{
    return (0);
}

int main(void)
{
    int status = bridge_init(nullptr);

    printf("native package: %s\n", status ? "no module" : "a module builds");
    return (status);
}
