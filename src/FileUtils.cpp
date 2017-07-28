#include "FileUtils.h"
#include "Throw.h"

#include <fstream>
#include <stdexcept>
#include <memory>
#include <vector>

#include <sys/stat.h>

#include "tinydir.h"



// Windows doesn't seem to define there macros :/
#ifndef S_ISREG   
#   define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#   define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif


namespace FileUtils
{
    bool exists( const std::string &path )
    {
        struct stat s;
        stat( path.c_str(), &s );
        
        return S_ISREG(s.st_mode) | S_ISDIR(s.st_mode);
    }
    
    bool isFile( const std::string &path )
    {
        struct stat s;
        stat( path.c_str(), &s );
        
        return S_ISREG(s.st_mode);
    }
    
    bool isDirectory( const std::string &path )
    {
        struct stat s;
        stat( path.c_str(), &s );
        
        return S_ISDIR(s.st_mode);
    }

    std::string getFileContent( const std::string &filename, bool isBinary )
    {
        std::ios::openmode mode = std::ios::in | std::ios::ate;
        if( isBinary ) {
            mode |= std::ios::binary;
        }

        std::ifstream file( filename, mode );
        if( !file ) {
            THROW( std::runtime_error,
                "Failed to open file \"%s\"", filename.c_str()
            );
        }

        std::size_t fileSize = 0;
        {
            // std::ios::ate seeks to the end on open.
            auto end = file.tellg();
            file.seekg( 0, std::ios::beg );
            auto beg = file.tellg();

            fileSize = (std::size_t)(end - beg);
        }

        std::string content;
        content.resize( fileSize );

        file.read( &content[0], fileSize );

        // We may have read less that fileSize bytes (on windows the newline '\n' is actually two characters on disk)
        content.resize(file.gcount());


        return std::move(content);
    }

    std::vector<std::string> getFileList( const std::string &path )
    {
        tinydir_dir dir;
        if (tinydir_open(&dir, path.c_str()) == -1) {
            THROW(std::runtime_error,
                "tinydir_open failed to open directory \"%s\" error %i: %s", path.c_str(), errno, strerror(errno)     
            );
        };

        std::vector<std::string> files;
        while (dir.has_next) {
            tinydir_file file;

            if (tinydir_readfile(&dir, &file) == -1) {
                THROW(std::runtime_error, "tinydir_readfile error %i: %s", errno, strerror(errno));
            }
            else if (file.is_reg) {
                files.emplace_back(file.path);
            }

            if (tinydir_next(&dir) == -1) {
                THROW(std::runtime_error, "tinydir_next error %i: %s", errno, strerror(errno));
            }
        }

        tinydir_close(&dir);
        return std::move(files);
    }

    std::string removeFileName( const std::string &filepath ) {
        auto loc = filepath.find_last_of("/\\");
        if( loc == std::string::npos ) return std::string();

        return filepath.substr(0,loc+1);
    }

    std::string removePath( const std::string &filepath )
    {
        auto loc = filepath.find_last_of("/\\");
        if( loc == std::string::npos ) return filepath;
        return filepath.substr(loc+1);
    }

    std::string extension( const std::string &filepath )
    {
        // we search for '\' and '/' to make sure that the '.' is part of the filename
        auto loc = filepath.find_last_of( "/\\." );
        if( loc == std::string::npos ) return std::string();

        if( filepath[loc] != '.' ) return std::string();
        return filepath.substr(loc+1);
    }

    std::string stripExtension( const std::string &filepath )
    {
        auto loc = filepath.find_last_of( "/\\." );
        if( loc == std::string::npos ) return filepath;

        if( filepath[loc] != '.' ) return filepath;
        return filepath.substr( 0, loc );
    }

    /// @todo implement real file mapping
    struct Mapping {
        FileMapping map;
        std::vector<uint8_t> content;
    };

    FileMapping* mapFile( const std::string &filename, int offset, int size )
    {
        std::string content = getFileContent( filename, true );

        std::unique_ptr<Mapping> mapping( new Mapping );
        mapping->content.assign( content.begin(), content.end() );
        mapping->map.ptr = mapping->content.data();
        mapping->map.size = mapping->content.size();
        
        return &mapping.release()->map;
    }

    void unmapFile( FileMapping* ptr )
    {
        Mapping *mapping = reinterpret_cast<Mapping*>(ptr);
        delete mapping;
    }
}
