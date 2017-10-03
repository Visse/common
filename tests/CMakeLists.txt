

option(BUILD_TESTS "Build tests" ON)

if (${BUILD_TESTS})


function( create_test name )
    add_executable( ${name} ${ARGN} )
    add_test( NAME ${name} COMMAND ${name} )
    target_link_libraries( ${name} catch Common )
    
    set_target_properties( ${name} PROPERTIES
       FOLDER "Tests"
       EXCLUDE_FROM_ALL TRUE
       RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:Common>
    )
    if (TARGET BUILD_TESTS)
        add_dependencies(BUILD_TESTS ${name})
    else (TARGET BUILD_TESTS)
        add_custom_target(BUILD_TESTS DEPENDS ${name})
        set_target_properties( BUILD_TESTS PROPERTIES
           FOLDER "Tests"
           EXCLUDE_FROM_ALL TRUE
           EXCLUDE_FROM_DEFAULT_BUILD TRUE
        )
        add_test(NAME BuildTests
                 COMMAND ${CMAKE_COMMAND} --build . --target BUILD_TESTS --config $<CONFIGURATION>
                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    endif (TARGET BUILD_TESTS)
    set_tests_properties( ${name} PROPERTIES
        DEPENDS BuildTests
    )
endfunction( create_test )


source_group( " " REGULAR_EXPRESSION ./* )

create_test( ArchiveTest ArchiveTest.cpp )
target_link_libraries( ArchiveTest Common )
set_tests_properties( ArchiveTest PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ArchiveTestData/
)

create_test( GeneratorN GeneratorN.cpp )
create_test( HandleVector HandleVector.cpp )
create_test( Variant Variant.cpp )
create_test(EnumString EnumString.cpp)
create_test(PImplHelper PImplHelper.cpp)
create_test(FlatMap FlatMap.cpp)
create_test(HashTable HashTable.cpp)
create_test(IntrusiveList IntrusiveList.cpp)
create_test(BuddyAllocator BuddyAllocator.cpp)


add_custom_command(TARGET BUILD_TESTS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:Common>
    $<TARGET_FILE_DIR:ArchiveTest>
)
        
endif (${BUILD_TESTS})

