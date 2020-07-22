#include "evdev.h"
#if USE_EVDEV != 0

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <libevdev/libevdev.h>

#define FUDGE_FACTOR 0.5

/**
 * Read: https://www.kernel.org/doc/Documentation/input/event-codes.txt
 */

int map(int x, int in_min, int in_max, int out_min, int out_max);
inline static int clamp(int value, int min, int max);
evdev_drv_instance* evdev_drv_instance_new(void);
void evdev_drv_instance_destroy(evdev_drv_instance* instance);

evdev_drv_instance* evdev_drv_instance_new()
{
	evdev_drv_instance* instance = (evdev_drv_instance*) malloc(sizeof (evdev_drv_instance));
	memset(instance, 0, sizeof(evdev_drv_instance));

	return instance;
}

void evdev_drv_instance_destroy(evdev_drv_instance* instance)
{
	free(instance);
}

/* WARNING: This is a bad implementation that should instead rely on `udev_device_get_property_value`.
 * Why isn't it? First of all... C is annoying to work  with.
 * But that's not the real reason.
 * All udev implementations fail to cross-compile using pkgsStatic. So we can't rely on udev.
 * If we could we would instead rely on the libinput driver for LVGL.
 * (Though that driver has warts that need fixing.)
 *
 * So, instead of doing the right thing, we rely on the return value of an external command.
 * As it is, it should be safe, as we are globbing a root-controlled location.
 * Though, a stray "'" in a filename there invites hackers into this process.
 *
 * Ugh. If *at least* a `shellwords` like escaping was available in C, or some
 * kind of `gsub` or character-based replace...
 */
static bool evdev_drv_device_is_touchpad(char* dev_name)
{
	char cmd[1024];
	snprintf(cmd, 1024, "udevadm info '%s' | grep 'ID_INPUT_TOUCHPAD=1$' > /dev/null", dev_name);

	/* :( */
	return system(cmd) == 0;
}

/* See the previous function's comment.
 * Same pitfalls, same reasoning.
 */
static bool evdev_drv_device_is_touchscreen(char* dev_name)
{
	char cmd[1024];
	snprintf(cmd, 1024, "udevadm info '%s' | grep 'ID_INPUT_TOUCHSCREEN=1$' > /dev/null", dev_name);

	/* :( */
	return system(cmd) == 0;
}

/* See the previous function's comment.
 * Same pitfalls, same reasoning.
 */
static bool evdev_drv_device_is_mouse(char* dev_name)
{
	char cmd[1024];
	snprintf(cmd, 1024, "udevadm info '%s' | grep 'ID_INPUT_MOUSE=1$' > /dev/null", dev_name);

	/* :( */
	return system(cmd) == 0;
}

/* See the previous function's comment.
 * Same pitfalls, same reasoning.
 */
static bool evdev_drv_device_is_keyboard(char* dev_name)
{
	char cmd[1024];
	// gpio-keys is ID_INPUT_KEY, while ID_INPUT_KEYBOARD applies to keyboards.
	snprintf(cmd, 1024, "udevadm info '%s' | grep 'ID_INPUT_KEY\\(BOARD\\)\\?=1$' > /dev/null", dev_name);

	/* :( */
	return system(cmd) == 0;
}

/**
 * Initialize the evdev interface
 */
