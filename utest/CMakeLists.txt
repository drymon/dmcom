cmake_minimum_required(VERSION 2.8)

find_package(cmocka REQUIRED)

set(DMLIST_UTEST "dmlist_utest")
add_executable(dmlist_utest "dmlist_utest.c")
target_link_libraries(${DMLIST_UTEST} ${CMOCKA_LIBRARIES})
add_dependencies(build_tests ${DMLIST_UTEST})
add_test(${DMLIST_UTEST} ${DMLIST_UTEST})
