set(UNIT_TEST waves_tests)
project(${UNIT_TEST})

file( GLOB CS "*.c" "transactions/*.c" )

add_executable( ${UNIT_TEST} ${CS} tests_main.c utils_tests.h)
target_link_libraries(${UNIT_TEST} waves_c)
add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "Run unit tests"
     POST_BUILD
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^${UNIT_TEST}$" --output-on-failures
)
