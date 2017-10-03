#pragma once


#if COMMON_BUILT_SHARED
#   if COMMON_BUILDING
#       define COMMON_API __declspec(dllexport)
#   else 
#       define COMMON_API __declspec(dllimport)
#   endif // BUILDING_COMMON
#else // Static build
#   define COMMON_API
#endif // BUILD_COMMON_SHARED


#ifndef COMMON_DEBUG_LEVEL
#   ifdef NDEBUG
#       define COMMON_DEBUG_LEVEL 0
#   else
// Default to level 1 in debug builds
#       define COMMON_DEBUG_LEVEL 1
#   endif
#endif

