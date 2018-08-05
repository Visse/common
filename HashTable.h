#pragma once

#include <type_traits>
#include <cstdint>

#include "build_config.h"
#include "PImplHelper.h"


namespace Common
{
    namespace internal {
        class HashTableBase {
            struct Impl;
            PImplHelper<Impl, 64> mImpl;

        public:
            COMMON_API explicit HashTableBase( int item_size, int initial_capacity );
            COMMON_API ~HashTableBase();

            HashTableBase() = delete;
            HashTableBase( const HashTableBase& ) = delete;
            HashTableBase& operator = ( const HashTableBase& ) = delete;

            COMMON_API bool insert( uint64_t hash, const void *item );
            COMMON_API void* find( uint64_t hash );
            COMMON_API void remove( uint64_t hash );

            COMMON_API void clear();
        };
    }

    template< typename Value >
    class HashTable :
        private internal::HashTableBase
    {
        static_assert(std::is_trivially_copyable<Value>::value, "Type must be trivially copyable!");
        static_assert(std::is_trivially_destructible<Value>::value, "Type must be trivially destructable!");

    public:
        HashTable() :
            HashTable(0)
        {}
        explicit HashTable( int initial_capacity ) :
            HashTableBase(sizeof(Value), initial_capacity)
        {}

        ~HashTable() = default;

        HashTable( const HashTable& ) = delete;
        HashTable& operator = ( const HashTable& ) = delete;

        // if hash exists, replace existing element
        bool insert( uint64_t hash, const Value &value ) {
            return HashTableBase::insert(hash, &value);
        }

        void remove( uint64_t hash ) {
            return HashTableBase::remove(hash);
        }

        Value* find( uint64_t hash ) {
            return (Value*)HashTableBase::find(hash);
        }

        void clear() {
            return HashTableBase::clear();
        }
    };
}