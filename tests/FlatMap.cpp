#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/FlatMap.h"

#include <random>
#include <algorithm>


TEST_CASE( "FlatMap", "[Common][FlatMap]" )
{
    using Common::FlatMap;

    std::vector<int> data;
    for (int i=0; i < 100; ++i) {
        data.push_back(i);
    }

    // deterministic random shuffle
    std::ranlux24 randEngine( 1234123 );
    std::shuffle(data.begin(), data.end(), randEngine);
    

    FlatMap<int, int> map;

    for (int i : data ) {
        map.insert(i, i);
    }

    REQUIRE(std::is_sorted(map.begin(), map.end()));

    for (int i : data) {
        auto iter = map.find(i);
        REQUIRE(iter != map.end());
        REQUIRE(iter->second == i);
    }

    auto nonExistent = map.find(1000);
    REQUIRE(nonExistent == map.end());
    nonExistent = map.find(-1);
    REQUIRE(nonExistent == map.end());

    map.insert_replace(-1, 1000);
    REQUIRE(map.find(-1) != map.end());

    size_t size = map.size();
    map.insert_replace(1, 0);
    REQUIRE(map.size() == size);
    auto iter = map.find(1);
    REQUIRE(iter != map.end());
    REQUIRE(iter->second == 0);

}