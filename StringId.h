#pragma once

#include "HandleType.h"
#include "build_config.h"

#include <cstdint>
#include <cstring>

namespace Common
{
    namespace internal
    {
        COMMON_API void incRef( uint64_t id );
        COMMON_API void decRef( uint64_t id );
    }

    WRAP_REFCOUNTED_HANDLE_FUNC(StringId, uint64_t, internal::incRef, internal::decRef, 0);

    COMMON_API StringId CreateStringId( const char *str, size_t len);
    COMMON_API const char* GetCString( StringId id );


    inline StringId CreateStringId( const char *str )
    {
        return CreateStringId(str, strlen(str));
    }

    template< int N >
    StringId CreateStringId( const char (&str)[N] ) {
        return CreateStringId(str, N);
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