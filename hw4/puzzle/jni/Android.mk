LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:=driver-func
LOCAL_SRC_FILES:=driver.c
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

