# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             blue3d

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/BlueAdapter.cpp
             src/main/cpp/BlueWorld.cpp
             src/main/cpp/JNI.cpp )

include_directories(../)
include_directories(../Houyi/framework/include/)
include_directories(../Houyi/framework/android/)
include_directories(../Houyi/geometry/include/)
include_directories(../Houyi/math/include/)
include_directories(../Houyi/loader/include/)
include_directories(../Houyi/astronomy/include/)
include_directories(../Houyi/renderer/include/)
include_directories(../Houyi/renderer/GLES20/include/)
include_directories(../Houyi/gui/include/)
include_directories(../Houyi/libs/libxml/)
include_directories(../Houyi/jni/)
include_directories(../Houyi/)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

set(houyi_DIR "${CMAKE_SOURCE_DIR}/../Houyi/libs")
add_library(houyi SHARED IMPORTED)
set_target_properties(houyi PROPERTIES IMPORTED_LOCATION ${houyi_DIR}/arm64-v8a/libhouyi.so)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       blue3d
                       houyi


                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )
