LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= 		-Werror -DANDROID -DANDROID_NDK -DFT2_BUILD_LIBRARY=1 -DDARWIN_NO_CARBON

LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/config
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/core
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/devices
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/effects
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/gpu
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/images
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/lazy
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/pdf
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/pipe
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/ports
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/svg
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/text
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/utils
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/views
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/include/xml
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/core
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/effects
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/effects/gradients
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/image
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/images
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/opts
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/ports/android
LOCAL_C_INCLUDES+= 	$(LOCAL_SKIA_PATH)/src/utils
LOCAL_C_INCLUDES+= 	$(LOCAL_JPG_PATH)
LOCAL_C_INCLUDES+= 	$(LOCAL_PNG_PATH)
LOCAL_C_INCLUDES+= 	$(LOCAL_FREETYPE_PATH)/include

SKIA_FILE_LIST:=	$(wildcard $(LOCAL_SKIA_PATH)/src/ports/android/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/ports/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/core/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/effects/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/effects/gradients/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/image/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/images/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/opts/*.cpp)
SKIA_FILE_LIST+=	$(wildcard $(LOCAL_SKIA_PATH)/src/utils/*.cpp)
LOCAL_SRC_FILES:= 	$(SKIA_FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS+= 		-llog -lGLESv2 -ljnigraphics
LOCAL_STATIC_LIBRARIES += houyipng houyijpg houyifreetype

LOCAL_MODULE+= 		houyiskia
include $(BUILD_SHARED_LIBRARY)	