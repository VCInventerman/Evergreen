#pragma once


// Detecting the platform using the preprocessor will not work for universal binaries
//#ifndef EVG_PREPROCESSOR_OVERRIDE
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define EVG_PLATFORM_WIN
#pragma warning(suppress : 4005)
#define UNICODE // ANSI functions are banned, they don't support unicode
#define _UNICODE
#define _WIN32_WINNT 0x0601


#elif defined(__unix__)

#if defined(__linux__)
#define EVG_PLATFORM_LINUX
#elif defined(__APPLE__)
#define EVG_PLATFORM_APPLE
#elif defined(TARGET_IPHONE_SIMULATOR)
#define EVG_TARGET_IOSSIM
#elif defined(TARGET_OS_IPHONE)
#define EVG_TARGET_IOS
#elif defined(TARGET_OS_MAC)
#define EVG_TARGET_OSX
#else
#error "Unknown Unix platform"
#endif


#else
#error "Unknown platform target"
#endif
//#endif


#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define EVG_COMPILER_MSVC
#elif defined(__clang__)
#define EVG_COMPILER_CLANG
#elif defined(__GNUC__)
#define EVG_COMPILER_GCC
#else
#error "Unknown compiler"
#endif




#if defined(EVG_COMPILER_MSVC)
#pragma warning( disable : 4189 )
#pragma warning( disable : 4100 )
#else
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic push


#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 
#endif

#define _FILE_OFFSET_BITS 64
#endif





#if defined(_MSC_VER)
#define debugBreak() __debugbreak()
#elif __has_builtin(__builtin_debugtrap)
#define debugBreak() __builtin_debugtrap()
#else
#include <signal.h>

#if defined(SIGTRAP)
raise(SIGTRAP)
#else
raise(SIGABRT)
#endif

#endif


















