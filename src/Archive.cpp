#include "Archive.h"
#include "ErrorUtils.h"
#include "HandleVector.h"
#include "FileUtils.h"

#include "lz4hc.h"
#include "lz4frame.h"

#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <cassert>

namespace Common
{
#define ARCHIVE_SIGNATURE "Zodiac Archive"
#define ARCHIVE_SIGNATURE_LEN sizeof(ARCHIVE_SIGNATURE)

    static const int ArchiveVersion = 1;


    struct ArchiveHeader {
        uint8_t sig[ARCHIVE_SIGNATURE_LEN] = ARCHIVE_SIGNATURE;
        uint32_t version = ArchiveVersion;
        uint32_t fileCount = 0;
    };

    struct Archive::Impl {
        virtual ~Impl() = default;
        
        virtual ArchiveFileHandle openFile( const std::string &file ) 
        {
            return ArchiveFileHandle {};
        }

        virtual void closeFile( ArchiveFileHandle handle )
        {
        }

        virtual size_t fileSize( ArchiveFileHandle handle )
        {
            return 0;
        }

        virtual const void* mapFile( ArchiveFileHandle handle )
        {
            return nullptr;
        }

        virtual const char* name()
        {
            return "";
        }
    };
    
    template< typename Impl >
    Archive CreateArchive( Impl &&impl )
    {
        return Archive(std::move(impl));
    }

    template< typename Type >
    Archive::Archive( Type &&impl ) :
        mImpl(decltype(mImpl)::DerivedConstructorTag<Type>(), std::move(impl))
    {
    }

    namespace Directory
    {
        struct FileHandleInfo {
            std::string content;
        };

        struct Impl :
            public Archive::Impl
        {
            std::string path;

            HandleVector<ArchiveFileHandle, FileHandleInfo> fileHandles;

            virtual ArchiveFileHandle openFile( const std::string &file ) override
            {
                std::string fullPath = path + file;
                if (!FileUtils::isFile(fullPath)) {
                    return ArchiveFileHandle {};   
                }

                FileHandleInfo info;
                info.content = FileUtils::getFileContent(fullPath, true);
                return fileHandles.create(std::move(info));
            }

            virtual void closeFile( ArchiveFileHandle handle ) override
            {
                fileHandles.free(handle);
            }

            virtual size_t fileSize( ArchiveFileHandle handle ) override
            {
                const FileHandleInfo *info = fileHandles.find(handle);
                if (!info) return 0;
                
                return info->content.size();
            }

            virtual const void* mapFile( ArchiveFileHandle handle ) override
            {
                const FileHandleInfo *info = fileHandles.find(handle);
                if (!info) return nullptr;

                return info->content.data();
            }

            virtual const char* name() override
            {
                return path.c_str();
            }

        };

        Archive OpenArchive( const std::string &path )
        {
            Impl impl;
            impl.path = path + '/';
            return CreateArchive(std::move(impl));
        }
    }


    namespace Ver1 {
        static const uint32_t MAX_FILE_NAME_LENGHT = 256;
        static const uint32_t MAX_FILE_COUNT = 4096;
        static const uint64_t MAX_FILE_SIZE = 32 * 1024 * 1024; // 32 MiB
        static const uint64_t MAX_TOTAL_FILE_SIZE = 1024 * 1024*1024; // 1 GiB

        struct ArchiveFileTableEntry {
            char name[MAX_FILE_NAME_LENGHT] = {};
            uint64_t filesize = 0,
                     checksum = 0,
                     offset  = 0,
                     compressedSize = 0;
        };

        struct FileInfo {
            char name[MAX_FILE_NAME_LENGHT] = {};
            uint64_t offset = 0,
                     size = 0;
        };

        struct FileHandleInfo {
            uint32_t file;
        };

        struct Impl :
            public Archive::Impl
        {
            Impl() = default;
            Impl(Impl&&) = default;

            std::string path;
            ArchiveHeader header;
            std::vector<FileInfo> fileTable;
            std::vector<uint8_t> fileData;

            HandleVector<ArchiveFileHandle, FileHandleInfo> fileHandles;
            
            virtual ArchiveFileHandle openFile( const std::string &name ) override
            {
                uint32_t file = 0;
                for (const auto &info : fileTable) {
                    if (name == info.name) {
                        FileHandleInfo handle;
                        handle.file = file;
                        return fileHandles.create(handle);
                    }
                    file++;
                }
                return ArchiveFileHandle{};
            }

            virtual void closeFile( ArchiveFileHandle handle ) override
            {
                fileHandles.free(handle);
            }

            virtual size_t fileSize( ArchiveFileHandle handle ) override
            {
                const FileHandleInfo *info = fileHandles.find(handle);
                if (!info) return 0;

                assert(info->file < fileTable.size());
                return fileTable[info->file].size;
            }

            virtual const void* mapFile( ArchiveFileHandle handle ) override
            {
                const FileHandleInfo *info = fileHandles.find(handle);
                if (!info) return 0;

                assert(info->file < fileTable.size());
                const auto &fileInfo = fileTable[info->file];

                assert (fileInfo.offset < fileData.size() && (fileInfo.offset+fileInfo.size) < fileData.size());
                return fileData.data() + fileInfo.offset;
            }

            virtual const char* name() override
            {
                return path.c_str();
            }
        };
        
