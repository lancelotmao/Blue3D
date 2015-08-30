LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_XML_PATH:=	$(LOCAL_PATH)/../libs/libxml

LOCAL_C_INCLUDES+= 	$(LOCAL_XML_PATH)

FILE_LIST:= 		$(wildcard $(LOCAL_XML_PATH)/*.c)
LOCAL_SRC_FILES:= 	$(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS+= 		-lz
LOCAL_STATIC_LIBRARIES += houyiiconv

LOCAL_MODULE:= 		houyixml
include $(BUILD_SHARED_LIBRARY)	