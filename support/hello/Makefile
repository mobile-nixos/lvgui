#
# Makefile
#
CC ?= gcc
AR ?= ar
LVGL_DIR ?= ${shell pwd}/lvgui
PREFIX ?= /usr

BINARY = hello-gui
VERSION = 0.0.1

LVGL_ENV_SIMULATOR ?= 1

# External components
include $(LVGL_DIR)/lvgui.mk

# Additional source files
CSRCS += ./src/theme.c
CSRCS += ./src/main.c
CSRCS += ./src/app_actions.c
CSRCS += ./src/tbgui_parts.c
CSRCS += ./src/window_main.c
CONFFILES += ./src/conf.h

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS)
OBJS = $(AOBJS) $(COBJS)

all: default

%.o: %.c $(CONFFILES)
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

default: $(BINARY)
    
$(BINARY): $(AOBJS) $(COBJS) $(CONFFILES)
	$(CC) -o $@ -fPIC $(AOBJS) $(COBJS) $(LDFLAGS)

clean: 
	rm -f $(BINARY) $(AOBJS) $(COBJS)

install:
	install -vDt $(PREFIX)/bin/ $(BINARY)
