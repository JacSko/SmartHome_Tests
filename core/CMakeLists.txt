add_library(Logger STATIC
		source/Logger.cpp
)
target_include_directories(Logger PUBLIC
	include
	public
)

add_library(TestSubjectExecutor STATIC
		source/TestSubjectExecutor.cpp
)
target_include_directories(TestSubjectExecutor PUBLIC
	include
	public
)

add_library(SocketDriver STATIC
		source/SocketDriver.cpp
)
target_include_directories(SocketDriver PUBLIC
	include
	public
)
target_link_libraries(SocketDriver PUBLIC
	Logger
	SmartHomeTypes
	pthread
)

add_library(TestCore STATIC
		source/TestCore.cpp
)
target_include_directories(TestCore PUBLIC
	include
	public
)
target_link_libraries(TestCore PUBLIC
	Logger
	SmartHomeTypes
	TestSubjectExecutor
	SocketDriver
)
