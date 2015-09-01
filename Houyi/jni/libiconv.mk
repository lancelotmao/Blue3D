LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ICONV_PATH:=	$(LOCAL_PATH)/../libs/libiconv

LOCAL_C_INCLUDES+= 	$(LOCAL_ICONV_PATH)

FILE_LIST+= 		$(wildcard $(LOCAL_ICONV_PATH)/*.c)
LOCAL_SRC_FILES:= 	$(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS+= 		-lc -lstdc++

LOCAL_MODULE:= 		houyiiconv
include $(BUILD_SHARED_LIBRARY)	