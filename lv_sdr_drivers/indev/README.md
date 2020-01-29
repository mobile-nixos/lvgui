Input driver changes
====================

evdev
-----

This is a bit more invasive. This driver is an entire copy, it wouldn't have
been trivial to add "patch like" functions.

### Multi-instances

The driver has lost "global state". It now relies on userdata to keep the state.

This userdata holds the state for one event source. This allows us to open all
event sources rather than being able to only open one.

Opening all event sources allows us not to care *which* input driver handles
the touch screen, which is a bit more specific to our use.

### "Other" touchscreen scheme

There are two touch input implementation supported by the kernel. Only the one
of those was supported by the current driver. On my test devices, 2 out of 4
would only report cursor move events.

We are using the "new tracking ID" and "removed tracking ID" events as touch
and release events. Coupled with ignoring any touches other than the first,
this gives us sufficient input for pointer-type usage.