        Archive OpenArchive( const std::string &path, std::ifstream &file, ArchiveHeader header )
        {
            if (header.fileCount > MAX_FILE_COUNT) {
                LOG_ERROR("Too many files(%i) in archive \"%s\", limit is %i", (int)header.fileCount, path.c_str(), (int)MAX_FILE_COUNT);   
                return Archive {};
            }

            std::vector<ArchiveFileTableEntry> fileTable(MAX_FILE_COUNT);

            if (!file.read((char*)fileTable.data(), header.fileCount * sizeof(ArchiveFileTableEntry))) {
                LOG_ERROR("Failed to read file table from archive \"%s\"", path.c_str());
                return Archive {};
            }

            uint64_t fileSize = 0;
            {
                auto pos = file.tellg();
                file.seekg(std::ios::end);
                auto end = file.tellg();
                file.seekg(pos);

                fileSize = end - pos;

                if (!file) {
                    LOG_ERROR("Failed to retrive file size of archive \"%s\"", path.c_str());
                    return Archive {};
                }
            }

            uint64_t totalSize = 0, largestCompressedSize = 0;

            // Verify file table
            for (auto &entry : fileTable) {
                if (std::count(std::begin(entry.name), std::end(entry.name), '\0') == 0) {
                    LOG_ERROR("Invalid filetable in archive \"%s\" - entry has invalid name (misssing terminating '\\0'", path.c_str());
                    return Archive {};
                }

                if (entry.offset > fileSize || (entry.offset+entry.compressedSize) > fileSize) {
                    LOG_ERROR("Invalid filetable in archive \"%s\" - entry has invalid offset (%llu) and/or size (%llu)", path.c_str(), entry.offset, entry.compressedSize);
                    return Archive {};
                }

                if (entry.filesize > MAX_FILE_SIZE) {
                    LOG_ERROR("Invalid filetable in archive \"%s\" - entry has to big size (%llu) max is %zu", path.c_str(), entry.filesize);
                    return Archive {};
                }

                totalSize += entry.filesize;
                largestCompressedSize = std::max(largestCompressedSize, entry.compressedSize);
            }

            if (totalSize > MAX_TOTAL_FILE_SIZE) {
                LOG_ERROR("Invalid archive \"%s\" to big size %llu, max is %llu", path.c_str(), totalSize, MAX_TOTAL_FILE_SIZE);
                return Archive {};
            }
            std::vector<uint8_t> data(totalSize);
            uint64_t decompressedSize = 0;


            std::vector<FileInfo> files(fileTable.size());

            // Decompress the files
            auto tableEnd = file.tellg();
            std::vector<uint8_t> buffer(largestCompressedSize);

            for (size_t i=0, s=fileTable.size(); i < s; ++i) {
                const auto &entry = fileTable[i];
                auto &info = files[i];

                // entry.name has been validated to be nulled terminated 
                strncpy(info.name, entry.name, MAX_FILE_NAME_LENGHT);

                file.seekg(tableEnd + std::streamoff(entry.offset));
                if (!file.read((char*)buffer.data(), entry.compressedSize)) {
                    LOG_ERROR("Failed to read file (\"%s\") in archive \"%s\"", info.name, path.c_str());
                    return Archive {};
                }

                int ret = LZ4_decompress_safe((const char*)buffer.data(), (char*)(data.data()+decompressedSize), entry.compressedSize, entry.filesize);
                if (ret < 0) {
                    LOG_ERROR("Failed to decompress file (\"%s\") in archive \"%s\"", info.name, path.c_str());
                    return Archive {};
                }

                if (ret != entry.filesize) {
                    LOG_ERROR("Failed to decompress file (\"%s\") in archive \"%s\" - missmatch for expecet (%llu) and gotten size (%llu)", info.name, path.c_str(), entry.filesize, (uint64_t)ret);
                    return Archive {};
                }

                info.offset = decompressedSize;
                info.size = entry.filesize;
                decompressedSize += entry.filesize;
            }

            Impl impl;
            impl.path = path;
            impl.header = header;
            impl.fileTable = std::move(files);
            impl.fileData = std::move(data);

            return CreateArchive(std::move(impl));
        }
    }
    
    COMMON_API Archive Archive::OpenArchive( const std::string &path )
    {
        if (FileUtils::isDirectory(path)) {
            return Directory::OpenArchive(path);
        }

        std::ifstream file(path);
        if (!file) {
            LOG_ERROR("Failed to open file \"%s\"", path.c_str());
            return Archive {};
        }
        ArchiveHeader header;
        if (!file.read((char*)&header, sizeof(header))) {
            LOG_ERROR("Failed to read header from file \"%s\"", path.c_str());
            return Archive {};
        }

        if (memcmp(header.sig, ARCHIVE_SIGNATURE, ARCHIVE_SIGNATURE_LEN) != 0) {
            LOG_ERROR("Signature missmatch in header in file \"%s\"", path.c_str());
            return Archive {};
        }

        switch (header.version) {
        case 1:
            return Ver1::OpenArchive(path, file, header);
        }

        LOG_ERROR("Unkown archive version %i for file \"%s\"", header.version, path.c_str());
        return Archive {};
    }

    COMMON_API Archive::Archive() = default;
    COMMON_API Archive::Archive( Archive &&move ) = default;
    COMMON_API Archive::~Archive() = default;

    COMMON_API ArchiveFileHandleRAII Archive::openFile( const std::string &file )
    {
        return ArchiveFileHandleRAII(this, mImpl->openFile(file));
    }

    COMMON_API void Archive::closeFile( ArchiveFileHandle handle )
    {
        mImpl->closeFile(handle);
    }

    COMMON_API size_t Archive::fileSize( ArchiveFileHandle handle )
    {
        return mImpl->fileSize(handle);
    }

    COMMON_API const void* Archive::mapFile( ArchiveFileHandle handle )
    {
        return mImpl->mapFile(handle);
    }

    COMMON_API const char* Archive::name()
    {
        return mImpl->name();
    }
}
