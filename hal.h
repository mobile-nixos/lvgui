#ifndef HAL_H
#define HAL_H

void hal_init(void);
lv_group_t * lvgui_get_focus_group();
void lvgui_focus_ring_disable();
lv_disp_drv_t disp_drv;

#endif
