#include <glob.h>
#include <stdio.h>
#include <sys/time.h>
#include "lvgl/lvgl.h"
#include "lv_drv_conf.h"

#include "hal.h"

LV_IMG_DECLARE(lvgui_cursor);
LV_IMG_DECLARE(lvgui_touch);

mn_hal_default_font_t mn_hal_default_font;
int mn_hal_default_dpi;
lv_disp_drv_t disp_drv;
static lv_obj_t * lvgui_cursor_obj;
static lv_obj_t * lvgui_touch_obj;
static lv_group_t * lvgui_focus_group;

void hal_preinit(void);
void hal_set_dpi(void);
void hal_setup_display(void);

#if USE_FBDEV
#	include "lv_drivers/display/fbdev.h"
#endif

#if USE_DRM
#	include "lv_drivers/display/drm.h"
#endif

#if USE_EVDEV
#	include "lv_drivers/indev/evdev.h"
#endif

#if USE_LIBINPUT
#	include "lv_drivers/indev/libinput_drv.h"
#endif

#if USE_MONITOR
#	include "lv_drivers/display/monitor.h"
#endif

#if USE_MOUSE
#	include "lv_drivers/indev/mouse.h"
#endif

#if USE_KEYBOARD
#	include "lv_drivers/indev/keyboard.h"
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

	// Configure the flush callback first...
#if USE_FBDEV
	disp_drv.flush_cb = fbdev_flush;
#endif
#if USE_DRM
	disp_drv.flush_cb = drm_flush;
#endif
#if USE_MONITOR
    disp_drv.flush_cb = monitor_flush;
#endif

	// As it's possible the driver re-configures it...
#if USE_FBDEV
	fbdev_init(&disp_drv);
#endif
#if USE_DRM
	drm_init(&disp_drv);
#endif
#if USE_MONITOR
    monitor_init();
    monitor_set_resolution(&disp_drv);
#endif
}

#if USE_EVDEV
static void init_evdev(char* name)
{
	evdev_drv_instance* instance = evdev_init(name);

	if (instance == NULL) {
		return;
	}

	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);

	indev_drv.type = instance->lv_indev_drv_type;
	indev_drv.read_cb = evdev_read;
	indev_drv.user_data = instance;
	lv_indev_t * indev = lv_indev_drv_register(&indev_drv);

	// Add a "regular" cursor for touchpads and mice.
	if (instance->is_mouse || instance->is_touchpad) {
		lv_indev_set_cursor(indev, lvgui_cursor_obj);
		lv_obj_set_hidden(lvgui_cursor_obj, true);
	}

	// For touchscreen, a helpful indicator of where the touch happens.
	// This will be useful to detect display orientation or calibration that
	// does not match with the expected.
	if (instance->is_touchscreen) {
		// The cursor should be offset so the center of the cursor is the
		// x,y point of the touch.
		indev->cursor_offset.x = -1 * lvgui_touch.header.w/2;
		indev->cursor_offset.y = -1 * lvgui_touch.header.h/2;

		lv_indev_set_cursor(indev, lvgui_touch_obj);
		// Start hidden, there may be a touchscreen that never gets used.
		// Additionally helps with "one-shot" uses like for splash screens.
		lv_obj_set_hidden(lvgui_touch_obj, true);

		// Setup an animation to "unclutter" (make the cursor disappear)
		lv_anim_t * a;
		a = lv_mem_alloc(sizeof(lv_anim_t));
		indev->cursor_unclutter_animation = a;
		lv_anim_init(a);
		lv_anim_set_exec_cb(a, lvgui_touch_obj, (lv_anim_exec_xcb_t)lv_obj_set_opa_scale);
		// 400ms after the move, take 500ms to disappear.
		lv_anim_set_time(a, 300, 500);
		lv_anim_set_values(a, 255, 0);
		lv_anim_set_path_cb(a, lv_anim_path_ease_in);
	}

	// Link the input device to the main focus group.
	lv_indev_set_group(indev, lvgui_focus_group);
}
#endif

