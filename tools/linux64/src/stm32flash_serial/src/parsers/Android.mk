LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libparsers
LOCAL_SRC_FILES := binary.c hex.c
include $(BUILD_STATIC_LIBRARY)
