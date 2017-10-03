#pragma once

#include "build_config.h"
#include "PImplHelper.h"

namespace Common
{
    class BuddyAllocator {
    public:
        static COMMON_API size_t SizeOfMetaDataForMemory( size_t memorySize, size_t leafSize );

    public:
        COMMON_API BuddyAllocator( void *metaData, size_t metaDataSize,
                        void *memory, size_t memorySize, size_t leafSize
                       );
        COMMON_API ~BuddyAllocator();

        COMMON_API void* allocate( size_t size );
        COMMON_API void free( void *ptr );

#if COMMON_DEBUG_LEVEL > 0
        COMMON_API void dumpInternalState();
#endif
    public:
        struct Impl;
        PImplHelper<Impl, 64> mImpl;
    };
}