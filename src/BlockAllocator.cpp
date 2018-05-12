#include "BlockAllocator.h"
#include "EnumFlagOp.h"
#include "DynamicBitUtilty.h"

#include <cassert>
#include <new>
#include <cstring>

namespace Common
{
    namespace BlockAllocatorImpl 
    {
        enum class Flags : uint32_t {
            None = 0,
            OwnsMetadata = 1
        };
        DECLARE_ENUM_FLAG(Flags);

        struct Impl {
            size_t blockSize, blockCount;

            uint32_t *blockInfo;
            Flags flags = Flags::None;
        };


        size_t requiredMetadataSize( size_t blockCount )
        {
            size_t bits = blockCount * 2;

            size_t elements = bits / 32;
            if (bits%32) {
                elements++;
            }
            return elements * sizeof(uint32_t);
        }

        void init( Impl *impl, size_t blockSize, size_t blockCount, void *metadata, size_t metadataSize )
        {
            assert (blockSize > 0);
            assert (blockCount > 0);

            impl->blockSize = blockSize;
            impl->blockCount = blockCount;
            impl->flags = Flags::None;

            if (metadata == nullptr) {
                assert (metadataSize == 0);
                metadataSize = requiredMetadataSize(blockCount);
                metadata = operator new(metadataSize);

                impl->flags = set(impl->flags, Flags::OwnsMetadata);
            }

            assert (metadataSize <= requiredMetadataSize(blockCount));
            
            std::memset(metadata, 0, metadataSize);
            impl->blockInfo = (uint32_t*)metadata;
        }

        void destroy( Impl *impl )
        {
            if (all(impl->flags, Flags::OwnsMetadata)) {
                operator delete(impl->blockInfo);
            }
        }

        bool isFree( Impl *impl, size_t block ) {
            assert (block < impl->blockCount);
            return DynamicBitUtility::getBit(impl->blockInfo, block*2 + 0) == 0;
        }

        void allocateBlock( Impl *impl, size_t block, bool mid )
        {
            assert (block < impl->blockCount);
            DynamicBitUtility::setBit(impl->blockInfo, block*2 + 0, 1);
            DynamicBitUtility::setBit(impl->blockInfo, block*2 + 1, mid);
        }

        uintptr_t allocateBlocks( Impl *impl, size_t count )
        {
            assert (count >= 1);

            if (count > impl->blockCount) return BlockAllocator::NULL_PTR;
            size_t lastBlock = impl->blockCount - count;

            for (size_t block=0; block <= lastBlock; ++block) {
                bool found = true;
                for (size_t i=0; i < count; ++i) {
                    if (isFree(impl, block+i)) continue;
                    found = false;
                    block += i;
                    break;
                }
                if (!found) continue;

                lastBlock = block + count - 1;

                uintptr_t ptr = block * impl->blockSize;
                
                for (; block < lastBlock; ++block) {
                    allocateBlock(impl, block, true);   
                }
                allocateBlock(impl, block, false);
                
                return ptr;
            }
            return BlockAllocator::NULL_PTR;
        }

        bool freeBlock( Impl *impl, size_t block )
        {
            assert (block < impl->blockCount);
            uint32_t wasFree = DynamicBitUtility::swapBit(impl->blockInfo, block*2 + 0, 0);
            uint32_t cont = DynamicBitUtility::swapBit(impl->blockInfo, block*2 + 1, 0);

            assert (wasFree == true);
            (void)wasFree;

            return cont;
        }

        void freeBlocks( Impl *impl, size_t firstBlock )
        {
            while (freeBlock(impl, firstBlock)) {
                firstBlock++;
            }
        }

        uintptr_t allocate( Impl *impl, size_t size )
        {
            if (size == 0) return BlockAllocator::NULL_PTR;

            size_t blockCount = size / impl->blockSize;
            if (size % impl->blockSize) blockCount++;
            
            return allocateBlocks(impl, blockCount);
        }

        void free( Impl *impl, uintptr_t ptr )
        {
            assert ((ptr % impl->blockSize) == 0);
            size_t block = ptr / impl->blockSize;

            freeBlocks(impl, block);
        }
    }

    namespace impl = BlockAllocatorImpl;

    struct BlockAllocator::Impl : public impl::Impl {};

    COMMON_API size_t BlockAllocator::RequiredMetaDataSize( size_t blockCount )
    {
        return impl::requiredMetadataSize(blockCount);
    }

    COMMON_API BlockAllocator::BlockAllocator( size_t blockSize, size_t blockCount )
    {
        impl::init(mImpl, blockSize, blockCount, nullptr, 0);
    }

    COMMON_API BlockAllocator::BlockAllocator( void *metadata, size_t metadataSize, 
                                               size_t blockSize, size_t blockCount )
    {
        impl::init(mImpl, blockSize, blockCount, metadata, metadataSize);
    }

    COMMON_API BlockAllocator::~BlockAllocator()
    {
        impl::destroy(mImpl);
    }

    COMMON_API uintptr_t BlockAllocator::allocate( size_t size )
    {
        return impl::allocate(mImpl, size);
    }

    COMMON_API void BlockAllocator::free( uintptr_t ptr )
    {
        return impl::free(mImpl, ptr);
    }
}