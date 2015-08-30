LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES+= 	$(LOCAL_JPG_PATH)

FILE_LIST:= 		$(wildcard $(LOCAL_JPG_PATH)/*.c)
LOCAL_SRC_FILES:= 	$(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS+= 		-lz

LOCAL_MODULE:= 		houyijpg
include $(BUILD_SHARED_LIBRARY)