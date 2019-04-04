project( waves-c )

file(GLOB CS "crypto/*.c" "crypto/base58/*.c"
        "crypto/blake2b/sse/*.c" "crypto/transactions/*.c" "crypto/libcurve25519-donna/*.c" "crypto/libcurve25519-donna/additions/*.c"
        "crypto/libcurve25519-donna/nacl_includes/*.c"  "crypto/libcurve25519-donna/nacl_sha512/*.c")

file(GLOB HS "crypto/*.h" "crypto/base58/*.h"
    "crypto/blake2b/sse/*.h" "crypto/transactions/*.h" "crypto/libcurve25519-donna/*.h" "crypto/libcurve25519-donna/additions/*.h"
    "crypto/libcurve25519-donna/nacl_includes/*.h"  "crypto/libcurve25519-donna/nacl_sha512/*.h")

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse2 -m64")
add_library(waves_c STATIC ${CS} crypto/utils.h crypto/utils.c crypto/utils.h)
target_link_libraries(waves_c ${OPENSSL_LIBRARIES} m crypto)
add_definitions(-Wall -O3 -Werror -Wextra -Wno-format)

foreach (file ${HS} )
    get_filename_component(dir ${file} DIRECTORY )
    STRING(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/src" "" rel_dir_path ${dir})
    install(FILES ${file} DESTINATION include/waves/${rel_dir_path} )
endforeach()

install(TARGETS waves_c
        ARCHIVE DESTINATION lib/waves
        PUBLIC_HEADER DESTINATION include/waves
)

add_executable( waves_vanity vanity.c )
target_link_libraries(waves_vanity pthread waves_c)
#install(TARGETS waves_vanity DESTINATION bin)

SET(CPACK_SET_DESTDIR "on")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_NAME "waves-c-dev")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "C library for working with Waves")
set(CPACK_DEBIAN_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}.${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_amd64.deb")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libssl1.1")
set(CPACK_PACKAGE_DESCRIPTION "C library for working with Waves")
set(CPACK_PACKAGE_CONTACT "Alexander Golenev alex.golenev@gmail.com")

include(CPack)