evdev_drv_instance* evdev_init(char* dev_name)
{

	int evdev_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if(evdev_fd == -1) {
		printf("WARNING: Unable to open %s.\n", dev_name);
		perror(" error");
		return NULL;
	}

	evdev_drv_instance* instance = evdev_drv_instance_new();

	fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

	instance->lv_indev_drv_type = LV_INDEV_TYPE_POINTER;

	instance->evdev_fd = evdev_fd;
	instance->evdev_root_x = 0;
	instance->evdev_root_y = 0;
	instance->evdev_key_val = 0;
	instance->evdev_button = LV_INDEV_STATE_REL;
	instance->evdev_mt_slot = 0;
	instance->evdev_abs_x_min = 0;
	instance->evdev_abs_y_min = 0;
	instance->evdev_abs_x_max = 0;
	instance->evdev_abs_y_max = 0;
	instance->is_touchpad = false;

	struct libevdev *evdev = libevdev_new();
	libevdev_set_fd(evdev, evdev_fd);

	instance->evdev_abs_x_min = libevdev_get_abs_minimum(evdev, ABS_X);
	instance->evdev_abs_y_min = libevdev_get_abs_minimum(evdev, ABS_Y);
	instance->evdev_abs_x_max = libevdev_get_abs_maximum(evdev, ABS_X);
	instance->evdev_abs_y_max = libevdev_get_abs_maximum(evdev, ABS_Y);

	printf(
			"Device %s\n  - x_min: %d, x_max: %d\n  - y_min: %d, y_max: %d\n",
			libevdev_get_name(evdev),
			instance->evdev_abs_x_min,
			instance->evdev_abs_x_max,
			instance->evdev_abs_y_min,
			instance->evdev_abs_y_max
		  );

	if (evdev_drv_device_is_keyboard(dev_name)) {
		instance->lv_indev_drv_type = LV_INDEV_TYPE_KEYBOARD;
		instance->is_keyboard = true;
		printf("  - is a keyboard\n");
	}

	// If a device is a keyboard *and* one of the following, for now
	// we're assuming the pointer-type input is more important.
	// The solution (FIXME) will be to handle all event types regardless
	// of the lv_indev_drv_type.

	if (evdev_drv_device_is_touchscreen(dev_name)) {
		instance->lv_indev_drv_type = LV_INDEV_TYPE_POINTER;
		instance->is_touchscreen = true;
		printf("  - is a touchscreen\n");
	}

	if (evdev_drv_device_is_touchpad(dev_name)) {
		instance->lv_indev_drv_type = LV_INDEV_TYPE_POINTER;
		instance->is_touchpad = true;
		printf("  - is a touchpad\n");
	}

	if (evdev_drv_device_is_mouse(dev_name)) {
		instance->lv_indev_drv_type = LV_INDEV_TYPE_POINTER;
		instance->is_mouse = true;
		printf("  - is a mouse\n");
	}

	return instance;
}

/**
 * Completely bypass the usual evdev input handling.
 * Emulating a touchpad across the existing mess is not working well.
 *
 * @return false: because the points are not buffered, so no more data to be read
 */
