cmake_minimum_required(VERSION 3.10)
project(phoenix.tools.zvdfs VERSION 0.1.0)


execute_process(
		COMMAND git log -1 --format=%h
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
		OUTPUT_VARIABLE GIT_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hh.in ${CMAKE_CURRENT_BINARY_DIR}/config.hh @ONLY)

add_executable(zvdfs main.cc)
target_link_libraries(zvdfs PRIVATE phoenix flags)
target_include_directories(zvdfs PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(zvdfs
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		)
