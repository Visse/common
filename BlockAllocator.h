#pragma once

#include "Common/build_config.h"
#include "Common/PImplHelper.h"
#include "Common/IAllocator.h"

namespace Common
{
    class BlockAllocator :
        public IAllocator
    {
    public:
        static COMMON_API size_t RequiredMetaDataSize( size_t blockCount );

        COMMON_API BlockAllocator( size_t blockSize, size_t blockCount );
        COMMON_API BlockAllocator( void *metadata, size_t metadataSize, size_t blockSize, size_t blockCount );

        COMMON_API virtual ~BlockAllocator();
        
        COMMON_API virtual uintptr_t allocate( size_t size ) override;
        COMMON_API virtual void free( uintptr_t ptr ) override;

    private:
        struct Impl;
        PImplHelper<Impl, 32> mImpl;
    };
}