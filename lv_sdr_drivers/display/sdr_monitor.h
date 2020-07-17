#ifndef SDR_MONITOR_H
#define SDR_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#if USE_MONITOR

#include "lv_drivers/display/monitor.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

void monitor_set_resolution(lv_disp_drv_t* disp_drv);

#endif /* USE_MONITOR */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SDR_MONITOR_H */
