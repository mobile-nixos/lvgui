CSRCS += sdr_fbdev.c
CSRCS += sdr_monitor.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_sdr_drivers/display
VPATH += :$(LVGL_DIR)/lv_sdr_drivers/display

CFLAGS += "-I$(LVGL_DIR)/lv_sdr_drivers/display"
