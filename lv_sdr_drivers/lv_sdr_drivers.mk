include $(LVGL_DIR)/lv_sdr_drivers/display/display.mk
include $(LVGL_DIR)/lv_sdr_drivers/indev/indev.mk

DEPPATH += --dep-path $(LVGL_DIR)/lv_drivers
VPATH += :$(LVGL_DIR)/lv_sdr_drivers

CFLAGS += "-I$(LVGL_DIR)/lv_sdr_drivers"
