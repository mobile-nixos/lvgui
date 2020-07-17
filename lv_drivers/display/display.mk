CSRCS += fbdev.c
CSRCS += monitor.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_drivers/display
VPATH += :$(LVGL_DIR)/lv_drivers/display

CFLAGS += "-I$(LVGL_DIR)/lv_drivers/display"
