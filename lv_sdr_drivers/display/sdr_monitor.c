#include "sdr_monitor.h"

#if USE_MONITOR

// Obviously "unset" values, that are not zeroes.
int monitor_height = 480;
int monitor_width = 640;

void monitor_set_resolution(lv_disp_drv_t* disp_drv)
{
	// HACK: we should actually query SDL.
	disp_drv->hor_res = MONITOR_HOR_RES;
	disp_drv->ver_res = MONITOR_VER_RES;
}

#endif