#if USE_LIBINPUT
static void hal_add_libinput_device(char* dev_path)
{
	libinput_drv_instance* instance = libinput_init_drv(dev_path);

	if (instance == NULL) {
		return;
	}

	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);

	indev_drv.type = instance->lv_indev_drv_type;
	indev_drv.read_cb = libinput_read;
	indev_drv.user_data = instance;
	lv_indev_t * indev = lv_indev_drv_register(&indev_drv);

	// Add a "regular" cursor for touchpads and mice.
	if (instance->is_pointer) {
		lv_indev_set_cursor(indev, lvgui_cursor_obj);
		lv_obj_set_hidden(lvgui_cursor_obj, true);
	}

	// For touchscreen, a helpful indicator of where the touch happens.
	// This will be useful to detect display orientation or calibration that
	// does not match with the expected.
	if (instance->is_touchscreen) {
		// The cursor should be offset so the center of the cursor is the
		// x,y point of the touch.
		indev->cursor_offset.x = -1 * lvgui_touch.header.w/2;
		indev->cursor_offset.y = -1 * lvgui_touch.header.h/2;

		lv_indev_set_cursor(indev, lvgui_touch_obj);
		// Start hidden, there may be a touchscreen that never gets used.
		// Additionally helps with "one-shot" uses like for splash screens.
		lv_obj_set_hidden(lvgui_touch_obj, true);

		// Setup an animation to "unclutter" (make the cursor disappear)
		lv_anim_t * a;
		a = lv_mem_alloc(sizeof(lv_anim_t));
		indev->cursor_unclutter_animation = a;
		lv_anim_init(a);
		lv_anim_set_exec_cb(a, lvgui_touch_obj, (lv_anim_exec_xcb_t)lv_obj_set_opa_scale);
		// 400ms after the move, take 500ms to disappear.
		lv_anim_set_time(a, 300, 500);
		lv_anim_set_values(a, 255, 0);
		lv_anim_set_path_cb(a, lv_anim_path_ease_in);
	}

	// Link the input device to the main focus group.
	lv_indev_set_group(indev, lvgui_focus_group);
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

	{
	// Prepare the "main" focus group
	lvgui_focus_group = lv_group_create();
	// By default, clicking will not change the focus.
	// This is so the unsightly focus mark does not show up uselessly
	// on touch devices without keyboards...
	// Anyways, the focus mark is not the main way to use the apps.
	lv_group_set_click_focus(lvgui_focus_group, false);
	}

	{
	lvgui_cursor_obj = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(lvgui_cursor_obj, &lvgui_cursor);
	lv_obj_set_click(lvgui_cursor_obj, false);
	// Hide by default
	// Un-hide when used.
	lv_obj_set_hidden(lvgui_cursor_obj, true);
	}

	{
	lvgui_touch_obj = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(lvgui_touch_obj, &lvgui_touch);
	lv_obj_set_click(lvgui_touch_obj, false);
	// Hide by default
	// Un-hide when used.
	lv_obj_set_hidden(lvgui_touch_obj, true);
	lv_obj_set_opa_scale_enable(lvgui_touch_obj, true);
	}

#if USE_EVDEV
	{
		char **filename;
		size_t cnt;
		glob_t globbuf;
		glob("/dev/input/event*", 0, NULL, &globbuf);
		for (filename = globbuf.gl_pathv, cnt = globbuf.gl_pathc; cnt; filename++, cnt--) {
			init_evdev(*filename);
		}
	}
#endif

#if USE_LIBINPUT
	{
		char **dev_path;
		size_t cnt;
		glob_t globbuf;
		glob("/dev/input/event*", 0, NULL, &globbuf);
		for (dev_path = globbuf.gl_pathv, cnt = globbuf.gl_pathc; cnt; dev_path++, cnt--) {
			hal_add_libinput_device(*dev_path);
		}
	}
#endif

#if USE_MOUSE
    mouse_init();
	{
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
	lv_indev_t * indev = lv_indev_drv_register(&indev_drv);
	// Link the input device to the main focus group.
	lv_indev_set_group(indev, lvgui_focus_group);
	}
#endif
#if USE_KEYBOARD
    keyboard_init();
	{
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYBOARD;
    indev_drv.read_cb = keyboard_read;
	lv_indev_t * indev = lv_indev_drv_register(&indev_drv);
	// Link the input device to the main focus group.
	lv_indev_set_group(indev, lvgui_focus_group);
	}
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

lv_group_t * lvgui_get_focus_group()
{
	return lvgui_focus_group;
}

void lvgui_style_mod_noop(struct _lv_group_t *g, lv_style_t *t)
{
}

void lvgui_focus_ring_disable()
{
	lv_group_set_style_mod_cb(lvgui_focus_group, lvgui_style_mod_noop);
	lv_group_set_style_mod_edit_cb(lvgui_focus_group, lvgui_style_mod_noop);
}
