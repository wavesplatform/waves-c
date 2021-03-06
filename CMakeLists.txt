cmake_minimum_required(VERSION 3.9)
project(waves-c
    VERSION 0.2.3
    DESCRIPTION "C library for working with Waves"
    LANGUAGES C)
set(PROJECT_VERSION_NUMBER 0x000203)
set(PROJECT_VERSION_STRING "${PROJECT_VERSION}")

option(CPACK_GENERATOR "CPack generator" "RPM DEB TGZ ZIP")

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

HEADER_DIRECTORIES(HDIRS)
include_directories(${HDIRS})

# put all builded stuff to bin
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib
        CACHE PATH
        "Single Directory for all"
    )

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
    include_directories(${OPENSSL_INCLUDE_DIR})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else(OpenSSL_FOUND)
    message(SEND_ERROR "Failed to find OpenSSL")
    return()
endif(OpenSSL_FOUND)

enable_testing()
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DWAVES_DEBUG")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

add_subdirectory(src build/src)
add_subdirectory(test build/test)
