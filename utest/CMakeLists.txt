cmake_minimum_required(VERSION 2.8)

OPTION(BUILD_DMLIST_TEST "Set to OFF to disable"			ON)
OPTION(BUILD_DMTIMER_TEST "Set to OFF to disable"			ON)

find_package(cmocka REQUIRED)

include_directories(
	${CMOCKA_INCLUDE_DIRS}
)

if(BUILD_DMLIST_TEST)
	set(DMLIST_UTEST "dmlist_utest")
	add_executable(${DMLIST_UTEST} "dmlist_utest.c")
	target_link_libraries(${DMLIST_UTEST} ${CMOCKA_LIBRARIES})
	add_dependencies(build_tests ${DMLIST_UTEST})
	add_test(${DMLIST_UTEST} ${DMLIST_UTEST})
endif(BUILD_DMLIST_TEST)

if(BUILD_DMTIMER_TEST)
	set(DMTIMER_UTEST "dmtimer_utest")
	add_executable(${DMTIMER_UTEST} "dmtimer_utest.c")
	target_link_libraries(${DMTIMER_UTEST} ${CMOCKA_LIBRARIES} dmcom)
	add_dependencies(build_tests ${DMTIMER_UTEST})
	add_test(${DMTIMER_UTEST} ${DMTIMER_UTEST})
endif(BUILD_DMTIMER_TEST)

