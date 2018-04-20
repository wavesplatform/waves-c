project( waves-c )
cmake_minimum_required( VERSION 3.1 )

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

# Search OpenSSL
find_package(OpenSSL REQUIRED)
if( OpenSSL_FOUND )
    list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
    message(SEND_ERROR "Failed to find OpenSSL")
    return()
endif()

file( GLOB CS "crypto/*.c" "crypto/curve25519-donna/*.c" "crypto/base58/*.c" "crypto/blake2b/sse/*.c" ) #"crypto/ed25519-donna/*.c"

HEADER_DIRECTORIES(HDIRS)
include_directories(${HDIRS} ${OPENSSL_INCLUDE_DIR})
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse2 -m64")
add_executable( waves_vanity ${CS} waves.c vanity.c )
target_link_libraries(waves_vanity pthread )
add_executable( waves_tests ${CS} waves.c tests.c )
add_definitions(-Wall -O3)
install(TARGETS waves_vanity DESTINATION bin)
