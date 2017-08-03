#include "StringId.h"
#include "strpool.h"


#include "ErrorUtils.h"

namespace Common
{
    static const size_t MAX_STRING_LEN = 1024;

    namespace
    {
        static struct StringPool {
            strpool_t pool;

            StringPool() {
                strpool_init(&pool, &strpool_default_config);
            }
            ~StringPool() {
                strpool_term(&pool);
            }

            operator strpool_t* () {
                return &pool;
            }
        } strpool;
    }

    namespace internal
    {
        void incRef( uint64_t id )
        {
            strpool_incref(strpool, id);
        }

        void decRef( uint64_t id )
        {
            int ret = strpool_decref(strpool, id);
            if (ret == 0) {
                strpool_discard(strpool, id);
            }
        }
    }

    StringId CreateStringId( const char *str, size_t len)
    {
        FATAL_ASSERT(len < MAX_STRING_LEN, "String is too long (%zu) - max is %zu", len, MAX_STRING_LEN);

        uint64_t handle = strpool_inject(strpool, str, (int)len);
        return StringId(handle);
    }
    
    const char* GetCString( StringId id )
    {
        const char *str = strpool_cstr(strpool, id);
        if (str) return str;
        return "";
    }
}