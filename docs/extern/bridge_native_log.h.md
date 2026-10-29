# include/bridge_native_log.h

This header gives `bridge_line_t`, one line of the log, and `bridge_native_log` and `bridge_native_say`, which write a line.
A module writes a line into the ring of its private memory, when the line's level is one it keeps.
`bridge_line_t` holds the line's `text` and its `len`, with two numbers `a0` and `a1` that go beside the text.

## Writing a line

`bridge_native_log` reads the header, finds the ring, and moves the head forward.
A ring that is already full drops the line, and the moved head counts the drop.
It stages the line's text in the arena, then fills the record from the header and the line.

## Writing a line with values

`bridge_native_say(api, level, format, ...)` makes the text of a line from `format` and the values after it, as `vsnprintf` does.
It checks the level and the ring as `bridge_native_log` does.
Then it makes the text one time, in place in the arena, and counts it in `scratch_used`.
Then it fills the record as `bridge_native_log` does.
Thus each module writes a message with values through one helper.

`a0` is the length of the full text, and `a1` is 0. When `a0` is more than the length of the text, the text was cut.

At a level that the module does not keep, it writes nothing and makes no text.
A line that a full ring drops makes no text.

The room for the text starts at the next multiple of 8 bytes, as for `bridge_native_put`.
A text longer than the room left is cut to the room less 1 byte, for the zero byte of `vsnprintf`.
With no room, the text is empty.
If `vsnprintf` gives an error, the line has an empty text, and `a0` is 0.

## Functions

| Function | Result |
| --- | --- |
| `bridge_native_take_record` | Checks the level and the ring for `bridge_native_log` and `bridge_native_say`, and fills the record from the header. Gives the record, or null when the line is not written. |
| `bridge_native_log` | Writes a line into the module's ring, when its level is kept. |
| `bridge_native_say` | Makes the text of a line from a format and its values, and writes the line, when its level is kept. |
