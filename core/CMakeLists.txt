cmake_minimum_required(VERSION 3.15)
project(phoenix::core)
include(CheckIncludeFiles)

option(PX_CORE_BUILD_TESTS "Build core library tests" ON)

set(PX_CORE_SRCS
		source/detail/stream.cc
		source/detail/compat.cc

		source/archive/archive_ascii.cc
		source/archive/archive_binary.cc
		source/archive/archive_binsafe.cc
		source/archive.cc

		source/vdfs.cc
		source/material.cc
		source/font.cc
		source/animation.cc)

set(PX_CORE_TESTS
		tests/test_runner.cc
		tests/test_stream.cc
		tests/test_compat.cc
		tests/test_archive.cc
		tests/test_material.cc
		tests/test_vdfs.cc
		tests/test_font.cc
		tests/test_animation.cc)

set(PX_CORE_DEFINES)

# Check for POSIX I/O headers
check_include_files("sys/mman.h;unistd.h;fcntl.h;sys/stat.h" HAS_POSIX_IO)
if (HAS_POSIX_IO)
	set(PX_CORE_DEFINES ${OZ_DEFINES} PX_POSIX_MMAP=1)
endif ()

add_library(phoenix-core ${PX_CORE_SRCS})
target_include_directories(phoenix-core PUBLIC include)
target_link_libraries(phoenix-core PUBLIC glm fmt)
target_compile_definitions(phoenix-core PUBLIC ${PX_CORE_DEFINES})

if (PX_CORE_BUILD_TESTS)
	enable_testing()

	add_executable(phoenix-core-tests ${PX_CORE_TESTS})
	target_link_libraries(phoenix-core-tests PRIVATE phoenix-core)

	add_test(NAME phoenix-core-test COMMAND $<TARGET_FILE:phoenix-core-tests> --check WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif ()