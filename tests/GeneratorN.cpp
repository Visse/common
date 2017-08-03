#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/GeneratorN.h"

#include <vector>

using namespace Common;

TEST_CASE( "GeneratorN", "[Common][GeneratorN]" )
{
    std::vector<int> values;
    auto gen = CreateGeneratorN(10,
        [](int i) {
            return i;
        }     
    );

    values.insert(values.end(), gen.begin(), gen.end());
    REQUIRE(values.size() == 10);

    for (int i=0; i < 10; ++i) {
        REQUIRE(values[i] == i);
    }

}