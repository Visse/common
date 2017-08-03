#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/Archive.h"


TEST_CASE( "Archive Directory", "[Common][Archive]" )
{
    using namespace Common;

    Archive archive = Archive::OpenArchive("dir");

    SECTION("File - \"hello world.txt\"")
    {
        auto helloWorldHandle = archive.openFile("hello world.txt");
        REQUIRE((bool)helloWorldHandle);

        static const char *HELLO_WORLD_CONTENT = "Hello World";
        static const size_t HELLO_WORLD_CONTENT_LEN = strlen(HELLO_WORLD_CONTENT);

        size_t size = archive.fileSize(helloWorldHandle);
        const void *ptr = archive.mapFile(helloWorldHandle);

        REQUIRE(size == HELLO_WORLD_CONTENT_LEN);
        REQUIRE(memcmp(ptr, HELLO_WORLD_CONTENT, HELLO_WORLD_CONTENT_LEN) == 0);
    }

    SECTION("File - \"big file.txt\"")
    {
        auto bigFileHandle = archive.openFile("big file.txt");
        REQUIRE((bool)bigFileHandle);

        size_t size = archive.fileSize(bigFileHandle);
        const void *ptr = archive.mapFile(bigFileHandle);

        REQUIRE(size == 7888885);
    }
    
    SECTION("File - \"don't exists.txt\"")
    {
        auto handle = archive.openFile("don't exists.txt");
        REQUIRE((bool)handle == false);
    }

}
