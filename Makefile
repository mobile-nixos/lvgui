#
# Makefile
#
CC ?= gcc
AR ?= ar
LVGL_DIR ?= ${shell pwd}
PREFIX ?= /usr
STATIC ?= false

# ${lvgui.version}-${lvgl.version}
VERSION = 0.1-6.1.2

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
	-Wno-switch-enum \
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

CFLAGS ?= $(WARNING_FLAGS) $(DEBUG_FLAGS) -I$(LVGL_DIR)/ -DLVGL_ENV_SIMULATOR=$(LVGL_ENV_SIMULATOR) -fPIC
LDFLAGS ?=
LDFLAGS += -lm

REQUIRES = 
ifeq ($(LVGL_ENV_SIMULATOR), 1)
CFLAGS += $(shell pkg-config --cflags sdl2)
LDFLAGS += $(shell pkg-config --libs sdl2)
REQUIRES += sdl2
else
CFLAGS += $(shell pkg-config --cflags libevdev)
LDFLAGS += $(shell pkg-config --libs libevdev)
LDFLAGS += -lpthread
REQUIRES += libevdev
endif

# External components
include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/lv_sdr_drivers/lv_sdr_drivers.mk
include $(LVGL_DIR)/lv_lib_nanosvg/lv_lib_nanosvg.mk

ifeq ($(STATIC), true)
LIBRARY = liblvgui.a
else
LIBRARY = liblvgui.so
endif

# Additional source files
CSRCS += ./hal.c
CSRCS += ./introspection.c
CSRCS += ./artwork/lvgui_cursor.c
CSRCS += ./lvgui_struct_accessors.c

CONFFILES = ./lv_conf.h ./lv_drv_conf.h

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS)
OBJS = $(AOBJS) $(COBJS)

all: default

%.o: %.c $(CONFFILES)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

default: $(LIBRARY)
    
liblvgui.so: $(AOBJS) $(COBJS) $(CONFFILES)
	$(CC) -shared -o $@ -fPIC $(AOBJS) $(COBJS) $(LDFLAGS)
    
liblvgui.a: $(AOBJS) $(COBJS) $(CONFFILES)
	$(AR) rcs $@ $(AOBJS) $(COBJS)

clean: 
	rm -f liblvgui.so liblvgui.a $(AOBJS) $(COBJS)

lvgui.pc:
	PREFIX="$(PREFIX)" \
	REQUIRES="$(REQUIRES)" \
	STATIC="$(STATIC)" \
	VERSION="$(VERSION)" \
	./generate-pc.sh

install: lvgui.pc
	mkdir -p $(PREFIX)/lib/
	cp -v $(LIBRARY) $(PREFIX)/lib/
	mkdir -p $(PREFIX)/include
	find . -name '*.h' -exec install -vD '{}' $(PREFIX)/include/'{}' ';'
	mkdir -p $(PREFIX)/lib/pkgconfig/
	cp -v lvgui.pc $(PREFIX)/lib/pkgconfig/lvgui.pc
