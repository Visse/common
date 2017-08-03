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