#include "BuddyAllocator.h"
#include "ErrorUtils.h"
#include "PointerHelpers.h"

#include <cstdint>
#include <cassert>
#include <cstring>

namespace Common
{
    namespace BuddyAllocatorImpl
    {
        static const size_t NO_NODE = (size_t)(-1);
        static const size_t NO_LEVEL = (size_t)(-1);

        struct Impl {
            uint32_t *blockSplit = nullptr,
                     *blockFree = nullptr;

            size_t leafSize = 0,
                   levels = 0;

            void *memory = nullptr;
            size_t memorySize = 0;
        };

        size_t NextPowOf2( size_t N )
        {
            assert (N < ((size_t)1<<(sizeof(size_t)*CHAR_BIT-1)));

            size_t i=1;
            while (i < N) i *= 2;
            return i;
        }

        size_t BytesForBlockSplit( size_t nodeCount ) 
        {
            size_t bits = nodeCount / 2;
            return 4 * ((bits+31) / 32);
        }

        size_t BytesForBlockFree( size_t nodeCount ) 
        {
            size_t bits = nodeCount;
            return 4 * ((bits+31) / 32);
        }

        size_t LevelsForLeafSize( size_t memorySize, size_t leafSize ) {
            size_t leafNodes = NextPowOf2(memorySize) / NextPowOf2(leafSize);

            // leafNodes must be power of 2
            assert ((leafNodes & (leafNodes-1)) == 0);

            size_t levels = 1;
            while (leafNodes >>= 1) levels++;
            return levels;
        }

        size_t NodeCountForLevels( size_t levels ) {
            return (1<<levels) - 1;
        }
    
        size_t SizeOfMetaDataForMemory( size_t memorySize, size_t leafSize )
        {
            size_t levels = LevelsForLeafSize(memorySize, leafSize);
            size_t nodes = NodeCountForLevels(levels);

            return BytesForBlockSplit(nodes) + BytesForBlockFree(nodes);
        }

        size_t parentNode( Impl *impl, size_t node ) {
            return (node-1)/2;
        }

        size_t firstChild( Impl *impl, size_t node ) {
            return node*2 + 1;
        }
       
        size_t firstNodeInLevel( Impl *impl, size_t level) {
            return (1<<level) -1;
        }
        
        size_t nodeCountInLevel( Impl *impl, size_t level ) {
            return (size_t(1)<<level);
        }

        size_t levelForAllocation( Impl *impl, size_t size )
        {
            assert ((size %  impl->leafSize) == 0);
            size /= impl->leafSize;


            size_t level = 0;
            while ((size_t(1)<<level) < size) level++;

            if (level > impl->levels) return NO_LEVEL;
            return impl->levels - level - 1;
        }

        size_t levelForNode( Impl *impl, size_t node )
        {
            if (node == 0) return 0;

            size_t level = 1;
            while ((size_t(1)<<level) <= (node+1)) ++level;
            return level-1;
        }

        size_t nodeSizeForLevel( Impl *impl, size_t level )
        {
            return (size_t(1) << (impl->levels-level-size_t(1)));
        }

        void markNodeAsFree( Impl *impl, size_t node )
        {
            assert (node < NodeCountForLevels(impl->levels));

            size_t element = node / 32,
                   index = node % 32;

            impl->blockFree[element] |= 1 << index;
        }

        void markNodeAsUsed( Impl *impl, size_t node )
        {
            assert (node < NodeCountForLevels(impl->levels));

            size_t element = node / 32,
                   index = node % 32;
            

            impl->blockFree[element] &= ~(1 << index);
        }

        bool isNodeFree( Impl *impl, size_t node ) {
            assert (node < NodeCountForLevels(impl->levels));

            size_t element = node / 32,
                   index = node % 32;
            

            return (impl->blockFree[element] & (1 << index)) != 0;
        }

        void splitNode( Impl *impl, size_t node )
        {
            assert (node < NodeCountForLevels(impl->levels)/2);

            size_t element = node / 32,
                   index = node % 32;


            impl->blockSplit[element] |= 1<<index;
        }

        void joinNode( Impl *impl, size_t node )
        {
            assert (node < NodeCountForLevels(impl->levels)/2);

            size_t element = node / 32,
                   index = node % 32;

            impl->blockSplit[element] &= ~(1<<index);
        }

