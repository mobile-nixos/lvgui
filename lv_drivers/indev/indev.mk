CSRCS += keyboard.c
CSRCS += mouse.c
CSRCS += mousewheel.c

DEPPATH += --dep-path $(LVGL_DIR)/lv_drivers/indev
VPATH += :$(LVGL_DIR)/lv_drivers/indev

CFLAGS += "-I$(LVGL_DIR)/lv_drivers/indev"
