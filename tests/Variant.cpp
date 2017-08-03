#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/Variant.h"

static int Instances = 0;
struct TestType1 {
    void *this_ = nullptr;
    int value = -1;
    TestType1() {
        REQUIRE(this_ == nullptr);
        this_ = this;
        Instances++;
    }

    TestType1(int a) {
        REQUIRE(this_ == nullptr);
        this_ = this;
        Instances++;

        value = a;
    }
    TestType1(int a, int b) {
        REQUIRE(this_ == nullptr);
        this_ = this;
        Instances++;

        value = a * b;
    }

    ~TestType1() {
        REQUIRE(this_ == this);
        this_ = nullptr;

        REQUIRE(Instances > 0);
        Instances--;
    }
};

struct TestType2 {
    void *this_ = nullptr;
    double value = -1;
    TestType2() {
        REQUIRE(this_ == nullptr);
        this_ = this;
        Instances++;
    }

    TestType2(double a) {
        REQUIRE(this_ == nullptr);
        this_ = this;
        Instances++;

        value = a;
    }
    TestType2(double a, double b) {
        REQUIRE(this_ == nullptr);
        this_ = this;
        Instances++;

        value = a * b;
    }

    ~TestType2() {
        REQUIRE(this_ == this);
        this_ = nullptr;

        REQUIRE(Instances > 0);
        Instances--;
    }
};

MAKE_VARIANT(TestVariant, 
    (Type1, TestType1),
    (Type2, TestType2)
);

static_assert( std::is_same<TestType1, TestVariant::underlaying_type<TestVariant::Type1>>::value, "" );
static_assert( std::is_same<TestType2, TestVariant::underlaying_type<TestVariant::Type2>>::value, "" );

TEST_CASE( "Variant", "[Common][Variant]" )
{
    Instances = 0;
    {
        TestVariant variant;
        REQUIRE(variant.type() == TestVariant::None);

        variant.setType1(1);
        REQUIRE(variant.type() == TestVariant::Type1);
        REQUIRE(variant.getType1().value == 1);
        REQUIRE(Instances == 1);

        variant.setType1(5, 10);
        REQUIRE(variant.type() == TestVariant::Type1);
        REQUIRE(variant.getType1().value == 50);
        REQUIRE(Instances == 1);

        variant.setType2(10, 20);
        REQUIRE(variant.type() == TestVariant::Type2);
        REQUIRE(std::abs(variant.getType2().value-200.0) < 1e-10);
        REQUIRE(Instances == 1);


        REQUIRE_THROWS(variant.getType1());


        TestVariant variant1(TestVariant::Type1, 1, 50);
        REQUIRE(variant1.type() == TestVariant::Type1);
        REQUIRE(variant1.getType1().value == 50);
        REQUIRE(Instances == 2);

        TestVariant variant2(TestVariant::Type2, 1, 50);
        REQUIRE(variant2.type() == TestVariant::Type2);
        REQUIRE((variant2.getType2().value-50.0) < 1e-10);
        REQUIRE(Instances == 3);
    }
}
