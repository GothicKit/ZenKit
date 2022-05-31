cmake_minimum_required(VERSION 3.15)
include(CheckIncludeFiles)
project(phoenix)

option(PX_BUILD_TOOLS "Build phoenix tool executables" ON)
option(PX_BUILD_TESTS "Build core library tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(PX_CXX_FLAGS -Wall -Wextra -Werror -Wconversion)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(PX_CXX_FLAGS ${PX_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -fsanitize=leak)

	if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		set(PX_CXX_FLAGS ${PX_CXX_FLAGS} -fstandalone-debug)
	endif()
endif()

set(PX_SOURCES
		source/detail/compat.cc
		source/detail/buffer.cc

		source/archive/archive_ascii.cc
		source/archive/archive_binary.cc
		source/archive/archive_binsafe.cc
		source/archive.cc

		source/vdfs.cc
		source/material.cc
		source/font.cc
		source/animation.cc
		source/texture.cc
		source/proto_mesh.cc
		source/messages.cc
		source/mesh.cc
		source/morph_mesh.cc
		source/model_hierarchy.cc
		source/softskin_mesh.cc
		source/model_mesh.cc
		source/model.cc
		source/model_script.cc

		source/world.cc
		source/world/bsp_tree.cc
		source/world/vob_tree.cc
		source/world/way_net.cc

		source/daedalus/script.cc
		source/daedalus/interpreter.cc)

set(PX_TESTS
		tests/test_compat.cc
		tests/test_buffer.cc
		tests/test_archive.cc
		tests/test_material.cc
		tests/test_vdfs.cc
		tests/test_font.cc
		tests/test_animation.cc
		tests/test_texture.cc
		tests/test_script.cc
		tests/test_proto_mesh.cc
		tests/test_world.cc
		tests/test_messages.cc
		tests/test_morph_mesh.cc
		tests/test_model_hierarchy.cc
		tests/test_model_mesh.cc
		tests/test_model.cc
		tests/test_model_script.cc)

add_library(phoenix ${PX_SOURCES})
target_include_directories(phoenix PUBLIC include)
target_link_libraries(phoenix PUBLIC glm fmt squish mio)
target_compile_options(phoenix PRIVATE ${PX_CXX_FLAGS})
target_link_options(phoenix PUBLIC ${PX_CXX_FLAGS})

if (PX_BUILD_TESTS)
	enable_testing()

	add_executable(phoenix-tests ${PX_TESTS})
	target_link_libraries(phoenix-tests PRIVATE phoenix doctest_with_main)
	target_compile_options(phoenix-tests PRIVATE ${PX_CXX_FLAGS})
	target_link_options(phoenix-tests PRIVATE ${PX_CXX_FLAGS})

	add_test(NAME phoenix-test COMMAND $<TARGET_FILE:phoenix-tests> --check WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif ()


add_subdirectory(tools)
add_subdirectory(vendor)
