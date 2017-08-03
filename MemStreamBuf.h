#pragma once

#include <streambuf>

namespace Common
{
    class MemStreamBuf :
        public std::streambuf
    {
    public:
        MemStreamBuf( const void *ptr, size_t size ) {
            char *cptr = reinterpret_cast<char*>(const_cast<void*>(ptr));
            setg(cptr, cptr, cptr+size);
        }
    };
}