        bool isNodeSplit( Impl *impl, size_t node )
        {
            assert (node < NodeCountForLevels(impl->levels)/2);

            size_t element = node / 32,
                   index = node % 32;


            return (impl->blockSplit[element] & (1<<index)) != 0;
        }

        void markEndAsAllocated( Impl *impl, size_t size )
        {
            if (size == 0) return;

            size_t nodes = (size + impl->leafSize-1)/impl->leafSize;

            size_t nodeCount = NodeCountForLevels(impl->levels);
            size_t firstNode = nodeCount - nodes;

            for (size_t i=firstNode; i < nodeCount; ++i) {
                size_t parent = parentNode(impl, i);
                // split all parents
                for (size_t j=1; j < impl->levels; ++j) {
                    splitNode(impl, parent);
                    parent = parentNode(impl, parent);
                }
            }

            for (size_t node = firstNode; node != 0; node=parentNode(impl, node)) {
                // First child
                if (node%2) continue;
                // second child - mark the first child as free
                markNodeAsFree(impl, node-1);
            }
            // since we split root node is used
            markNodeAsUsed(impl, 0);
        }

        void init( Impl *impl, void *metaData, size_t metaDataSize, void *memory, size_t memorySize, size_t leafSize )
        {
            FATAL_ASSERT(metaDataSize >= SizeOfMetaDataForMemory(memorySize, leafSize), "Need more space for metadata!");

            impl->leafSize = leafSize;
            impl->levels = LevelsForLeafSize(memorySize, leafSize);

            size_t nodeCount = NodeCountForLevels(impl->levels);

            impl->blockSplit = (uint32_t*)metaData;
            impl->blockFree = (uint32_t*)advance(metaData, BytesForBlockSplit(nodeCount));

            memset(impl->blockSplit, 0, BytesForBlockSplit(nodeCount));
            memset(impl->blockFree, 0, BytesForBlockFree(nodeCount));

            size_t size = NextPowOf2(memorySize);

            impl->memory = memory;
            impl->memorySize = memorySize;

            // Mark root node as free
            markNodeAsFree(impl, 0);

            markEndAsAllocated(impl, size-memorySize);
        }

        size_t findNode( Impl *impl, size_t offset )
        {
            for (size_t i=0; i < (impl->levels-1); ++i) {
                size_t nodeSize = nodeSizeForLevel(impl, i);
                size_t node = firstNodeInLevel(impl, i) + offset / nodeSize;

                if (!isNodeSplit(impl, node)) {
                    assert ((offset%nodeSize) == 0);
                    return node;
                }
            }
            // its a leaf node
            return offset + firstNodeInLevel(impl, impl->levels-1);
        }

        size_t siblingForNode( Impl *impl, size_t node )
        {
            assert (node != 0);
            return (node%2) ? (node+1) : (node-1);
        }

        size_t allocateNode( Impl *impl, size_t level )
        {
            if (level == 0) {
                if (isNodeFree(impl, 0)) {
                    markNodeAsUsed(impl, 0);
                    return 0;
                }
                return NO_NODE;
            }
            assert (level < impl->levels);

            size_t firstNode = firstNodeInLevel(impl, level),
                   nodeCount = nodeCountInLevel(impl, level);

            for (size_t i=0; i < nodeCount; ++i) {
                if (isNodeFree(impl, firstNode+i)) {
                    markNodeAsUsed(impl, firstNode + i);
                    return firstNode + i;
                }
            }
         
            size_t node = allocateNode(impl, level-1);
            if (node == NO_NODE) return NO_NODE;
            splitNode(impl, node);
            
            node = firstChild(impl, node);
            markNodeAsFree(impl, siblingForNode(impl, node));
            return node;
        }

        void* allocate( Impl *impl, size_t size )
        {
            size_t leafSize = impl->leafSize;
            // round to next multiple of leafSize
            size = ((size+leafSize-1) / leafSize) * leafSize;

            size_t level = levelForAllocation(impl, size);
            if (level == NO_LEVEL) return nullptr;

            size_t node = allocateNode( impl, level );
            if (node == NO_NODE) return nullptr;
            // assert that we got the correct level
            assert (levelForNode(impl, node) == level);

            size_t nodeSize = nodeSizeForLevel(impl, level) * impl->leafSize;
            size_t idx = node - firstNodeInLevel(impl, level);
            return advance( impl->memory, nodeSize * idx);
        }

