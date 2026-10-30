# src/native_limit.c

This file holds the four limits of an instance, which the backend ignores.

## The limits kept as nothing

`instance_set_work_limit`, `instance_set_timeout`, `instance_set_memory_limit` and `instance_set_element_limit` do nothing.
The backend reports none of these guarantees, so Bridge never hands it a number to keep.

## Functions

| Function | Result |
| --- | --- |
| `instance_set_work_limit` | Does nothing. |
| `instance_set_timeout` | Does nothing. |
| `instance_set_memory_limit` | Does nothing. |
| `instance_set_element_limit` | Does nothing. |
