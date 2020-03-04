#include "sdr_evdev.h"
#if USE_SDR_EVDEV != 0

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <libevdev/libevdev.h>

int map(int x, int in_min, int in_max, int out_min, int out_max);
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

	return instance;
}

/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */
bool evdev_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
	struct input_event in;

	evdev_drv_instance* instance = drv->user_data;
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
			} else if(drv->type == LV_INDEV_TYPE_KEYPAD) {
				data->state = (in.value) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
				switch(in.code) {
					case KEY_BACKSPACE:
						data->key = LV_KEY_BACKSPACE;
						break;
					case KEY_ENTER:
						data->key = LV_KEY_ENTER;
						break;
					case KEY_UP:
						data->key = LV_KEY_UP;
						break;
					case KEY_LEFT:
						data->key = LV_KEY_PREV;
						break;
					case KEY_RIGHT:
						data->key = LV_KEY_NEXT;
						break;
					case KEY_DOWN:
						data->key = LV_KEY_DOWN;
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

	if(drv->type == LV_INDEV_TYPE_KEYPAD) {
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

#endif
