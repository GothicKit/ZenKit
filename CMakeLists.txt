cmake_minimum_required(VERSION 3.15)
project(phoenix)

option(PX_BUILD_TOOLS "Build phoenix tool executables" ON)
option(PX_BUILD_ENGINE "Build the phoenix engine library" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize=undefined -fsanitize=leak ${CMAKE_CXX_FLAGS}")

	if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug")
	endif()
endif()


add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(engine)
add_subdirectory(vendor)


add_executable(test test.cc)
target_link_libraries(test phoenix-core)
