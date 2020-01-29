/**
 * @file fbdev.h
 *
 */

#ifndef SDR_FBDEV_H
#define SDR_FBDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#if USE_FBDEV

#include "lv_drivers/display/fbdev.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

void fbdev_set_resolution(lv_disp_drv_t* disp_drv);

#endif  /*USE_FBDEV*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*SDR_FBDEV_H*/
