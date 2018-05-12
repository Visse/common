#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#define ENUM_STRING_IMPL

#include "Common/DynamicBitUtilty.h"


using namespace Common::DynamicBitUtility;

TEST_CASE( "DynamicBitUtilty", "[Common][DynamicBitUtilty]" )
{
    uint32_t field[32];
    const unsigned count = 32 * 32;

    SECTION("Patern fill")
    {
        // i = size of pattern
        //   i==1 => 01010101
        //   i==2 => 00110011
        //   i==3 => 00011100
        // etc
        for (int i=1; i < count; ++i) {
            bool val = true;
            for (int c=0; c < count; ++c) {
                if (c%i) val = !val;

                setBit(field, c, val);
            }

            val = true;
            for (int c=0; c < count; ++c) {
                if (c%i) val = !val;

                REQUIRE(getBit(field, c) == (val ? 1 : 0));
                REQUIRE(swapBit(field, c, val ? 0 : 1) == (val ? 1 : 0));
            }

        
            val = false;
            for (int c=0; c < count; ++c) {
                if (c%i) val = !val;

                REQUIRE(getBit(field, c) == (val ? 1 : 0));
                setBit(field, c, !val);
                REQUIRE(getBit(field, c) == (val ? 0 : 1));
            }
        }
    }
}