TOP_LOCAL_PATH := $(call my-dir)

include $(call all-named-subdir-makefiles, parsers)

LOCAL_PATH := $(TOP_LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE := stm32flash
LOCAL_SRC_FILES :=	\
	dev_table.c	\
	i2c.c		\
	init.c		\
	main.c		\
	port.c		\
	serial_common.c	\
	serial_platform.c	\
	stm32.c		\
	utils.c
LOCAL_STATIC_LIBRARIES := libparsers
include $(BUILD_EXECUTABLE)
