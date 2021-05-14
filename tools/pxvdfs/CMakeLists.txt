cmake_minimum_required(VERSION 3.15)
project(phoenix::tools::pxvdfs VERSION 0.1.0)


execute_process(
		COMMAND git log -1 --format=%h
		WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
		OUTPUT_VARIABLE GIT_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hh.in ${CMAKE_CURRENT_BINARY_DIR}/config.hh @ONLY)

add_executable(pxvdfs main.cc)
target_link_libraries(pxvdfs PRIVATE phoenix-core argh)
target_include_directories(pxvdfs PRIVATE ${CMAKE_CURRENT_BINARY_DIR})