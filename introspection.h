#ifndef INTROSPECTION_H
#define INTROSPECTION_H

/*
 * This defines a couple functions allowing a non-native binding (e.g. ffi) to
 * know more about the compilation options.
 */

bool lv_introspection_is_simulator(void);
bool lv_introspection_is_debug(void);
bool lv_introspection_use_assert_style(void);
const char * lv_introspection_display_driver(void);

#endif
