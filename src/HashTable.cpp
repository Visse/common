#include "HashTable.h"

#include "mgn/hashtable.h"

#include <cstring>

namespace Common
{
    namespace internal
    {
        struct HashTableBase::Impl {
            hashtable_t hashtable;
        };

        COMMON_API HashTableBase::HashTableBase( int item_size, int initial_capacity )
        {
            hashtable_init(&mImpl->hashtable, item_size, initial_capacity, nullptr);
        }
        
        COMMON_API HashTableBase::~HashTableBase()
        {
            hashtable_term(&mImpl->hashtable);
        }

        COMMON_API bool HashTableBase::insert(uint64_t hash, const void * item)
        {
            if (void *tmp = find(hash)) {
                std::memcpy(tmp, item, mImpl->hashtable.item_size);
                return false;
            }
            hashtable_insert(&mImpl->hashtable, hash, item);
            return true;
        }

        COMMON_API void* HashTableBase::find(uint64_t hash)
        {
            return hashtable_find(&mImpl->hashtable, hash);
        }

        COMMON_API void HashTableBase::remove(uint64_t hash)
        {
            return hashtable_remove(&mImpl->hashtable, hash);
        }

        COMMON_API void HashTableBase::clear() 
        {
            return hashtable_clear(&mImpl->hashtable);
        }
    }
}
