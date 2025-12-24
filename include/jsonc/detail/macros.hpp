#pragma once
#include <utility>

#if __cplusplus >= 202302L || (_HAS_CXX23 == 1 && defined(_MSC_VER))
#define _JSONC_HAS_CXX23 1
#else
#define _JSONC_HAS_CXX23 0
#endif

#if _JSONC_HAS_CXX23 == 1
#define _JSONC_UNREACHABLE() std::unreachable()
#else
#ifdef _MSC_VER
#define _JSONC_UNREACHABLE() __assume(false)
#else
#define _JSONC_UNREACHABLE() __builtin_unreachable()
#endif
#endif
