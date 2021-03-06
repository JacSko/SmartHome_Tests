#
#	SmartHome_tests project is a test framework, which includes either core sources and test cases.
#
#	GoogleTest framework is used as test executor.
#
#######################################################################################################
cmake_minimum_required(VERSION 3.10)

project(smarthome_tests)


# Builds normal tests, that cooperates with SmartHome binary
if (NOT UNIT_TESTS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test_suites)

add_executable(main
		test_suites/main.cpp)
target_link_libraries(main PUBLIC
		Logger
		TestSubjectExecutor
		SocketDriver
)

endif()


# Builds unit tests of core test framework implementation
if (UNIT_TESTS)

	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fno-exceptions -fprofile-arcs -ftest-coverage -fPIC")
	add_definitions(
		-DUNIT_TESTS
		)
	enable_testing()

endif()



add_subdirectory(core)
add_subdirectory(external/SmartHome_API)
add_subdirectory(external/googletest)