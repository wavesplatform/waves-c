project( waves-c )

file(GLOB CS "crypto/*.c" "crypto/base58/*.c"
        "crypto/blake2b/sse/*.c" "crypto/transactions/*.c" "crypto/libcurve25519-donna/*.c" "crypto/libcurve25519-donna/additions/*.c"
        "crypto/libcurve25519-donna/nacl_includes/*.c"  "crypto/libcurve25519-donna/nacl_sha512/*.c")

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse2 -m64")
add_library(waves_c STATIC ${CS} crypto/utils.h crypto/utils.c crypto/utils.h)
target_link_libraries(waves_c ${OPENSSL_LIBRARIES} m)
add_definitions(-Wall -O3 -Werror -Wextra -Wno-format)

add_executable( waves_vanity vanity.c )
target_link_libraries(waves_vanity pthread waves_c)
install(TARGETS waves_vanity DESTINATION bin)