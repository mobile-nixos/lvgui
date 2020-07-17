"`lvgui`"
=========

This is a fork of some parts of the project previously known as LittleVGL.

The main reason for the fork is to take ownership of the code as some of my
requirements do not match with the design goals of the upstream project.

This fork aims to support **only** non-embedded Linux-based systems.

The main goals are:

 - Keep the output binaries and dependencies small
 - Work on the framebuffer and a "desktop" implementation
 - First class support for keyboard, mouse and touch input

Additionally, these are foundational blocks used by an mruby implementation of
a more in-depth toolkit. Compared to LittleVGL, some of the more complex parts
and interactions will be dropped from the native code library and pushed into
the scripted layer.

## Acknowledgements

This is originally based on the [LittleVGL](https://github.com/lvgl/lvgl)
project version 6.
