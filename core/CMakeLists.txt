cmake_minimum_required(VERSION 3.15)
project(phoenix::core)
include(CheckIncludeFiles)

option(PX_CORE_BUILD_TESTS "Build core library tests" ON)

set(PX_CORE_SRCS)
set(PX_CORE_TESTS)
set(PX_CORE_DEFINES)

# Check for POSIX I/O headers
check_include_files("sys/mman.h;unistd.h;fcntl.h;sys/stat.h" HAS_POSIX_IO)
if (HAS_POSIX_IO)
	set(PX_CORE_DEFINES ${OZ_DEFINES} PX_POSIX_MMAP=1)
endif ()

add_library(phoenix-core ${PX_CORE_SRCS})
target_include_directories(phoenix-core PUBLIC include)
target_link_libraries(phoenix-core PUBLIC glm)

if (PX_CORE_BUILD_TESTS)
	enable_testing()

	add_executable(phoenix-core-tests ${PX_CORE_TESTS})
	target_link_libraries(phoenix-core-tests PRIVATE phoenix-core)

	add_test(NAME phoenix-core-test COMMAND $<TARGET_FILE:phoenix-core-tests> WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif ()