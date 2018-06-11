#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include "Common/IteratorAdopter.h"

#include <numeric>

struct TestAdopter :
    public Common::IteratorAdopter<TestAdopter, int, std::random_access_iterator_tag>
{
    using value_type = int;
    value_type *ptr = nullptr;

    TestAdopter() = default;
    TestAdopter( const TestAdopter& ) = default;
    TestAdopter& operator = ( const TestAdopter& ) = default;

    TestAdopter( value_type *ptr_ ) :
        ptr(ptr_)
    {}

    value_type& dereference() const {
        return *ptr;
    }

    void increment() {
        ptr++;
    }
    void decrement() {
        ptr--;
    }
    void advance( ptrdiff_t n ) {
        ptr += n;
    }

    bool equal( const TestAdopter &other ) const {
        return ptr == other.ptr;
    }

    ptrdiff_t distance( const TestAdopter &other  ) const {
        return other.ptr - ptr;
    }
};


TEST_CASE( "IteratorAdopter", "[Common][IteratorAdopter]" )
{
    int array[10];
    // fill array with increasing numbers
    std::iota(std::begin(array), std::end(array), 0);

    const auto begin = TestAdopter(std::begin(array)),
               end = TestAdopter(std::end(array));


    SECTION("operator == !=") {
        REQUIRE(begin == begin);
        REQUIRE(begin != end);
    }
    SECTION ("operator ++") {
        int idx = 0;
        for (auto iter = begin; iter != end; ++idx) {
            REQUIRE (*iter == idx);
            ++iter;
        }
        idx = 0;
        for (auto iter = begin; iter != end; ++idx) {
            REQUIRE (*iter == idx);
            iter++;
        }
    }
    SECTION ("operator --") {
        int idx = 10;
        for (auto iter = end; --idx >= 0;) {
            --iter;
            REQUIRE (*iter == idx);
        }
        idx = 10;
        for (auto iter = end; --idx >= 0;) {
            iter--;
            REQUIRE (*iter == idx);
        }
    }
    SECTION ("operator -") {
        REQUIRE ((end - begin) == std::size(array));
    }

    SECTION ("operator * and ->") {
        int idx = 0;
        for (auto iter = begin; iter != end; ++iter, ++idx) {
            *iter = -idx;
        }

        idx = 0;
        for (auto iter = begin; iter != end; ++iter, ++idx) {
            REQUIRE ( *(iter.operator->()) == -idx);
        }
    }

    SECTION ("random access") {
        srand(0);
        int size = std::size(array);
        
        CAPTURE(size);

        REQUIRE((begin < end) == true);
        REQUIRE((begin <= end) == true);
        
        REQUIRE((begin > end) == false);
        REQUIRE((begin >= end) == false);

        REQUIRE((begin < begin) == false);
        REQUIRE((begin <= begin) == true);

        REQUIRE((end - begin) == size);

        for (int i=0; i < 100; ++i) {
            int idx = rand() % size;
            
            CAPTURE(idx);
            auto iter = begin + idx;

            REQUIRE(*iter == idx);
            REQUIRE(iter[0] == idx);
            // [-idx] == begining of array
            REQUIRE(iter[-idx] == 0);
            REQUIRE(begin[idx] == idx);

            REQUIRE((iter - begin) == idx);
        }
    };
}