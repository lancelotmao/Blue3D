APP_STL := stlport_static
APP_OPTIM := debug

# Enable NEON
TARGET_ARCH_ABI=armeabi-v7a

NDK_TOOLCHAIN_VERSION := 4.8

# Use clang to avoid GCC version and to have better diagnostic error message
#NDK_TOOLCHAIN_VERSION := clang

# Enable c++11 extentions in source code
APP_CPPFLAGS += -std=c++11

#APP_MODULES = houyi