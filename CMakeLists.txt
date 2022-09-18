cmake_minimum_required(VERSION 3.10)
include(CheckIncludeFiles)
project(phoenix)

option(PX_BUILD_TOOLS "Build phoenix tool executables" ON)
option(PX_BUILD_TESTS "Build core library tests" ON)
option(PX_BUILD_EXAMPLES "Build example code" OFF)
option(PX_NO_SANITIZERS "Build without sanitizers in debug mode" OFF)
set(PX_LOG_LEVEL "INFO" CACHE STRING "The logging level to use for phoenix. Set to DEBUG, INFO, WARN or ERROR")

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
	# TODO: enable warnings as errors
	set(PX_CXX_FLAGS "/W4")

	if (${CMAKE_BUILD_TYPE} MATCHES "Debug" AND NOT ${PX_NO_SANITIZERS})
		set(PX_CXX_FLAGS ${PX_CXX_FLAGS} "/fsanitize=address")
	endif ()
else ()
	set(PX_CXX_FLAGS -Wall -Wextra -Werror -Wconversion)

	if (${CMAKE_BUILD_TYPE} MATCHES "Debug" AND NOT ${PX_NO_SANITIZERS})
		set(PX_CXX_FLAGS ${PX_CXX_FLAGS} -fsanitize=address -fsanitize=undefined)

		if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
			set(PX_CXX_FLAGS ${PX_CXX_FLAGS} -fstandalone-debug)
		endif ()

		if (NOT APPLE)
			set(PX_CXX_FLAGS ${PX_CXX_FLAGS} -fsanitize=leak)
		endif ()
	endif ()
endif ()

if (PX_LOG_LEVEL MATCHES "DEBUG")
	set(PX_DEFINES PX_LOG_LEVEL_DEBUG=1)
elseif (PX_LOG_LEVEL MATCHES "INFO")
	set(PX_DEFINES PX_LOG_LEVEL_INFO=1)
elseif (PX_LOG_LEVEL MATCHES "WARN")
	set(PX_DEFINES PX_LOG_LEVEL_WARN=1)
elseif (PX_LOG_LEVEL MATCHES "ERROR")
	set(PX_DEFINES PX_LOG_LEVEL_ERROR=1)
endif ()

set(PX_SOURCES
		source/phoenix.cc
		source/buffer.cc

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

		source/vobs/camera.cc
		source/vobs/light.cc
		source/vobs/misc.cc
		source/vobs/mob.cc
		source/vobs/sound.cc
		source/vobs/trigger.cc
		source/vobs/vob.cc
		source/vobs/zone.cc

		source/daedalus/script.cc
		source/daedalus/vm.cc

		source/ext/dds_convert.cc
		source/ext/daedalus_classes.cc)

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


file(GLOB_RECURSE PX_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hh")

add_library(phoenix ${PX_SOURCES} ${PX_HEADERS})
target_include_directories(phoenix PUBLIC include)
target_link_libraries(phoenix PUBLIC glm fmt squish mio lexy)
target_compile_definitions(phoenix PUBLIC ${PX_DEFINES})
target_compile_options(phoenix PRIVATE ${PX_CXX_FLAGS})
target_link_options(phoenix PUBLIC ${PX_CXX_FLAGS})

set_target_properties(phoenix
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		)

if (PX_BUILD_TESTS)
	enable_testing()
	include(vendor/doctest/scripts/cmake/doctest.cmake)

	add_executable(phoenix-tests ${PX_TESTS})
	target_link_libraries(phoenix-tests PRIVATE phoenix doctest_with_main)
	target_compile_options(phoenix-tests PRIVATE ${PX_CXX_FLAGS})
	target_link_options(phoenix-tests PRIVATE ${PX_CXX_FLAGS})

	doctest_discover_tests(phoenix-tests EXTRA_ARGS -tse=messages,script,world WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/tests)
endif ()

if (PX_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

add_subdirectory(tools)
add_subdirectory(vendor)
