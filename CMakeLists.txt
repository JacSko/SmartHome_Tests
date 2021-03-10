#
#	SmartHome_tests project is a test framework, which includes either core sources and test cases.
#
#	GoogleTest framework is used as test executor.
#
#######################################################################################################
cmake_minimum_required(VERSION 3.10)

project(smarthome_tests)

add_definitions(-DPROJECT_ROOT_PATH=\"${PROJECT_SOURCE_DIR}\" -DSIMULATION)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test_executables)
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fno-exceptions -fprofile-arcs -ftest-coverage -fPIC")
enable_testing()

add_subdirectory(core)
add_subdirectory(external/SmartHome_API)
add_subdirectory(external/googletest)
add_subdirectory(test_suites)
