LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libhouyiiconv
LOCAL_SRC_FILES := ../prebuilts/libhouyiiconv.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libhouyixml
LOCAL_SRC_FILES := ../prebuilts/libhouyixml.so
include $(PREBUILT_SHARED_LIBRARY)
