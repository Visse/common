#pragma once

#include "Murmur3_32.h"
#include "HandleType.h"

#include <cstring>

namespace Common
{
    MAKE_HANDLE(StringId, uint32_t);

    inline StringId CreateStringId( const char *str, uint32_t len) {
        return StringId{murmur3_32(str, len)};
    }

    inline StringId CreateStringId( const char *str ) {
        return CreateStringId(str, (uint32_t)strlen(str));
    }
}

namespace std
{
    template< typename >
    struct hash;

    template<>
    struct hash<Common::StringId> {
        size_t operator () (Common::StringId id) const {
            return (size_t)id;
        }
    };
}