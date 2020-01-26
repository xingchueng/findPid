LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS:=optional
LOCAL_SRC_FILES:=find_pid.c
LOCAL_MODULE:=find_pid
LOCAL_CERTIFICATE:=platform
include $(BUILD_EXECUTABLE)

