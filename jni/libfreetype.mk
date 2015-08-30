LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= 		-Werror -DANDROID -DANDROID_NDK -DFT2_BUILD_LIBRARY=1 -DDARWIN_NO_CARBON

LOCAL_C_INCLUDES+= 	$(LOCAL_FREETYPE_PATH)/include

FREETYPE_FILE_LIST:=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/autofit/autofit.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftapi.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftinit.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftbase.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftbbox.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftbitmap.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftdebug.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftfstype.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftlcdfil.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftxf86.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftglyph.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftstroke.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftsynth.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/ftsystem.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/base/fttype1.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/cff/cff.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/gzip/ftgzip.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/pshinter/pshinter.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/psnames/psnames.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/raster/raster.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/sfnt/sfnt.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/smooth/smooth.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/truetype/truetype.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/bdf/bdf.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/cid/type1cid.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/lzw/ftlzw.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/pcf/pcf.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/pfr/pfr.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/psaux/psaux.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/type1/type1.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/type42/type42.c)
FREETYPE_FILE_LIST+=	$(wildcard $(LOCAL_FREETYPE_PATH)/src/winfonts/*.c)

LOCAL_SRC_FILES += $(FREETYPE_FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_MODULE+= 		houyifreetype
include $(BUILD_SHARED_LIBRARY)	