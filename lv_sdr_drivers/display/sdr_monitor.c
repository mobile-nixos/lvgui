#include "sdr_monitor.h"

#if USE_MONITOR

void monitor_set_resolution(lv_disp_drv_t* disp_drv)
{
	// HACK: we should actually query SDL.
	disp_drv->hor_res = MONITOR_HOR_RES;
	disp_drv->ver_res = MONITOR_VER_RES;
}

#endif
