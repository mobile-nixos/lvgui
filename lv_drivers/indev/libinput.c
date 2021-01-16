/*********************
 *      INCLUDES
 *********************/
#include "libinput_drv.h"
#if USE_LIBINPUT != 0

#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <linux/input.h>

#define DRV_DEBUG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int open_restricted(const char *path, int flags, void *user_data);
static void close_restricted(int fd, void *user_data);

/**
 * Handles keyboard inputs
 */
static void libinput_drv_handle_keyboard_input(libinput_drv_instance* instance, struct libinput_event* event, lv_indev_data_t * data);
/**
 * Manages memory allocation for libinput_drv_instance
 */
static libinput_drv_instance* libinput_drv_instance_new();
/**
 * Manages memory deallocation for libinput_drv_instance
 */
static void libinput_drv_instance_destroy(libinput_drv_instance* instance);

/**********************
 *  STATIC VARIABLES
 **********************/

// Application-global variables for libinput.
static const struct libinput_interface drv_libinput_interface = {
  .open_restricted = open_restricted,
  .close_restricted = close_restricted,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * reconfigure the device file for libinput
 * @param dev_name set the libinput device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
bool libinput_set_file(libinput_drv_instance* instance, char* dev_name)
{
	// This check *should* not be necessary, yet applications crashes even on NULL handles.
	// citing libinput.h:libinput_path_remove_device:
	// > If no matching device exists, this function does nothing.
	if (instance->libinput_device) {
		instance->libinput_device = libinput_device_unref(instance->libinput_device);
		libinput_path_remove_device(instance->libinput_device);
	}

	instance->libinput_device = libinput_path_add_device(instance->libinput_context, dev_name);
	if(!instance->libinput_device) {
		perror("unable to add device to libinput context:");
		return false;
	}
	instance->libinput_device = libinput_device_ref(instance->libinput_device);
	if(!instance->libinput_device) {
		perror("unable to reference device within libinput context:");
		return false;
	}

	// Resets some props
	instance->state = LV_INDEV_STATE_REL;
	instance->key = 0;
	instance->root_y = 0;
	instance->root_x = 0;

	return true;
}

libinput_drv_instance* libinput_init_drv(char* dev_name)
{
#ifdef DRV_DEBUG
	printf("[indev/libinput]: Initializing for '%s'...\n", dev_name);
#endif

	// Allocate the instance in memory
	libinput_drv_instance* instance = libinput_drv_instance_new();

	// Assign a fresh context
	instance->libinput_context = libinput_path_create_context(&drv_libinput_interface, NULL);

	if(!libinput_set_file(instance, dev_name)) {
		perror("Unable to add device to libinput context:");
		libinput_drv_instance_destroy(instance);
		return NULL;
	}

	// Add some metadata
	instance->is_pointer = libinput_device_has_capability(instance->libinput_device, LIBINPUT_DEVICE_CAP_POINTER);
	instance->is_touchscreen = libinput_device_has_capability(instance->libinput_device, LIBINPUT_DEVICE_CAP_TOUCH);
	instance->is_keyboard = libinput_device_has_capability(instance->libinput_device, LIBINPUT_DEVICE_CAP_KEYBOARD);

	// The least "important" type of input; will be overwritten if pointer or touchscreen.
	instance->lv_indev_drv_type = LV_INDEV_TYPE_KEYBOARD;
	if (instance->is_touchscreen || instance->is_pointer) {
		instance->lv_indev_drv_type = LV_INDEV_TYPE_POINTER;
	}

#ifdef DRV_DEBUG
	printf("\n");
	printf(
		"> Device *%s* (`%s`):\n",
		libinput_device_get_name(instance->libinput_device),
		libinput_device_get_sysname(instance->libinput_device)
	);
	if (instance->is_pointer)     { printf(">  - is a pointer\n"); }
	if (instance->is_touchscreen) { printf(">  - is a touchscreen\n"); }
	if (instance->is_keyboard)    { printf(">  - is a keyboard\n"); }
	printf("\n");
#endif

	// Prepare file descriptors for polling
	instance->fds[0].fd = libinput_get_fd(instance->libinput_context);
	instance->fds[0].events = POLLIN;
	instance->fds[0].revents = 0;

	fcntl(instance->fds[0].fd, F_SETFL, O_ASYNC | O_NONBLOCK);

#ifdef DRV_DEBUG
	printf("[indev/libinput]: done with '%s'...\n", dev_name);
#endif

	return instance;
}

/**
 * Get the current position and state of the libinput
 * @param indev_drv driver object itself
 * @param data store the libinput data here
 */
bool libinput_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
	struct libinput_event *event = NULL;

	struct libinput_event_pointer *pointer_event = NULL;
	struct libinput_event_touch *touch_event = NULL;
	uint32_t in_button = 0;

	bool changed = false;

	libinput_drv_instance* instance = drv->user_data;

	libinput_dispatch(instance->libinput_context);
	// (maybe) empty the event queue
	// Keyboard events will cause the function to exit.
	while((event = libinput_get_event(instance->libinput_context)) != NULL) {
		enum libinput_event_type type = libinput_event_get_type(event);
		changed = true;

		// -> https://github.com/wayland-project/libinput/blob/f2baea50c01f9e10fb2076f5dd64311347ac5c3e/src/libinput.h#L716-L904
		switch (type) {
			case LIBINPUT_EVENT_TOUCH_MOTION:
			case LIBINPUT_EVENT_TOUCH_DOWN:
				touch_event = libinput_event_get_touch_event(event);
				instance->root_x = libinput_event_touch_get_x_transformed(touch_event, LV_HOR_RES);
				instance->root_y = libinput_event_touch_get_y_transformed(touch_event, LV_VER_RES);
				instance->state = LV_INDEV_STATE_PR;
				break;

			case LIBINPUT_EVENT_TOUCH_UP:
				instance->state = LV_INDEV_STATE_REL;
				break;

			// Stylus hovering, or "drawing tablet", like QEMU
			case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
				pointer_event = libinput_event_get_pointer_event(event);
				instance->root_x = libinput_event_pointer_get_absolute_x_transformed(pointer_event, LV_HOR_RES);
				instance->root_y = libinput_event_pointer_get_absolute_y_transformed(pointer_event, LV_VER_RES);
				break;

			case LIBINPUT_EVENT_POINTER_BUTTON:
				pointer_event = libinput_event_get_pointer_event(event);
				in_button = libinput_event_pointer_get_button(pointer_event);
				if (in_button == BTN_LEFT) {
					if (libinput_event_pointer_get_button(pointer_event) == LIBINPUT_BUTTON_STATE_PRESSED) {
						instance->state = LV_INDEV_STATE_PR;
					}
					else {
						instance->state = LV_INDEV_STATE_REL;
					}
				}
				break;

			case LIBINPUT_EVENT_KEYBOARD_KEY:
				if (instance->lv_indev_drv_type == LV_INDEV_TYPE_KEYBOARD) {
					libinput_drv_handle_keyboard_input(instance, event, data);

					// Whatever happens, we have to send the current key event.
					// Otherwise multiple events in the queue will be lost.
					return true;
				}
				break;

			case LIBINPUT_EVENT_DEVICE_ADDED:
			case LIBINPUT_EVENT_DEVICE_REMOVED:
			case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN:
			case LIBINPUT_EVENT_GESTURE_PINCH_END:
			case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE:
			case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
			case LIBINPUT_EVENT_GESTURE_SWIPE_END:
			case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE:
			case LIBINPUT_EVENT_POINTER_AXIS: // scroll-wheel
			case LIBINPUT_EVENT_SWITCH_TOGGLE:
			case LIBINPUT_EVENT_TABLET_PAD_BUTTON:
			case LIBINPUT_EVENT_TABLET_PAD_KEY:
			case LIBINPUT_EVENT_TABLET_PAD_RING:
			case LIBINPUT_EVENT_TABLET_PAD_STRIP:
			case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
			case LIBINPUT_EVENT_TABLET_TOOL_BUTTON:
			case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
			case LIBINPUT_EVENT_TABLET_TOOL_TIP:
				// no-op
				break;

			case LIBINPUT_EVENT_NONE:
				// DO NOT mask this with DRV_DEBUG
				// It is unexpected, since we're polling
				printf("[indev/libinput]: Unexpected LIBINPUT_EVENT_NONE.\n");
				break;

			case LIBINPUT_EVENT_POINTER_MOTION:
			case LIBINPUT_EVENT_TOUCH_CANCEL:
			case LIBINPUT_EVENT_TOUCH_FRAME:
			default:
				// DO NOT mask this with DRV_DEBUG
				// We want to be verbose on *unhandled* events.
				// If the event does nothing, add it to the no-op list.
				printf("[indev/libinput]: Event type %d not handled.\n", type);
				break;
		}
		libinput_event_destroy(event);
	}

	if (changed) {
		data->state = instance->state;
		data->key = instance->key;
		data->point.x = instance->root_x;
		data->point.y = instance->root_y;

		// Clamp values, mostly useful for relative moves.
		if(data->point.x < 0)
			data->point.x = 0;
		if(data->point.y < 0)
			data->point.y = 0;
		if(data->point.x >= lv_disp_get_hor_res(drv->disp))
			data->point.x = lv_disp_get_hor_res(drv->disp) - 1;
		if(data->point.y >= lv_disp_get_ver_res(drv->disp))
			data->point.y = lv_disp_get_ver_res(drv->disp) - 1;

#ifdef DRV_DEBUG
		printf(
			"[indev/libinput]: lvgl input data: x = %d; y = %4d; state = %4d; key = %3d; // %s\n",
			data->point.x,
			data->point.y,
			data->state,
			data->key,
			libinput_device_get_name(instance->libinput_device)
		);
#endif
	}

	// False because there are no events to handle anymore
	return false;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static int open_restricted(const char *path, int flags, void *user_data)
{
	int fd = open(path, flags);
	return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data)
{
	close(fd);
}

static libinput_drv_instance* libinput_drv_instance_new()
{
	libinput_drv_instance* instance = (libinput_drv_instance*) malloc(sizeof (libinput_drv_instance));
	memset(instance, 0, sizeof(libinput_drv_instance));

	return instance;
}

static void libinput_drv_instance_destroy(libinput_drv_instance* instance)
{
	free(instance);
}

static void libinput_drv_handle_keyboard_input(libinput_drv_instance* instance, struct libinput_event* event, lv_indev_data_t * data)
{
	struct libinput_event_keyboard* keyboard_event = libinput_event_get_keyboard_event(event);
	instance->key = libinput_event_keyboard_get_key(keyboard_event);
	instance->state = libinput_event_keyboard_get_key_state(keyboard_event) == LIBINPUT_KEY_STATE_PRESSED;

	// Update the state for the key
	data->state = instance->state;

	// BUT, we need to map to an *actual* key.
	// Either some internal LVGL key binding
	// OR, using xkbcommon for mapping scancodes to text input.
	switch(instance->key) {
		case KEY_BACKSPACE:
			data->key = LV_KEY_BACKSPACE;
			break;
		case KEY_ENTER:
			data->key = LV_KEY_ENTER;
			break;
		case KEY_TAB:
			data->key = LV_KEY_NEXT;
			break;

		case KEY_DELETE:
			data->key = LV_KEY_DEL;
			break;

		case KEY_HOME:
			data->key = LV_KEY_HOME;
			break;
		case KEY_END:
			data->key = LV_KEY_END;
			break;

		case KEY_RIGHT:
			data->key = LV_KEY_RIGHT;
			break;
		case KEY_LEFT:
			data->key = LV_KEY_LEFT;
			break;
		case KEY_UP:
			data->key = LV_KEY_UP;
			break;
		case KEY_DOWN:
			data->key = LV_KEY_DOWN;
			break;

		// Used for navigating on tablets/phones without involving the display.
		case KEY_VOLUMEUP:
			data->key = LV_KEY_PREV;
			break;
		case KEY_VOLUMEDOWN:
			data->key = LV_KEY_NEXT;
			break;
		case KEY_POWER:
			data->key = LV_KEY_ENTER;
			break;

		default:
			// Here we handle converting to a useful value
			// TODO: handle using xkbcommon
			data->key = instance->key;
			}
			break;
	}
#ifdef DRV_DEBUG
	printf(
		"[indev/libinput]: lvgl input data (keyboard): state = %4d; key = %3d; // %s\n",
		data->state,
		data->key,
		libinput_device_get_name(instance->libinput_device)
	);
#endif

	// Don't forget to cleanup!
	libinput_event_destroy(event);
}

#endif
