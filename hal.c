#include <stdio.h>
#include <sys/time.h>
#include "lvgl/lvgl.h"
#include "lv_drv_conf.h"

#include "hal.h"

mn_hal_default_font_t mn_hal_default_font;
int mn_hal_default_dpi;
lv_disp_drv_t disp_drv;

void hal_preinit(void);
void hal_set_dpi(void);
void hal_setup_display(void);

#if USE_FBDEV
#	include "lv_sdr_drivers/display/sdr_fbdev.h"
#endif

#if USE_SDR_EVDEV
#	include "lv_sdr_drivers/indev/sdr_evdev.h"
#endif

#if USE_MONITOR
#	include "lv_sdr_drivers/display/sdr_monitor.h"
#endif

#if USE_MOUSE
#	include "lv_drivers/indev/mouse.h"
#endif

#define DISP_BUF_SIZE (80*LV_HOR_RES_MAX)

// WARNING: This is not proper DPI.
// This is the bizarro concept of DPI for lvgl, which is more
// akin to setting *some* scaling, mostly default sizes and paddings.
// Font has to be handled separately from their "DPI".
// Here we're using this to scale the UI better according to the resolution,
// and not according to the device size.
void hal_set_dpi()
{
	// HACK
	// This needs to happen before lv_init.
	// This means we need to figure out the DPI before hal_init :(
	// Default fallback value.
	mn_hal_default_dpi = 100;

	mn_hal_default_dpi = 200*(disp_drv.hor_res)/720;
	printf("HAL DPI: %d\n", mn_hal_default_dpi);

	// Not strictly DPI, but fonts don't actually scale with DPI
	// so we need to handle it ourselves.

	// HACK
	// This will be inherited by the default theme, via styles...
	// This has to be not-NULL before lv_init or else the default theme will fail.
	// It fails due to its use in `lv_core/lv_style.c`

	if (mn_hal_default_dpi < 150) {
		printf("HAL DPI: Using Roboto 22\n");
		mn_hal_default_font = &lv_font_roboto_22;
	}
	else {
		printf("HAL DPI: Using Roboto 28\n");
		mn_hal_default_font = &lv_font_roboto_28;
	}
}

// Initializes the display, but does **not** register
// with lvgl. This will be registered once lvgl is initialized.
// This allows us to use the display driver information for
// things like setting up the DPI.
void hal_setup_display()
{
	/*A small buffer for LittlevGL to draw the screen's content*/
	static lv_color_t buf[DISP_BUF_SIZE];

	/*Initialize a descriptor for the buffer*/
	static lv_disp_buf_t disp_buf;
	lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

	/*Initialize and register a display driver*/
	lv_disp_drv_init(&disp_drv);
	disp_drv.buffer = &disp_buf;


#if USE_FBDEV
	fbdev_init();
    fbdev_set_resolution(&disp_drv);
#endif
#if USE_MONITOR
    monitor_init();
    monitor_set_resolution(&disp_drv);
#endif

#if USE_FBDEV
	disp_drv.flush_cb = fbdev_flush;
#endif
#if USE_MONITOR
    disp_drv.flush_cb = monitor_flush;
#endif
}

#if USE_SDR_EVDEV
static void init_evdev(char* name)
{
	evdev_drv_instance* instance = evdev_init(name);

	if (instance == NULL) {
		return;
	}

	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);

	// FIXME: This assumes only pointers via evdev...
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = evdev_read;
	indev_drv.user_data = instance;
	/*lv_indev_t * indev = */lv_indev_drv_register(&indev_drv);

	// // TODO: Better handle pointer.
	// if (indev_drv.type == LV_INDEV_TYPE_POINTER) {
	// 	lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL);
	// 	// FIXME: stop relying on LV_SYMBOL_POWER and instead on a custom pointer.
	// 	lv_img_set_src(cursor_obj, LV_SYMBOL_POWER);
	// 	lv_indev_set_cursor(indev, cursor_obj);
	// 	lv_obj_set_click(cursor_obj, false);
	// }
}
#endif

void hal_preinit()
{
	hal_setup_display();
	hal_set_dpi();
}

void hal_init(void)
{
	// Do some preliminary hardware initialization.
	// Mostly some initialization with the display that can't be done later.
	hal_preinit();

	// Initialize lvgl
	lv_init();

	// Finish initializing hardware.
	LV_LOG_INFO("HAL begins");

	lv_disp_drv_register(&disp_drv);

#if USE_SDR_EVDEV
	for (int i = 0; i < EVDEV_DRV_MAX_EVENTS; i++) {
		char event_file[256];
		snprintf(event_file, 256, "/dev/input/event%d", i);
		init_evdev(event_file);
	}
#endif

#if USE_MOUSE
    mouse_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
	lv_indev_drv_register(&indev_drv);
#endif

	LV_LOG_INFO("HAL Finished");
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
	static uint64_t start_ms = 0;
	if(start_ms == 0) {
		struct timeval tv_start;
		gettimeofday(&tv_start, NULL);
		start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
	}

	struct timeval tv_now;
	gettimeofday(&tv_now, NULL);
	uint64_t now_ms;
	now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

	uint32_t time_ms = now_ms - start_ms;
	return time_ms;
}
