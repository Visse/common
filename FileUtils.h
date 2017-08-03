#pragma once

#include <string>
#include <cstdint>
#include <vector>

#include "build_config.h"

namespace FileUtils
{
    
    COMMON_API bool exists( const std::string &path );
    COMMON_API bool isFile( const std::string &path );
    COMMON_API bool isDirectory( const std::string &path );
    

    struct FileMapping {
        const void *ptr = nullptr;
        size_t size = 0;
    };
    COMMON_API FileMapping* mapFile( const std::string &filename, int offset=0, int size=-1 );
    COMMON_API void unmapFile( FileMapping *ptr );
    
    COMMON_API std::string getFileContent( const std::string &filename, bool isBinary );

    COMMON_API std::vector<std::string> getFileList( const std::string &path );

    // removes filename from path -
    // "hello/world" => "hello/"
    // "hello" => ""
    // "hello/" => "hello/"
    COMMON_API std::string removeFileName( const std::string &filepath );

    // removes the path
    COMMON_API std::string removePath( const std::string &filepath );


    COMMON_API std::string extension( const std::string &filepath );
    COMMON_API std::string stripExtension( const std::string &filepath );

}