        void freeNode( Impl *impl, size_t node )
        {
            if (node == 0) {
                markNodeAsFree(impl, 0);
            }
            else {
                size_t sibling = siblingForNode(impl, node);
                if (isNodeFree(impl, sibling)) {
                    markNodeAsUsed(impl, sibling);
                    size_t parent = parentNode(impl, node);
                    joinNode(impl, parent);
                    freeNode(impl, parent);
                }
                else {
                    markNodeAsFree(impl, node);
                }
            }
        }

        void free( Impl *impl, void *ptr )
        {
            size_t offset = difference(ptr, impl->memory);

            assert ((offset % impl->leafSize) == 0);
            offset /= impl->leafSize;

            size_t node = findNode(impl, offset);
            assert (isNodeFree(impl, node) == false);

            freeNode(impl, node);
        }
    }

    struct BuddyAllocator::Impl :
        public BuddyAllocatorImpl::Impl
    {};

    COMMON_API size_t BuddyAllocator::SizeOfMetaDataForMemory(size_t memorySize, size_t leafSize)
    {
        return BuddyAllocatorImpl::SizeOfMetaDataForMemory(memorySize, leafSize);
    }
     
    COMMON_API BuddyAllocator::BuddyAllocator( void *metaData, size_t metaDataSize, void *memory, size_t memorySize, size_t leafSize )
    {
        BuddyAllocatorImpl::init(mImpl, metaData, metaDataSize, memory, memorySize, leafSize);   
    }

    COMMON_API BuddyAllocator::~BuddyAllocator()
    {
    }

    COMMON_API void* BuddyAllocator::allocate( size_t size )
    {
        return BuddyAllocatorImpl::allocate(mImpl, size);
    }

    COMMON_API void BuddyAllocator::free( void *ptr )
    {
        if (!ptr) return;
        BuddyAllocatorImpl::free(mImpl, ptr);
    }
}

#if COMMON_DEBUG_LEVEL > 0
#include <cstdio>
COMMON_API void Common::BuddyAllocator::dumpInternalState()
{
    using namespace BuddyAllocatorImpl;
    size_t width = 5;

    FILE *file = fopen("buddyallocator_dump.txt", "a");


    fprintf(file, "Size: %u      LeafSize: %u     Count: %u\n", (unsigned) mImpl->memorySize, (unsigned) mImpl->leafSize, (unsigned)NodeCountForLevels(mImpl->levels));

    for (size_t i=0; i < mImpl->levels; ++i) {
        size_t count = nodeCountInLevel(mImpl, i);
        size_t first = firstNodeInLevel(mImpl, i);
        
        fprintf(file, "%2u   |", (unsigned)i);
        for (size_t n=0; n < count; ++n) {
            size_t node = first + n;
            fprintf(file, "%3u", (unsigned)node);

            bool parentFree = false;
            size_t parent = node;
            for (size_t j=0; j < i; ++j) {
                parent = parentNode(mImpl, parent);
                if (parentFree) {
                    assert(isNodeSplit(mImpl, parent));
                    assert(! isNodeFree(mImpl, parent));
                }
                else {
                    parentFree = isNodeFree(mImpl, parent);   
                }
            }

            bool split = i == (mImpl->levels-1) ? false : isNodeSplit(mImpl, node);
            bool free = isNodeFree(mImpl, node);

            if (parentFree) {
                // parent is free - this one should neither be split or free
                assert (split == false);
                assert (free == false);
                fputc('-', file);
            }
            else if (split) {
                // can't be both split & free at the same time
                assert(free == false);
                fputc('S', file);
            }
            else if (free) {
                fputc('F', file);
            }
            else {
                fputc('A', file);
            }

            size_t size = nodeSizeForLevel(mImpl, i) - 1;

            for (size_t s=0; s < (size*width); ++s) {
                fputc(' ', file);
            }
            fputc('|', file);
        }
        fputc('\n', file);
    }


    fclose(file);
}
#endif