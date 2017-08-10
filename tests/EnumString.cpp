#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#define ENUM_STRING_IMPL

#include "Common/EnumString.h"

enum class TestEnum 
{
    Value1,
    Value2,
    Value3,
};

DECL_ENUM_TO_FROM_STRING(TestEnum,,
    (Value1),
    (Value2, "ValueTwo", "Two"),
    (Value3, "three")
)


void testString( TestEnum value, const char *str )
{
    TestEnum val;
    REQUIRE(TestEnumFromString(str, strlen(str), val));
    REQUIRE(val == value);
}


TEST_CASE( "EnumFromString", "[Common][EnumString]" )
{
    testString(TestEnum::Value1, "Value1");
    testString(TestEnum::Value1, "vAlUe1"); // case insensitive
    testString(TestEnum::Value2, "value2");
    testString(TestEnum::Value2, "valuetwo");
    testString(TestEnum::Value2, "Two");
    testString(TestEnum::Value3, "Value3");
    testString(TestEnum::Value3, "Three");


    TestEnum value;
    REQUIRE_FALSE(TestEnumFromString("Test", value));
    REQUIRE_FALSE(TestEnumFromString("Value", value));
    REQUIRE_FALSE(TestEnumFromString("Value12", value));
    
}


TEST_CASE( "EnumToString", "[Common][EnumString]" )
{
    auto equal = []( const char *lhs, const char *rhs ) -> bool {
        return strcmp(lhs, rhs) == 0;
    };

    REQUIRE(equal(TestEnumToString(TestEnum::Value1), "Value1"));
    REQUIRE(equal(TestEnumToString(TestEnum::Value2), "Value2"));
    REQUIRE(equal(TestEnumToString(TestEnum::Value3), "Value3"));
    REQUIRE(equal(TestEnumToString((TestEnum)123), "Invalid TestEnum"));
}
