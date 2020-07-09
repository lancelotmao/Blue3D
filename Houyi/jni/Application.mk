APP_STL := c++_static
APP_OPTIM := debug
APP_PLATFORM := android-18
APP_ABI := arm64-v8a

# Enable NEON
TARGET_ARCH_ABI=arm64-v8a

#NDK_TOOLCHAIN_VERSION := 4.9

# Use clang to avoid GCC version and to have better diagnostic error message
NDK_TOOLCHAIN_VERSION := clang

# Enable c++11 extentions in source code
APP_CPPFLAGS += -std=c++11

#APP_MODULES = houyi