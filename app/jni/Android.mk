LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_XML_PATH:=	$(LOCAL_PATH)/../../libs/libxml

LOCAL_FRAMEWORK_PATH:= $(LOCAL_PATH)/../../framework
LOCAL_FRAMEWORK_ANDROID_PATH:= $(LOCAL_PATH)/../../framework/android
LOCAL_GEOMETRY_PATH:= $(LOCAL_PATH)/../../geometry
LOCAL_MATH_PATH:= $(LOCAL_PATH)/../../math
LOCAL_LOADER_PATH:= $(LOCAL_PATH)/../../loader
LOCAL_RENDERER_PATH:= $(LOCAL_PATH)/../../renderer
LOCAL_GUI_PATH:= $(LOCAL_PATH)/../../gui
LOCAL_GLES_RENDERER_PATH:= $(LOCAL_PATH)/../../renderer/GLES20
LOCAL_AI_PATH:= $(LOCAL_PATH)/../../ai
LOCAL_ASTRONOMY_PATH:= $(LOCAL_PATH)/../../astronomy
LOCAL_APP_PATH:= $(LOCAL_PATH)/../csource

LOCAL_CFLAGS:= 		-Werror -DANDROID -DDEBUG -DDEBUG_MEM -DANDROID_NDK

LOCAL_C_INCLUDES:= 	$(LOCAL_PATH)/../../jni
LOCAL_C_INCLUDES+= 	$(LOCAL_FRAMEWORK_PATH)/include	
LOCAL_C_INCLUDES+= 	$(LOCAL_FRAMEWORK_ANDROID_PATH)
LOCAL_C_INCLUDES+= 	$(LOCAL_RENDERER_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_GLES_RENDERER_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_GEOMETRY_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_MATH_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_LOADER_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_AI_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_GUI_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_ASTRONOMY_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_APP_PATH)
LOCAL_C_INCLUDES+= 	$(LOCAL_XML_PATH)

FILE_LIST:= 		$(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST+= 		$(wildcard $(LOCAL_APP_PATH)/*.cpp)
LOCAL_SRC_FILES:= 	$(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS+= 		-L$(LOCAL_PATH)/../../libs/armeabi/
LOCAL_LDLIBS+= 		-llog -lGLESv2 -ljnigraphics -lz -lhouyi

LOCAL_MODULE:= 		blue3d
include $(BUILD_SHARED_LIBRARY)