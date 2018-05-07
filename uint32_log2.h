#include <cinttypes>

namespace Common
{

#if defined __GNUC__

static inline uint32_t uint32_log2( uint32_t x )
{
    // '| 1' is to handle the case where x=0, and will return 0
    return 31 - __builtin_clz(x | 1);
}

#elif defined _MSC_VER

#include "intrin.h"

static inline uint32_t uint32_log2( uint32_t x )
{
    unsigned long res = 0;
    // '| 1' is to handle the case where x=0, and will return 0
    _BitScanReverse(&res, x | 1);
    return res;
}

#else
#error Add implemention of uint32_log2 to this platform
#endif

}