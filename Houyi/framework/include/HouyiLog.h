#ifndef Houyi_HouyiLog_h
#define Houyi_HouyiLog_h

#include "Platform.h"
#include <set>
#include <string>
#include <cstdio>

using namespace std;

// defined at HouyiRoot.cpp
extern set<string> log_set;

#define  LOG_TAG    "Houyi"

#ifdef DEBUG
#ifdef ANDROID
#define LOGD(...)	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...)	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...)	__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...)	__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LogOnce(...) _LogOnce(__FILE__, __LINE__, __VA_ARGS__)
#define _LogOnce(__FILE__, __LINE__, ...) \
{\
    char numstr[21];\
    sprintf(numstr, "%d", __LINE__);\
    string key = __FILE__;\
    key += numstr;\
    if (log_set.find(key) == log_set.end())\
    {\
        LOGD("LogOnce Start: File = %s, Line= %d\n", __FILE__, __LINE__);\
        LOGD(__VA_ARGS__);\
        LOGD("LogOnce end\n");\
        log_set.insert(key);\
    }\
}
#elif __APPLE__
#define LOGD(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#define LogOnce(fmt, ...) _LogOnce(__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define _LogOnce(__FILE__, __LINE__, fmt, ...) \
{\
    char numstr[21];\
    sprintf(numstr, "%d", __LINE__);\
    string key = __FILE__;\
    key += numstr;\
    if (log_set.find(key) == log_set.end())\
    {\
        LOGD("LogOnce Start: File = %s, Line= %d\n", __FILE__, __LINE__);\
        LOGD(fmt, ##__VA_ARGS__);\
        LOGD("LogOnce end\n");\
        log_set.insert(key);\
    }\
}
#endif // PLATFORM

#else
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LogOnce(...)
#endif // DEBUG

#endif
