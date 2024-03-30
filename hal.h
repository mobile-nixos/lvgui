#ifndef HAL_H
#define HAL_H

/*
 * Same semantics as drm_orientation_t.
 */
enum {
	HAL_PANEL_ORIENTATION_NORMAL    = 0,
	HAL_PANEL_ORIENTATION_BOTTOM_UP = 1,
	HAL_PANEL_ORIENTATION_RIGHT_UP  = 2,
	HAL_PANEL_ORIENTATION_LEFT_UP   = 3,
};
typedef uint8_t hal_panel_orientation_t;

void hal_init(const char* asset_path);
lv_group_t * lvgui_get_focus_group();
void lvgui_focus_ring_disable();
char * hal_asset_path(const char* asset_path);
hal_panel_orientation_t hal_get_panel_orientation() ;

extern lv_disp_drv_t disp_drv;
extern int mn_hal_default_dpi;
extern mn_hal_default_font_t mn_hal_default_font;

#endif
