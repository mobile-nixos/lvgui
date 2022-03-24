/**
 * @file drm.h
 *
 */

#ifndef DRM_H
#define DRM_H

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

#if USE_DRM

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void drm_init(lv_disp_drv_t* drv);
void drm_exit(void);
void drm_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

enum drm_orientation {
	DRM_ORIENTATION_NORMAL,
	DRM_ORIENTATION_UPSIDE_DOWN,
	DRM_ORIENTATION_CLOCKWISE,
	DRM_ORIENTATION_COUNTER_CLOCKWISE,
};
typedef enum drm_orientation drm_orientation_t;

/**
 * Input drivers should use this to determine how to rotate direct input devices.
 */
extern drm_orientation_t drm_display_orientation;

#endif  /*USE_DRM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DRM_H*/
