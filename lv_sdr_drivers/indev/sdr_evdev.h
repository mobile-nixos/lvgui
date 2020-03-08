/**
 * @file evdev.h
 *
 */

#ifndef SDR_EVDEV_H
#define SDR_EVDEV_H

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

#if USE_SDR_EVDEV

#ifndef EVDEV_SWAP_AXES
#define EVDEV_SWAP_AXES 0
#endif
#ifndef EVDEV_SCALE
#define EVDEV_SCALE 0
#endif
#ifndef EVDEV_CALIBRATE
#define EVDEV_CALIBRATE 0
#endif
#ifndef EVDEV_DRV_MAX_EVENTS
#define EVDEV_DRV_MAX_EVENTS 10
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

typedef struct {
	int lv_indev_drv_type;
	int evdev_fd;
	int evdev_root_x;
	int evdev_root_y;
	int evdev_button;
	int evdev_key_val;
	int evdev_mt_slot;

	int evdev_abs_x_min;
	int evdev_abs_y_min;
	int evdev_abs_x_max;
	int evdev_abs_y_max;
	bool is_mouse;
	bool is_touchscreen;
	bool is_touchpad;

	// Used only by touchpad emulation.
	bool is_touched;
} evdev_drv_instance;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the evdev
 */
evdev_drv_instance* evdev_init(char* dev_name);
/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool evdev_read(lv_indev_drv_t * drv, lv_indev_data_t * data);


/**********************
 *      MACROS
 **********************/

#endif /* USE_SDR_EVDEV */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SDR_EVDEV_H */
