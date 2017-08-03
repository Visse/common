#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/HandleVector.h"
#include "Common/HandleType.h"

#include <map>


TEST_CASE( "HandleVector", "[Common][HandleVector]" )
{
    static int ValueInstances = 0,
               CopiesMade = 0,
               MovesMade = 0,
               CopyAssignsMade = 0,
               MoveAssignsMade = 0;
    ValueInstances = 0;
    CopiesMade = 0;
    MovesMade = 0;
    CopyAssignsMade = 0;
    MoveAssignsMade = 0;

    struct Value {
        Value() = delete;
        Value( uint32_t value ) :
            val(value)
        {
            ValueInstances++;
            this_ = this;
        }
        ~Value()
        {
            ValueInstances--;
            REQUIRE(this_ == this);
        }

        Value( const Value& copy ) :
            val(copy.val)
        {
            ValueInstances++;
            this_ = this;
            REQUIRE(copy.this_ == &copy);

            CopiesMade++;
        }
        Value( Value&& move ) :
            val(move.val)
        {
            ValueInstances++;
            this_ = this;
            REQUIRE(move.this_ == &move);

            MovesMade++;
        }

        Value& operator = ( const Value &copy ) 
        {
            REQUIRE(this_ == this);
            REQUIRE(copy.this_ == &copy);
            val = copy.val;
            CopyAssignsMade++;
            return *this;
        }

        Value& operator = ( Value &&move ) 
        {
            REQUIRE(this_ == this);
            REQUIRE(move.this_ == &move);
            val = move.val;
            MoveAssignsMade++;
            return *this;
        }

        uint32_t val;
        void *this_;
    };

    MAKE_HANDLE( TestHandle, uint32_t );

    SECTION("Core functionallity")
    {
        HandleVector<TestHandle, Value> vector;
        std::map<TestHandle, uint32_t> handles;

        SECTION("Creating & freeing handles")
        {
            std::set<TestHandle> oldHandles;
            size_t size = 0;
            for (int a=0; a < 20; ++a) {
                for (int i = 0; i < 1000; ++i) {
                    uint32_t value = i;
                    TestHandle handle = vector.create(value);
                    REQUIRE((bool)handle);
                    REQUIRE(handles.count(handle) == 0);
                    handles.emplace( handle, value );

                    REQUIRE(oldHandles.count(handle) == 0);
                }

                for (auto val : handles) {
                    REQUIRE(vector.valid(val.first) == true);
                }

                for (auto val : handles) {
                    REQUIRE(vector.free(val.first) == true);
                }

                for (auto val : handles) {
                    REQUIRE(vector.valid(val.first) == false);
                    oldHandles.insert(val.first);
                }
                handles.clear();

                if (size == 0) size = vector.underlying_size();
                // require that no more memory has been allocated (it has been reused)
                REQUIRE(size <= vector.underlying_size());
            }
        }

        SECTION("Getting & setting")
        {
            // create the test handles
            for (int i = 0; i < 1000; ++i) {
                Value value = i;
                TestHandle handle = vector.create(value);
                REQUIRE((bool)handle);
                REQUIRE(handles.count(handle) == 0);
                handles.emplace(handle, value.val);
            }

            size_t size = vector.underlying_size();
            for (int a=0; a < 20; ++a) {
                for (auto &val : handles) {
                    Value value((uint32_t)-1);
                    REQUIRE(vector.get(val.first, value) == true);
                    REQUIRE(value.val == val.second);
                    val.second = value.val*(a+1);
                    REQUIRE(vector.set(val.first, val.second) == true);
                }

                // require that no more memory has been allocated
                REQUIRE(size == vector.underlying_size());
            }
        }

        SECTION("Invalid handles")
        {
            auto requireInvalid = [&] (TestHandle handle) {
                REQUIRE(vector.valid(handle) == false);
                Value value((uint32_t)-1);
                REQUIRE(vector.get(handle,value) == false);
                REQUIRE(vector.set(handle,value) == false);
                REQUIRE(vector.free(handle) == false);
            };
            auto requireValid = [&] (TestHandle handle) {
                REQUIRE(vector.valid(handle) == true);
                Value value((uint32_t)-1);
                REQUIRE(vector.get(handle,value) == true);
                REQUIRE(vector.set(handle,value) == true);
            };

            // create the test handles
            for (int i = 0; i < 1000; ++i) {
                Value value((uint32_t)-1);
                TestHandle handle = vector.create(value);
                REQUIRE((bool)handle);
                REQUIRE(handles.count(handle) == 0);
                handles.emplace(handle, value.val);
            }

            requireInvalid(TestHandle());
            for (auto &val : handles) {
                requireValid(val.first);
                Value value((uint32_t)-1);
                REQUIRE(vector.get(val.first, value) == true);
                REQUIRE(value.val == val.second);
                REQUIRE(vector.free(val.first) == true);
                requireInvalid(val.first);
            }
        }

        SECTION("Generation bits")
        { 
            // test how many handles we can recreate before we get a old handle
            const unsigned count = (1<<vector.GenerationBits) * 16;
            unsigned i = 0;
            while(true) {
                TestHandle handle = vector.create(0);
                if(handles.count(handle) > 0) break;
                handles.emplace(handle, 0);
                vector.free(handle);
                ++i;
            }

            INFO("Iterations before handle repeat: "<< i << " - effective generation bits: " << std::log(i)/std::log(2) << ", expected " << vector.GenerationBits);
            REQUIRE((1<<vector.GenerationBits) < i);
        }

        SECTION("DataValue") 
        {
            HandleVector<TestHandle, uint32_t, HANDLE_VECTOR_DEFAULT, 2, 0> v0;
            HandleVector<TestHandle, uint32_t, HANDLE_VECTOR_DEFAULT, 2, 1> v1;
            HandleVector<TestHandle, uint32_t, HANDLE_VECTOR_DEFAULT, 2, 2> v2;
            HandleVector<TestHandle, uint32_t, HANDLE_VECTOR_DEFAULT, 2, 3> v3;


            for (int i=0; i < 1000; ++i) {
                uint32_t value;
                TestHandle handle = v0.create(0);
                REQUIRE(v0.valid(handle) == true);
                REQUIRE(v1.valid(handle) == false);
                REQUIRE(v2.valid(handle) == false);
                REQUIRE(v3.valid(handle) == false);

                REQUIRE(v0.get(handle,value) == true);
                REQUIRE(v1.get(handle,value) == false);
                REQUIRE(v2.get(handle,value) == false);
                REQUIRE(v3.get(handle,value) == false);

                REQUIRE(v0.set(handle,value) == true);
                REQUIRE(v1.set(handle,value) == false);
                REQUIRE(v2.set(handle,value) == false);
                REQUIRE(v3.set(handle,value) == false);

                REQUIRE(v0.set(handle,value) == true);
                REQUIRE(v1.set(handle,value) == false);
                REQUIRE(v2.set(handle,value) == false);
                REQUIRE(v3.set(handle,value) == false);

                REQUIRE(v0.free(handle) == true);
                REQUIRE(v1.free(handle) == false);
                REQUIRE(v2.free(handle) == false);
                REQUIRE(v3.free(handle) == false);
            }
        }
    
        SECTION("FindIf") {
            // create the test handles
            for (int i = 0; i < 1000; ++i) {
                Value value = i;
                TestHandle handle = vector.create(value);
                REQUIRE((bool)handle);
                handles.emplace(handle, value.val);
            }
            
            for (int i = 0; i < 1000; ++i) {
                TestHandle handle = vector.findIf(
                    [i]( Value value ) {
                        return value.val == i;                
                    }
                );
                REQUIRE(handles.count(handle) == 1);
                REQUIRE(handles[handle] == i);
            }
        }
    }



    REQUIRE(ValueInstances == 0);
}