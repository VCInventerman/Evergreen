#pragma once


// Detecting the platform using the preprocessor will not work for universal binaries
#ifndef EVG_PREPROCESSOR_OVERRIDE
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define EVG_PLATFORM_WIN
#define _WIN32_WINNT 0x0601


#elif defined(_POSIX_VERSION)

#ifdef __linux__
#define EVG_PLATFORM_LINUX
#elif __APPLE__
#define EVG_PLATFORM_APPLE
#if TARGET_IPHONE_SIMULATOR
#define EVG_TARGET_IOSSIM
#elif TARGET_OS_IPHONE
#define EVG_TARGET_IOS
#elif TARGET_OS_MAC
#define EVG_TARGET_OSX
#else
#   error "Unknown Apple platform"
#endif

#elif __unix__
#define EVG_PLATFORM_UNIX
#endif


#else
#   error "Unknown compiler"
#endif
#endif