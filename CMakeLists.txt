cmake_minimum_required(VERSION 3.15)
project(phoenix)

option(PX_BUILD_TOOLS "Build phoenix tool executables" ON)
option(PX_BUILD_ENGINE "Build the phoenix engine library" ON)

add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(engine)
add_subdirectory(vendor)