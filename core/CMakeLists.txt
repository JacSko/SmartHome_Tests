if (NOT UNIT_TESTS)

add_library(Logger STATIC
		source/Logger.cpp
)
target_include_directories(Logger PUBLIC
	include
	public
)


endif()



if (UNIT_TESTS)

add_subdirectory(test)

endif()