add_executable(FanModuleTests
            source/FanModuleTests.cpp
)

target_include_directories(FanModuleTests PUBLIC
)
target_link_libraries(FanModuleTests PUBLIC
        gtest_main
        gmock_main
        TestCore
)

add_test(NAME FanModuleTests COMMAND FanModuleTests)

###############################

add_executable(SlmModuleTests
            source/SlmModuleTests.cpp
)

target_include_directories(SlmModuleTests PUBLIC
)
target_link_libraries(SlmModuleTests PUBLIC
        gtest_main
        gmock_main
        TestCore
)

add_test(NAME SlmModuleTests COMMAND SlmModuleTests)

###############################