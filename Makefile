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

# External components
include $(LVGL_DIR)/lvgui.mk

ifeq ($(STATIC), true)
LIBRARY = liblvgui.a
else
LIBRARY = liblvgui.so
endif

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
