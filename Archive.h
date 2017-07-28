#pragma once

#include "PImplHelper.h"
#include "HandleType.h"

#include <string>

namespace Common
{
    MAKE_HANDLE(ArchiveFileHandle, uint32_t);

    class Archive {
    public:

    public:
        static Archive OpenArchive( const std::string &path );

    public:
        Archive();
        Archive( Archive &&move );
        ~Archive();

        ArchiveFileHandle openFile( const std::string &file );
        void closeFile( ArchiveFileHandle handle );

        size_t fileSize( ArchiveFileHandle handle );
        const void* mapFile( ArchiveFileHandle handle );
    
    private:
        template< typename Type >
        friend Archive CreateArchive( Type &&impl );

        template< typename Type >
        Archive( Type &&impl );

    public:
        struct Impl;


        PImplHelper<Impl, 136> mImpl;
    };
}