static bool emulate_touchpad(lv_indev_drv_t * drv, lv_indev_data_t * data, evdev_drv_instance * instance) {
	int evdev_fd = instance->evdev_fd;
	struct input_event in;

	int delta_x = 0;
	int delta_y = 0;

	int new_abs_x = instance->evdev_root_x;
	int new_abs_y = instance->evdev_root_y;
	bool new_touch = instance->is_touched;

	while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
		switch (in.type) {
			case EV_SYN:
				if (in.code == SYN_REPORT) {
					// Is this the continuation from a touch?
					if (instance->is_touched == new_touch) {
						delta_x += new_abs_x - instance->evdev_root_x;
						delta_y += new_abs_y - instance->evdev_root_y;
					}
					instance->evdev_root_x = new_abs_x;
					instance->evdev_root_y = new_abs_y;
					instance->is_touched = new_touch;
				}
				break;
			case EV_ABS:
				switch (in.code) {
					case ABS_X:
						new_abs_x = in.value;
						break;
					case ABS_Y:
						new_abs_y = in.value;
						break;
					default:
						break;
				}
				break;
			case EV_KEY:
				switch (in.code) {
					case BTN_LEFT:
						if (in.value == 0) {
							instance->evdev_button = LV_INDEV_STATE_REL;
						}
						else if (in.value == 1) {
							instance->evdev_button = LV_INDEV_STATE_PR;
						}

						break;
					case BTN_TOUCH:
						if (in.value == 0) {
							new_touch = false;
						}
						else if (in.value == 1) {
							new_touch = true;
						}
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}

	// TODO: We probably want to use evdev_abs_*_max to make all touchpad act
	// more alike... right now our delta is based on the implementation detail
	// of the specific touchpad being used.

	data->point.x += delta_x * FUDGE_FACTOR;
	data->point.y += delta_y * FUDGE_FACTOR;
	data->point.x = clamp(data->point.x, 0, lv_disp_get_hor_res(drv->disp) - 1);
	data->point.y = clamp(data->point.y, 0, lv_disp_get_ver_res(drv->disp) - 1);

	// At each loop the state of the button needs to be refreshed.
	data->state = instance->evdev_button;

	return false;
}

/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool evdev_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
	evdev_drv_instance* instance = drv->user_data;
	if (instance->is_touchpad) {
		return emulate_touchpad(drv, data, instance);
	}

	struct input_event in;
	int evdev_fd = instance->evdev_fd;

	while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
		if(in.type == EV_REL) {
			if(in.code == REL_X)
#if EVDEV_SWAP_AXES
				instance->evdev_root_y += in.value;
#else
			instance->evdev_root_x += in.value;
#endif
			else if(in.code == REL_Y)
#if EVDEV_SWAP_AXES
				instance->evdev_root_x += in.value;
#else
			instance->evdev_root_y += in.value;
#endif
		} else if(in.type == EV_ABS) {
			if (in.code == ABS_MT_SLOT) {
				instance->evdev_mt_slot = in.value;
			}
			else {
				// First, let's ignore additional tracking slots.
				// We only track the first touch.
				if (instance->evdev_mt_slot != 0) {
					// Skip additional data
					return true;
				}
				else if (in.code == ABS_X) {
#				if EVDEV_SWAP_AXES
					instance->evdev_root_y = in.value;
#				else
					instance->evdev_root_x = in.value;
#				endif
				}
				else if (in.code == ABS_Y) {
#				if EVDEV_SWAP_AXES
					instance->evdev_root_x = in.value;
#				else
					instance->evdev_root_y = in.value;
#				endif
				}
				else if (in.code == ABS_MT_POSITION_X) {
#				if EVDEV_SWAP_AXES
					instance->evdev_root_y = in.value;
#				else
					instance->evdev_root_x = in.value;
#				endif
				}
				else if (in.code == ABS_MT_POSITION_Y) {
#				if EVDEV_SWAP_AXES
					instance->evdev_root_x = in.value;
#				else
					instance->evdev_root_y = in.value;
#				endif
				}
				else if (in.code == ABS_MT_TRACKING_ID) {
					// Was the finger released?
					if (in.value == -1) {
						instance->evdev_button = LV_INDEV_STATE_REL;
					}
					else {
						instance->evdev_button = LV_INDEV_STATE_PR;
					}
				}
#if 0
				else {
					printf("WARNING: EV_ABS not handled %d \n", in.code);
				}
#endif
			}
		} else if(in.type == EV_KEY) {
			if(in.code == BTN_MOUSE || in.code == BTN_TOUCH || in.code == BTN_LEFT) {
				if(in.value == 0)
					instance->evdev_button = LV_INDEV_STATE_REL;
				else if(in.value == 1)
					instance->evdev_button = LV_INDEV_STATE_PR;
			} else if(drv->type == LV_INDEV_TYPE_KEYBOARD) {
				data->state = (in.value) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
				switch(in.code) {
					case KEY_BACKSPACE:
						data->key = LV_KEY_BACKSPACE;
						break;
					case KEY_ENTER:
						data->key = LV_KEY_ENTER;
						break;
					case KEY_TAB:
						data->key = LV_KEY_NEXT;
						break;

					// Phone compat
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
						data->key = 0;
						break;
				}
				instance->evdev_key_val = data->key;
				instance->evdev_button = data->state;
				return false;
			}
		}
	}

	if(drv->type == LV_INDEV_TYPE_KEYBOARD) {
		/* No data retrieved */
		data->key = instance->evdev_key_val;
		data->state = instance->evdev_button;
		return false;
	}
	if(drv->type != LV_INDEV_TYPE_POINTER)
		return false;
	/*Store the collected data*/

	/* There may be no abs max. */
	if (instance->evdev_abs_x_max == 0) {
		/* Use the values directly */
		data->point.x = instance->evdev_root_x;
		data->point.y = instance->evdev_root_y;
	}
	else {
		/* Otherwise map */
		data->point.x = map(instance->evdev_root_x, instance->evdev_abs_x_min, instance->evdev_abs_x_max, 0, lv_disp_get_hor_res(drv->disp));
		data->point.y = map(instance->evdev_root_y, instance->evdev_abs_y_min, instance->evdev_abs_y_max, 0, lv_disp_get_ver_res(drv->disp));
	}

	data->state = instance->evdev_button;

	if(data->point.x < 0)
		data->point.x = 0;
	if(data->point.y < 0)
		data->point.y = 0;
	if(data->point.x >= lv_disp_get_hor_res(drv->disp))
		data->point.x = lv_disp_get_hor_res(drv->disp) - 1;
	if(data->point.y >= lv_disp_get_ver_res(drv->disp))
		data->point.y = lv_disp_get_ver_res(drv->disp) - 1;

	return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline static int clamp(int value, int min, int max)
{
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}
	return value;
}
#endif
