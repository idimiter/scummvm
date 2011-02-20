MODULE := engines/pegasus

MODULE_OBJS = \
	detection.o \
	graphics.o \
	menu.o \
	pegasus.o \
	sound.o \
	video.o


# This module can be built as a plugin
ifeq ($(ENABLE_PEGASUS), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
