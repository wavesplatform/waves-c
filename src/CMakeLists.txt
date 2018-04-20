project( waves-c )

file( GLOB CS "crypto/*.c" "crypto/curve25519-donna/*.c" "crypto/base58/*.c" "crypto/blake2b/sse/*.c" ) #"crypto/ed25519-donna/*.c"

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-msse2 -m64")
add_library( waves_c STATIC ${CS} )
add_definitions(-Wall -O3)

add_executable( waves_vanity vanity.c )
target_link_libraries(waves_vanity pthread waves_c)
install(TARGETS waves_vanity DESTINATION bin)