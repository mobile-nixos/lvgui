#include "lvgl/lvgl.h"
#include "lv_drv_conf.h"

#include "introspection.h"

#if LVGL_ENV_SIMULATOR
#define IS_SIMULATOR true
#else
#define IS_SIMULATOR false
#endif

bool lv_introspection_is_simulator(void) {
	return IS_SIMULATOR;
}

bool lv_introspection_is_debug(void) {
	return LV_USE_DEBUG;
}

bool lv_introspection_use_assert_style(void) {
	return LV_USE_ASSERT_STYLE;
}
