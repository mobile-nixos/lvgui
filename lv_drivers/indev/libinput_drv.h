#ifndef LVGL_LIBINPUT_H
#define LVGL_LIBINPUT_H

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

#if USE_LIBINPUT

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include <libinput.h>
#include <poll.h>

/*********************
 *      DEFINES
 *********************/

typedef struct {
	// Configure this input driver to be a specific LVGL input type.
	int lv_indev_drv_type;

	// The lvgl input device
	lv_indev_t * indev;

	// One context per input device.
	// This is because LVGL's input handling wants to *own* one loop per
	// device; sharing context means sharing input FDs.
	struct libinput *libinput_context;

	// The "tangible" libinput opaque type.
	struct libinput_device *libinput_device;

	// The file descriptors to poll() on
	struct pollfd fds[1];

	// Properties of the input, shouldn't change.
	bool is_pointer;
	bool is_touchscreen;
	bool is_keyboard;

	// Starting from here, properties used to keep track of the state between
	// calls.

	// Pressed or released
	int state;
	// Keyboard key being pressed (or released)
	int key;
	// Coordinates for a pointer
	double root_x;
	double root_y;
} libinput_drv_instance;

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*libinput_drv_add_cb_t)(libinput_drv_instance* instance);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Starts the libinput driver with the usual defaults.
 */
void libinput_drv_init();

/**
 * Initialize a new libinput device instance
 */
libinput_drv_instance* libinput_init_drv(char* dev_name);

/**
 * Get the current position and state of the libinput
 * @param indev_drv driver object itself
 * @param data store the libinput data here
 */
bool libinput_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);


/**********************
 *      MACROS
 **********************/

#endif /* USE_LIBINPUT */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVGL_LIBINPUT_H */
