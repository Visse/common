#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/BuddyAllocator.h"

#include <vector>
#include <random>

using namespace Common;


void run_tests(size_t size, size_t leafSize)
{
    std::vector<uint8_t> metadata(BuddyAllocator::SizeOfMetaDataForMemory(size, leafSize) + 2*sizeof(uint32_t), 0xcd);
    BuddyAllocator allocator(metadata.data() + sizeof(uint32_t), metadata.size() - 2 * sizeof(uint32_t), (void*)0x1, size, leafSize);
    
    struct PtrInfo {
        uintptr_t ptr;
        size_t size;
    };
    std::vector<PtrInfo> ptrs;
    const size_t leafCount = size / leafSize;

    auto validateAllocation = [&]( PtrInfo p1 )
    {
        REQUIRE(p1.ptr < size);
        REQUIRE((p1.ptr+p1.size) <= size);

        for (const PtrInfo &p2 : ptrs) {
            if (p1.ptr < p2.ptr && (p1.ptr+p1.size) <= p2.ptr) continue;
            if (p1.ptr >= (p2.ptr+p2.size) && (p1.ptr+p1.size) > (p2.ptr+p2.size)) continue;
            FAIL("Allocations overlapp!");
        }
    };

    auto allocate = [&]( size_t size, PtrInfo &info) {
        void *ptr = allocator.allocate(size);
        if (!ptr) return false;

        info = PtrInfo{(uintptr_t)ptr-0x1, size};
        validateAllocation(info);
        return true;
    };
    auto free = [&]( PtrInfo info ) {
        void *ptr = (void*)(info.ptr + 0x1);
        allocator.free(ptr);
    };

    auto freeAll = [&]() {
        for (PtrInfo info : ptrs) {
            free(info);
        }
        ptrs.clear();
        
        // do a full round of allocation to make sure its posible to allocate all memmory (to make sure the state haven't got currupt)
        for (size_t i=0; i < leafCount; ++i) {
            PtrInfo info;
            REQUIRE(allocate(leafSize, info));
            ptrs.push_back(info);
        }
        for (PtrInfo info : ptrs) {
            free(info);
        }
        ptrs.clear();
    };

    SECTION("Allocate + free")
    {
        for (int _=0; _ < 10; ++_) {
            for (size_t i=0; i < leafCount; ++i) {
                PtrInfo info;
                REQUIRE(allocate(leafSize, info));
                ptrs.push_back(info);
            }


            for (auto &info : ptrs) {
                free(info);
            }
            ptrs.clear();
        }
    }

    SECTION("Allocate & free interleaved")
    {
        std::mt19937 rand(size);
        
        size_t maxNumAllocations = leafCount;

        std::bernoulli_distribution freeDist;
        for (size_t i=0; i < 100000; ++i) {
            double chance = (double)ptrs.size() / (double)maxNumAllocations;
            if (freeDist(rand, std::bernoulli_distribution::param_type{chance})) {
                size_t elem = std::uniform_int_distribution<size_t>(0, ptrs.size()-1)(rand);

                free(ptrs[elem]);
                ptrs.erase(ptrs.begin() + elem);
            }
            else {
                PtrInfo info;
                // Leaf size'd allocations should never failed (as long as there are space for them)
                REQUIRE(allocate(leafSize, info));

                ptrs.push_back(info);
            }
        }

        freeAll();
    }

    SECTION("Mixed size allocations")
    {
        std::mt19937 rand(size);
        size_t allocated = 0;
        const size_t maxAllocated = leafCount * leafSize;
        const size_t maxSize = size / 2;

        std::bernoulli_distribution freeDist;
        for (size_t i=0; i < 100000; ++i) {
            double chance = (double)allocated / (double)maxAllocated;
            if (freeDist(rand, std::bernoulli_distribution::param_type{chance})) {
                size_t elem = std::uniform_int_distribution<size_t>(0, ptrs.size()-1)(rand);

                free(ptrs[elem]);
                allocated -= ptrs[elem].size;
                ptrs.erase(ptrs.begin() + elem);
            }
            else {
                PtrInfo info;
                size_t size = std::uniform_int_distribution<size_t>(1, maxSize)(rand);

                bool success = false;
                for (; size > leafSize; size -= leafSize) {
                    if (allocate(size, info)) {
                        success = true;
                        break;
                    }
                }
                if (!success) {
                    // leaf sized allocation should never fail
                    REQUIRE(allocate(leafSize, info));
                }

                // need to round allocation size to next leaf size
                size_t actualSize = leafSize;
                while (actualSize < size) actualSize *= 2;
                allocated += actualSize;
                info.size = actualSize;

                ptrs.push_back(info);
            }
        }

        freeAll();
    }
}

TEST_CASE( "BuddyAllocator", "[Common][BuddyAllocator]" )
{
    SECTION("size=512 & leaf=32")
    {
        run_tests(512, 32);
    }

    SECTION("size=1024 & leaf=32")
    {
        run_tests(1024, 32);
    }
    
    SECTION("size=544 & leaf=32")
    {
        run_tests(544, 32);
    }

    SECTION("size=1234 & leaf=32")
    {
        run_tests(1234, 32);
    }
}