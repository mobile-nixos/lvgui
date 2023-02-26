LVGL_ENV_SIMULATOR ?= 1

WARNING_FLAGS ?= \
	-Wall \
	-Wclobbered \
	-Wdeprecated \
	-Wdouble-promotion \
	-Wempty-body \
	-Werror \
	-Wextra \
	-Wformat-security \
	-Wmaybe-uninitialized \
	-Wmissing-prototypes \
	-Wmultichar \
	-Wno-cast-qual \
	-Wno-discarded-qualifiers \
	-Wno-error=cpp \
	-Wno-error=missing-prototypes \
	-Wno-error=pedantic \
	-Wno-error=strict-prototypes \
	-Wno-format-nonliteral \
	-Wno-ignored-qualifiers \
	-Wno-missing-field-initializers \
	-Wno-sign-compare \
	-Wno-switch-default \
	-Wno-unused-function \
	-Wno-unused-parameter \
	-Wno-unused-value \
	-Wpointer-arith \
	-Wreturn-type \
	-Wshift-negative-value \
	-Wsizeof-pointer-memaccess \
	-Wstack-usage=4096 \
	-Wswitch-enum \
	-Wtype-limits \
	-Wundef \
	-Wuninitialized \
	-Wunreachable-code \
	-fno-strict-aliasing \

# Fails on master.
#	-Wshadow \

DEBUG_FLAGS ?= -O3 -g0

CFLAGS += $(WARNING_FLAGS) $(DEBUG_FLAGS)
CFLAGS += -I$(LVGL_DIR)/
CFLAGS += -DLVGL_ENV_SIMULATOR=$(LVGL_ENV_SIMULATOR)
CFLAGS += -fPIC
CFLAGS += -DVERSION=$(VERSION)

REQUIRES = 

PKG_CONFIG ?= pkg-config

LDFLAGS += -lm

ifeq ($(LVGL_ENV_SIMULATOR), 1)
CFLAGS += $(shell $(PKG_CONFIG) --cflags sdl2)
LDFLAGS += $(shell $(PKG_CONFIG) --libs sdl2)
REQUIRES += sdl2
else
CFLAGS += $(shell $(PKG_CONFIG) --cflags libinput)
LDFLAGS += $(shell $(PKG_CONFIG) --libs libinput)
REQUIRES += libinput

CFLAGS += $(shell $(PKG_CONFIG) --cflags libdrm)
LDFLAGS += $(shell $(PKG_CONFIG) --libs libdrm)

LDFLAGS += -lpthread
LDFLAGS += -lxkbcommon
endif

CFLAGS += $(shell $(PKG_CONFIG) --cflags freetype2)
LDFLAGS += $(shell $(PKG_CONFIG) --libs freetype2)

CONFFILES += $(LVGL_DIR)/lv_conf.h $(LVGL_DIR)/lv_drv_conf.h

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/lv_lib_nanosvg/lv_lib_nanosvg.mk
include $(LVGL_DIR)/lv_lib_freetype/lv_lib_freetype.mk

# Additional source files
CSRCS += $(LVGL_DIR)/font.c
CSRCS += $(LVGL_DIR)/hal.c
CSRCS += $(LVGL_DIR)/url.c
CSRCS += $(LVGL_DIR)/introspection.c
CSRCS += $(LVGL_DIR)/artwork/lvgui_cursor.c
CSRCS += $(LVGL_DIR)/artwork/lvgui_touch.c
CSRCS += $(LVGL_DIR)/lvgui_struct_accessors.c
