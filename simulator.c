#include "lvgl/lvgl.h"
#include "lv_drv_conf.h"

#include "simulator.h"

#if LVGL_ENV_SIMULATOR
#define IS_SIMULATOR true
#else
#define IS_SIMULATOR false
#endif

bool lv_is_simulator(void) {
	return IS_SIMULATOR;
}
