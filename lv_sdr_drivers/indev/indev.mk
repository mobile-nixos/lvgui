CSRCS += sdr_evdev.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_sdr_drivers/indev
VPATH += :$(LVGL_DIR)/lv_sdr_drivers/indev

CFLAGS += "-I$(LVGL_DIR)/lv_sdr_drivers/indev"
