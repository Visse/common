#pragma once

#include "PImplHelper.h"
#include "HandleType.h"
#include "build_config.h"

#include <string>

namespace Common
{
    MAKE_HANDLE(ArchiveFileHandle, uint32_t);

    class Archive;
    struct ArchiveFileHandleRAII {
        Archive *archive = nullptr;
        ArchiveFileHandle handle;

        ArchiveFileHandleRAII() = delete;
        ArchiveFileHandleRAII( const ArchiveFileHandleRAII& ) = delete;
        ArchiveFileHandleRAII& operator = ( const ArchiveFileHandleRAII &) = delete;

        ArchiveFileHandleRAII( Archive *archive_, ArchiveFileHandle handle_ ) :
            archive(archive_),
            handle(handle_)
        {}

        ArchiveFileHandleRAII( ArchiveFileHandleRAII &&move ) :
            archive(move.archive),
            handle(move.handle)
        {
            move.archive = nullptr;
        }

        operator bool () {
            return (bool)handle;
        }

        ~ArchiveFileHandleRAII();

        operator ArchiveFileHandle () && {
            archive = nullptr;
            return handle;
        }
    };

    class Archive {
    public:

    public:
        static COMMON_API Archive OpenArchive( const std::string &path );

    public:
        COMMON_API Archive();
        COMMON_API Archive( Archive &&move );
        COMMON_API ~Archive();

        COMMON_API ArchiveFileHandleRAII openFile( const std::string &file );
        COMMON_API void closeFile( ArchiveFileHandle handle );

        COMMON_API size_t fileSize( ArchiveFileHandle handle );
        COMMON_API const void* mapFile( ArchiveFileHandle handle );

        size_t fileSize( ArchiveFileHandleRAII &handle ) {
            return fileSize(handle.handle);
        }
        const void* mapFile( ArchiveFileHandleRAII &handle ) {
            return mapFile(handle.handle);
        }
    
        COMMON_API const char* name();

    private:
        template< typename Type >
        friend Archive CreateArchive( Type &&impl );

        template< typename Type >
        Archive( Type &&impl );

    public:
        struct Impl;

    private:
        PImplHelper<Impl, 256, alignof(void*), PImplHelperFlags::AllocDerivedMove> mImpl;
    };

    inline ArchiveFileHandleRAII::~ArchiveFileHandleRAII() {
        if (archive) archive->closeFile(handle);
    }
}