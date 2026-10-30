# src/native_instance.c

This file makes and frees a module instance.
An instance carries the entry table, the two memory bases, and the functions found in it.

## Functions

| Function | Result |
| --- | --- |
| `instance_create` | Takes an instance from `native_allocate`, fills its table, and points it at the shared and private memory. |
| `instance_destroy` | Frees the functions found in the instance, then the instance. |
