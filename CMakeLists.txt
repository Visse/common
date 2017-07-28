

source_group( " " REGULAR_EXPRESSION ./* )
source_group( "src" REGULAR_EXPRESSION ./src/* )

include_directories(.)
add_library( Common SHARED
    Throw.h
    HandleType.h
    EnumFlagOp.h
    
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

    Color.h
    src/Color.cpp

    CommandLineParse.h
    src/CommandLineParse.cpp

    BuiltinFromString.h
    src/BuiltinFromString.cpp

    GeneratorN.h
    BinaryFind.h

    Variant.h

    Archive.h
    src/Archive.cpp
)
target_link_libraries(Common
    PRIVATE libstb
    PRIVATE tinydir
    PRIVATE lz4
)
