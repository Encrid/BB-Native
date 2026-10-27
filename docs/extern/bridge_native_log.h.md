# include/bridge_native_log.h

This header gives `bridge_line_t`, one line of the log, and `bridge_native_log` and `bridge_native_say`, which write a line.
A module writes a line into the ring of its private memory, when the line's level is one it keeps.
`bridge_line_t` holds the line's `text` and its `len`, with two numbers `a0` and `a1` a format reads.

## Writing a line

`bridge_native_log` reads the header, finds the ring, and moves the head forward.
A ring that is already full drops the line, and the moved head counts the drop.
It stages the line's text in the arena, then fills the record from the header and the line.

## Writing a line with values

`bridge_native_say(api, level, format, ...)` makes the text of a line from `format` and the values after it, as `vsnprintf` does.
It makes the text in the arena, and then writes the line as `bridge_native_log` does, with 0 in `a0` and `a1`.
A text longer than the room left in the arena is cut to that room.
A level that the module does not keep writes nothing, and makes no text.
Thus each module writes a message with values through one helper.

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_log` | Writes a line into the module's ring, when its level is kept. |
| `bridge_native_say` | Makes the text of a line from a format and its values, and writes the line, when its level is kept. |
