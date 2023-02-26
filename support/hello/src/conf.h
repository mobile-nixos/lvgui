#ifndef HELLO_CONF_H
#define HELLO_CONF_H

#include <lvgl/lvgl.h>
#include <lv_drv_conf.h>

#if LVGL_ENV_SIMULATOR == 1
#ifndef HELLO_ASSETS_PATH
#define HELLO_ASSETS_PATH "./"
#endif
#endif

#ifndef HELLO_ASSETS_PATH
#error HELLO_ASSETS_PATH must be defined
#endif

#define FRAME_RATE 1 / 60
#define SECOND_AS_MICROSECONDS 1000000

#endif
