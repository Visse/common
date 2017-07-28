#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace FileUtils
{
    
    bool exists( const std::string &path );
    bool isFile( const std::string &path );
    bool isDirectory( const std::string &path );
    

    struct FileMapping {
        const void *ptr = nullptr;
        size_t size = 0;
    };
    FileMapping* mapFile( const std::string &filename, int offset=0, int size=-1 );
    void unmapFile( FileMapping *ptr );
    
    std::string getFileContent( const std::string &filename, bool isBinary );

    std::vector<std::string> getFileList( const std::string &path );

    // removes filename from path -
    // "hello/world" => "hello/"
    // "hello" => ""
    // "hello/" => "hello/"
    std::string removeFileName( const std::string &filepath );

    // removes the path
    std::string removePath( const std::string &filepath );


    std::string extension( const std::string &filepath );
    std::string stripExtension( const std::string &filepath );

}
