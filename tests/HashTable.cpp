#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/HashTable.h"

#include <random>

TEST_CASE( "HashTable", "[Common][HashTable]" )
{
    std::vector<int> values(10000);


    int i=0;
    for (int &v : values) {
        v = i++;
    }
    

    // deterministic random shuffle
    std::ranlux24 randEngine( 1234123 );
    std::shuffle(values.begin(), values.end(), randEngine);

    Common::HashTable<int> hashtable;


    for (int &v : values) {
        REQUIRE(hashtable.find(v) == nullptr);
        REQUIRE(hashtable.insert(v, v) == true);
        REQUIRE(hashtable.find(v) != nullptr);
        REQUIRE(*hashtable.find(v) == v);
    }
}