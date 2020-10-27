#pragma once

#ifdef _MSC_VER
#define BGL_FORCE_INLINE __forceinline
#else
#define BGL_FORCE_INLINE __attribute__((always_inline))
#define BGL_FORCE_NOINLINE __attribute__((noinline))
#endif