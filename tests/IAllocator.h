
#include "catch.hpp"
#include "Common/IAllocator.h"

#include <random>
#include <cassert>


struct TestAllocatorParams 
{
    size_t minAllocationSize = 0,
           maxAllocationSize = 1024,
           maxAllocationCount = 32768;

    size_t maxAllocationRange = size_t(-1);

    size_t seed = 0;
};

void testAllocator( Common::IAllocator *allocator, const TestAllocatorParams params )
{
    using Common::IAllocator;

    struct Allocation {
        uintptr_t ptr, size;
    };
    std::vector<Allocation> allocations;

    std::ranlux24 randEngine(params.seed);
    std::uniform_int_distribution<size_t> dist(params.minAllocationSize, params.maxAllocationSize);

    auto randSize = [&]() {
        return dist(randEngine);
    };

    auto checkAllocation = [&]( const Allocation &p1 ) -> void {
        // Check that allocation range is valid
        REQUIRE(p1.size < params.maxAllocationRange);

        REQUIRE(p1.ptr < (params.maxAllocationRange - p1.size));
        REQUIRE((p1.ptr + p1.size) < params.maxAllocationRange);

        for (const auto &p2 : allocations) {
            if ((p2.ptr + p2.size) < p1.ptr) continue;
            if ((p1.ptr + p1.size) < p2.ptr) continue;

            FAIL("Allocation overlap!");
        }
    };

    auto allocate = [&]( Allocation &result ) -> bool {
        size_t size = randSize();

        uintptr_t ptr = allocator->allocate(size);

        if (size == 0) {
            REQUIRE(ptr == IAllocator::NULL_PTR);
            return false;
        }
        if (ptr == IAllocator::NULL_PTR) {
            return false;
        }

        result.ptr = ptr;
        result.size = size;

        checkAllocation(result);
        return true;
    };

    SECTION("Allocation + Free") {
        size_t maxAmount=0, minAmount=size_t(-1), amount;

        for (int i=0; i < 10; ++i) {
            amount = 0;
            // Try to allocate as much as posible
            for (size_t i=0; i < params.maxAllocationCount; ++i) {
                Allocation allocation;
                if (allocate(allocation)) {
                    allocations.push_back(allocation);
                    amount += allocation.size;
                }
            }
        
            // Clean up
            for (auto allocation : allocations) {
                allocator->free(allocation.ptr);   
            }
            allocations.clear();

            // Some allocations must have succeded
            REQUIRE(amount > 0);

            maxAmount = std::max(maxAmount, amount);
            minAmount = std::min(minAmount, amount);
        }
    }

    SECTION("Allocation & Free interleaved") {
        for (int i=0; i < 10; ++i) {
            // Fill up the allocator
            for (size_t i=0; i < params.maxAllocationCount; ++i) {
                Allocation allocation;
                if (allocate(allocation)) {
                    allocations.push_back(allocation);
                }
            }

            // Free 1/4 of the allocations
            for (size_t i=0, s = allocations.size()/4; i < s; ++i) {
                size_t n = dist(randEngine, decltype(dist)::param_type{0, allocations.size()});

                auto &alloc = allocations.back();
                std::swap(allocations[n], alloc);

                allocator->free(alloc.ptr);
                allocations.pop_back();
            }

            // Do a mix of allocs & free's
            for (size_t i=0; i < params.maxAllocationCount*2; ++i) {
                if (dist(randEngine, decltype(dist)::param_type(0,1)) == 0) {
                    if (allocations.size() < params.maxAllocationCount) {
                        Allocation alloc;
                        if (allocate(alloc)) {
                            allocations.push_back(alloc);
                        }
                    }
                    else {
                        if (!allocations.empty()) {
                            size_t n = dist(randEngine, decltype(dist)::param_type{0, allocations.size()});

                            auto &alloc = allocations.back();
                            std::swap(allocations[n], alloc);

                            allocator->free(alloc.ptr);
                            allocations.pop_back();

                        }
                    }
                }
            }

            // Clean up
            for (auto allocation : allocations) {
                allocator->free(allocation.ptr);   
            }
            allocations.clear();
        }
    }
}

void testAllocator( Common::IAllocator *allocator )
{

}
