# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

file(GLOB HOUYI_SRC
        framework/*.cpp
        framework/android/*.cpp
        geometry/*.cpp
        loader/*.cpp
        math/*.cpp
        renderer/*.cpp
        renderer/GLES20/*.cpp
        gui/*.cpp
        astronomy/*.cpp
        jni/*.cpp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library(houyi SHARED ${HOUYI_SRC})

include_directories(../)
include_directories(framework/include/)
include_directories(framework/android/)
include_directories(geometry/include/)
include_directories(math/include/)
include_directories(loader/include/)
include_directories(astronomy/include/)
include_directories(renderer/include/)
include_directories(renderer/GLES20/include/)
include_directories(gui/include/)
include_directories(libs/libxml/)
include_directories(.)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

add_library(LIBXML SHARED IMPORTED )
set_target_properties(LIBXML
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/prebuilts/libhouyixml.so)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       houyi

                       # Links the target library to the log library
                       # included in the NDK.
                       log GLESv2 jnigraphics z android LIBXML)
