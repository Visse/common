#pragma once



namespace Common
{
    inline void* advance( void *ptr, ptrdiff_t amount ) 
    {
        return reinterpret_cast<void*>(
            reinterpret_cast<intptr_t>(ptr) + amount
        );
    }
    inline const void* advance( const void *ptr, ptrdiff_t amount ) 
    {
        return reinterpret_cast<const void*>(
            reinterpret_cast<intptr_t>(ptr) + amount
        );
    }

    inline ptrdiff_t difference( const void *p1, const void *p2 ) {
        return reinterpret_cast<intptr_t>(p1) - 
               reinterpret_cast<intptr_t>(p2);
    }

}