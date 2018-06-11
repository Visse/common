

source_group( " " REGULAR_EXPRESSION ./* )
source_group( "src" REGULAR_EXPRESSION ./src/* )

include_directories(.)
add_library( Common SHARED
    Throw.h
    HandleType.h
    EnumFlagOp.h
    build_config.h
    
    StringFormat.h
    src/StringFormat.cpp

    Clock.h
    src/Clock.cpp

    ErrorUtils.h
    src/ErrorUtils.cpp

    FileUtils.h
    src/FileUtils.cpp

    Murmur3_32.h
    PointerHelpers.h

    FileParse.h

    HandleVector.h

    PImplHelper.h

    PPUtils.h

    StringEqual.h
    src/StringEqual.cpp

    DataUnion.h

    StringId.h
    src/StringId.cpp

    Color.h
    src/Color.cpp

    CommandLineParse.h

    BuiltinFromString.h
    src/BuiltinFromString.cpp
    FromString.h

    GeneratorN.h
    BinaryFind.h

    Variant.h

    Archive.h
    src/Archive.cpp

    MemStreamBuf.h
    BuiltinCast.h
    EnumString.h

    FlatMap.h

    HashTable.h
    src/HashTable.cpp

    IntrusiveList.h

    BuddyAllocator.h
    src/BuddyAllocator.cpp

    CreateDefaultOperators.h

    Trace.h

    uint32_log2.h

    BlockAllocator.h
    src/BlockAllocator.cpp

    IAllocator.h

    DynamicBitUtilty.h

    HalfEdgeMesh.h
    src/HalfEdgeMesh.impl.h
    src/HalfEdgeMesh.cpp

    HalfEdgeMeshIterator.h
    src/HalfEdgeMeshIterator.cpp

    IteratorAdopter.h
)
target_link_libraries(Common
    PRIVATE stb
    PRIVATE tinydir
    PRIVATE lz4
    PRIVATE mgn
)

target_compile_definitions(Common
    PUBLIC COMMON_BUILT_SHARED
    PRIVATE COMMON_BUILDING
)


file(GLOB COMMON_HEADERS ./*.h)
install( TARGETS Common
    EXPORT Zodiac
    RUNTIME DESTINATION bin/$<CONFIG>
    ARCHIVE DESTINATION lib/$<CONFIG>
    INCLUDES DESTONATION include
)
install(FILES ${COMMON_HEADERS} DESTINATION include/Common)


add_subdirectory(tests)
