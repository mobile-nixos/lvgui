#include "lvgl/lvgl.h"
#include "lv_drv_conf.h"

#include "introspection.h"
#include "hal.h"

#if USE_FBDEV || USE_DRM
#	include "lv_drivers/display/fbdev.h"
#endif

#if USE_DRM
#	include "lv_drivers/display/drm.h"
#endif

#if USE_MONITOR
#	include "lv_drivers/display/monitor.h"
#endif

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

const char * lv_introspection_display_driver(void)
{
#if USE_FBDEV || USE_DRM
	if (disp_drv.flush_cb == fbdev_flush)
		return "fbdev";
#endif
#if USE_DRM
	if (disp_drv.flush_cb == drm_flush)
		return "drm";
#endif
#if USE_MONITOR
	if (disp_drv.flush_cb == monitor_flush)
		return "monitor";
#endif
	return "unknown";
}
