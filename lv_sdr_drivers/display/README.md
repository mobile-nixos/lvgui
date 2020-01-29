Display driver changes
======================

I'm adding a function to each driver that will setup the resolution
dynamically. This is used to remove the reliance on hard-coded values in the
drivers, leading to the inability to re-use the same compilation on different
devices.

Ideally, this is something that display drivers upstream should do, setting
the diplay dimensions.

Monitor (SDL)
-------------

For now, this is done through directly re-using the hardcoded defines for the
monitor driver.

A more proper approach would be to query SDL. Combined with an hypothetical way
to dynamically set the window size at launch, this would allow easier testing
of different sizes.

