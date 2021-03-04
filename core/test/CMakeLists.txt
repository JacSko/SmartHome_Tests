add_executable(SocketDriverTests
            unit/SocketDriverTests.cpp
            ../source/SocketDriver.cpp
)

target_include_directories(SocketDriverTests PUBLIC
        ../include
        ../public
)
target_link_libraries(SocketDriverTests PUBLIC
        gtest_main
        gmock_main
        SmartHomeTypes
        loggerMock
)

add_test(NAME SocketDriverTests COMMAND SocketDriverTests)





####
#	MOCKS
####

add_library(loggerMock STATIC
			mocks/logger_mock.hpp
)
target_include_directories(loggerMock PUBLIC
            mocks
            ../include
)
set_target_properties(loggerMock PROPERTIES LINKER_LANGUAGE CXX)