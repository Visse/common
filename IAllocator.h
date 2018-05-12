#pragma once

#include <cstdint>

namespace Common
{
    class IAllocator {
    public:
        IAllocator() = default;
        virtual ~IAllocator() = default;


        virtual uintptr_t allocate( size_t size ) = 0;
        virtual void free( uintptr_t ptr ) = 0;
        
        // No copy by default
        IAllocator( const IAllocator& ) = delete;
        IAllocator( IAllocator&& ) = delete;

        IAllocator& operator = ( const IAllocator& ) = delete;
        IAllocator& operator = ( IAllocator&& ) = delete;

    public:
        static const uintptr_t NULL_PTR = uintptr_t(-1);
    };
}

