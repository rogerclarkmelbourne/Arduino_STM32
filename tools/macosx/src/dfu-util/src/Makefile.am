AM_CFLAGS = -Wall -Wextra

bin_PROGRAMS = dfu-util dfu-suffix dfu-prefix
dfu_util_SOURCES = main.c \
		portable.h \
		dfu_load.c \
		dfu_load.h \
		dfu_util.c \
		dfu_util.h \
		dfuse.c \
		dfuse.h \
		dfuse_mem.c \
		dfuse_mem.h \
		dfu.c \
		dfu.h \
		usb_dfu.h \
		dfu_file.c \
		dfu_file.h \
		quirks.c \
		quirks.h

dfu_suffix_SOURCES = suffix.c \
		dfu_file.h \
		dfu_file.c

dfu_prefix_SOURCES = prefix.c \
		dfu_file.h \
		dfu_file.c
