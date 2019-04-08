project( waves-c )

file(GLOB CS "crypto/*.c" "crypto/base58/*.c"
        "crypto/blake2b/sse/*.c" "crypto/transactions/*.c" "crypto/libcurve25519-donna/*.c" "crypto/libcurve25519-donna/additions/*.c"
        "crypto/libcurve25519-donna/nacl_includes/*.c"  "crypto/libcurve25519-donna/nacl_sha512/*.c")

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse2 -m64 -fPIC")
add_library(waves_c STATIC ${CS} crypto/utils.h crypto/utils.c crypto/utils.h)
add_library(waves_c_shared SHARED ${CS} crypto/utils.h crypto/utils.c crypto/utils.h)
set_target_properties(waves_c_shared PROPERTIES OUTPUT_NAME waves_c)

target_link_libraries(waves_c ${OPENSSL_LIBRARIES} m crypto)
add_definitions(-Wall -O3 -Werror -Wextra -Wno-format)

install(FILES crypto/base58/libbase58.h DESTINATION include/waves/ RENAME b58.h)
install(FILES crypto/waves_crypto.h DESTINATION include/waves/ RENAME crypto.h)

install(TARGETS waves_c waves_c_shared
        ARCHIVE DESTINATION lib/waves
        LIBRARY DESTINATION lib/waves
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
