#ifndef HAL_H
#define HAL_H

void hal_init(const char* asset_path);
lv_group_t * lvgui_get_focus_group();
void lvgui_focus_ring_disable();
lv_disp_drv_t disp_drv;
int mn_hal_default_dpi;
char * hal_asset_path(const char* asset_path);

#endif
