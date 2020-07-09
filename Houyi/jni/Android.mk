# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS:= 		-Werror -DANDROID -DDEBUG -DDEBUG_MEM -DANDROID_NDK -mfloat-abi=softfp -mfpu=neon

LOCAL_LINK_LIB_PATH:= 	$(LOCAL_PATH)/../libs

LOCAL_ICONV_PATH:=		$(LOCAL_PATH)/../libs/libiconv
LOCAL_XML_PATH:=		$(LOCAL_PATH)/../libs/libxml
LOCAL_FREETYPE_PATH:= 	$(LOCAL_PATH)/../libs/freetype

# until we really need to build these libraries from source which usually is not the case
#include $(LOCAL_PATH)/libiconv.mk
#include $(LOCAL_PATH)/libxml.mk
#include $(LOCAL_PATH)/libpng.mk
#include $(LOCAL_PATH)/libjpg.mk
#include $(LOCAL_PATH)/libskia.mk
#include $(LOCAL_PATH)/libfreetype.mk

include $(LOCAL_PATH)/houyi.mk
include $(LOCAL_PATH)/prebuilt.mk
