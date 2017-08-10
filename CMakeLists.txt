

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

    GeneratorN.h
    BinaryFind.h

    Variant.h

    Archive.h
    src/Archive.cpp

    MemStreamBuf.h
    BuiltinCast.h
    EnumString.h
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
