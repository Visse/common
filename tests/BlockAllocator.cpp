#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "IAllocator.h"
#include "Common/BlockAllocator.h"
#include "Common/StringFormat.h"

TEST_CASE( "BlockAllocator", "[Common][BlockAllocator]" )
{
    using Common::BlockAllocator;
    const int Cases[] = {
        1, 2, 3, 4, 16, 31, 32, 100, 128, 1024, 1111, 2048
    };

    for (int blockSize : Cases) {
        for (int blockCount : Cases) {
            SECTION(StringUtils::printf("BlockSize = %i, BlockCount = %i", blockSize, blockCount))
            {
                // make sure that we can allocate the right amount of blocks of the specified size
                BlockAllocator allocator(blockSize, blockCount);
                SECTION("Fill") {
                    std::vector<uintptr_t> ptrs;

                    for (int i=0; i < blockCount; ++i) {
                        uintptr_t ptr = allocator.allocate(blockSize);
                        REQUIRE(ptr != BlockAllocator::NULL_PTR);
                        ptrs.push_back(ptr);
                    }

                    // at this point the allocator should be full
                    uintptr_t ptr = allocator.allocate(blockSize);
                    REQUIRE(ptr == BlockAllocator::NULL_PTR);

                    allocator.clear();
                    
                    for (int i=0; i < blockCount; ++i) {
                        uintptr_t ptr = allocator.allocate(blockSize);
                        REQUIRE(ptr != BlockAllocator::NULL_PTR);
                        ptrs.push_back(ptr);
                    }

                    // at this point the allocator should be full
                    ptr = allocator.allocate(blockSize);
                    REQUIRE(ptr == BlockAllocator::NULL_PTR);

                    // Free all ptrs
                    for (auto ptr : ptrs) {
                        allocator.free(ptr);
                    }
                }

                // Run some general allocator tests
                SECTION("General Tests")
                {
                    testAllocator(&allocator);
                }
            }
        }
    }
}