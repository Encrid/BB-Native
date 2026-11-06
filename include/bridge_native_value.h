/*
** Copyright (C) 2026 Enkrid
**
** Under the GNU Affero General Public License, version 3 only: see license.
*/
#ifndef BRIDGE_NATIVE_VALUE_H
    #define BRIDGE_NATIVE_VALUE_H

    #include "bridge_native_api.h"

static inline uint32_t bridge_native_find_value(const bridge_api_t *api,
    uint64_t id, uint32_t slot)
{
    const bridge_header_t *h = bridge_native_get_header(api);
    const bridge_entity_t *e;
    uint32_t base;

    if ((uint32_t)id >= h->entities)
        return (0);
    e = (const bridge_entity_t *)(api->shared_base + h->entity_index)
        + (uint32_t)id;
    if (e->generation != (uint32_t)(id >> 32))
        return (0);
    base = ((const uint32_t *)(api->private_base + h->column_base))[e->table
        * h->types + slot];
    if (!base)
        return (0);
    return (base + e->row
        * ((const uint32_t *)(api->private_base + h->stride))[slot]);
}

#endif
