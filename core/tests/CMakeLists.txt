add_executable(TestCoreTests
            unit/TestCoreTests.cpp
            ../source/TestCore.cpp
)

target_include_directories(TestCoreTests PUBLIC
        ../include
        ../public
)
target_link_libraries(TestCoreTests PUBLIC
        gtest_main
        gmock_main
        SmartHomeTypes
)
add_test(NAME TestCoreTests COMMAND TestCoreTests)
