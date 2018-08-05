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
        
        COMMON_API BlockAllocator();
        COMMON_API BlockAllocator( size_t blockSize, size_t blockCount );
        COMMON_API BlockAllocator( void *metadata, size_t metadataSize, size_t blockSize, size_t blockCount );

        COMMON_API BlockAllocator( BlockAllocator &&move );
        COMMON_API BlockAllocator& operator = ( BlockAllocator &&move );

        COMMON_API virtual ~BlockAllocator();
        
        COMMON_API virtual uintptr_t allocate( size_t size ) override;
        COMMON_API virtual void free( uintptr_t ptr ) override;

        COMMON_API void clear();

        // Clones all allocations from 'other'
        //  Ex: if other has allocated block 1 to 10, 
        //      this marks block 1 to 10 in this allocator as allocated
        // WARNING: this resets all allocations
        // This allocator must be atleast as big as other,
        // and have the same blocksize
        COMMON_API void cloneAllocationsFrom( const BlockAllocator &other );

    private:
        struct Impl;
        PImplHelper<Impl, 32> mImpl;
    };
}