file(GLOB CS "crypto/*.c" "crypto/base58/*.c" "crypto/base64/*.c"
        "crypto/blake2b/sse/*.c" "crypto/transactions/*.c" "crypto/libcurve25519-donna/*.c" "crypto/libcurve25519-donna/additions/*.c"
        "crypto/libcurve25519-donna/nacl_includes/*.c"  "crypto/libcurve25519-donna/nacl_sha512/*.c")

set(CPACK_SET_DESTDIR "on")
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -msse2 -m64 -fPIC")

add_library(waves_c STATIC ${CS} crypto/utils.h crypto/utils.c crypto/utils.h)
add_library(waves_c_shared SHARED ${CS} crypto/utils.h crypto/utils.c crypto/utils.h)
set_target_properties(waves_c_shared PROPERTIES OUTPUT_NAME waves_c)

target_link_libraries(waves_c ${OPENSSL_LIBRARIES} m)
target_link_libraries(waves_c_shared ${OPENSSL_LIBRARIES} m)
add_definitions(-Wall -O3 -Werror -Wextra -Wno-format)

install(FILES crypto/base58/b58.h crypto/base64/b64.h DESTINATION include/waves)
install(FILES crypto/crypto.h DESTINATION include/waves)
file(GLOB TX_FILES "crypto/transactions/*_tx.h")
install(FILES ${TX_FILES} crypto/transactions/tx_common.h crypto/transactions/tx.h DESTINATION include/waves)

install(TARGETS waves_c waves_c_shared
        ARCHIVE DESTINATION lib/waves
        LIBRARY DESTINATION lib/waves
        PUBLIC_HEADER DESTINATION include/waves
)

add_executable( waves_vanity vanity.c )
target_link_libraries(waves_vanity pthread waves_c)
#install(TARGETS waves_vanity DESTINATION bin)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION_STRING})
set(CPACK_PACKAGE_RELEASE 2)
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "support@wavesplatform.com")
set(CPACK_PACKAGE_VENDOR "Waves Platform")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libssl1.1")

set(CPACK_RPM_PACKAGE_RELOCATABLE ON)
set(CPACK_RPM_PACKAGE_AUTOREQ "no")
set(CPACK_RPM_PACKAGE_AUTOREQPROV "no")
set(CPACK_RPM_PACKAGE_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_RPM_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_RPM_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_RPM_PACKAGE_VERSION "${PROJECT_VERSION_STRING}")
set(CPACK_RPM_PACKAGE_RELEASE ${CPACK_PACKAGE_RELEASE})
set(CPACK_RPM_PACKAGE_LICENSE "ASL 2.0")
set(CPACK_RPM_PACKAGE_GROUP Development/Tools)
set(CPACK_RPM_PACKAGE_VENDOR "${CPACK_PACKAGE_VENDOR}")

include(CPack)
