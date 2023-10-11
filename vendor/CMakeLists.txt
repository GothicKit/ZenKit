include(FetchContent)

# This warning only occurs in CMake 3.24 or greater.
# https://cmake.org/cmake/help/latest/policy/CMP0135.html#cmp0135
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
cmake_policy(SET CMP0135 NEW)
endif ()

set(BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_SQUISH_WITH_OPENMP OFF CACHE BOOL "" FORCE)

function(px_add_dependency NAME URL URL_HASH)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}/CMakeLists.txt)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${NAME} EXCLUDE_FROM_ALL)
        set(${NAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${NAME} CACHE PATH "Path to the sources for the ${NAME} library." FORCE)
    else ()
        FetchContent_Declare(
                ${NAME}
                URL ${URL}
                URL_HASH SHA1=${URL_HASH}
        )

        if (NOT ${NAME}_POPULATED)
            message(STATUS "Downloading ${NAME}")
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif ()

        set(${NAME}_SOURCE_DIR ${${NAME}_SOURCE_DIR} CACHE PATH "Path to the sources for the ${NAME} library." FORCE)
    endif ()
endfunction()

px_add_dependency(doctest https://github.com/doctest/doctest/archive/refs/tags/v2.4.9.zip d1563419fa370c34c90e028c2e903a70c8dc07b2)
px_add_dependency(mio https://github.com/mandreyel/mio/archive/3f86a95c0784d73ce6815237ec33ed25f233b643.zip 62a0e43b07a6b66e415ad01f4b225c72a3c9935b)
px_add_dependency(glm https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip 45408897f419944fb28d8fd835791f237be2ec19)
px_add_dependency(libsquish https://github.com/lmichaelis/phoenix-libsquish/archive/cc82beff55210816e1bd531fc6057203dc309807.zip 953f5cd072cd6674d1aeaff5ff91225f2197283c)

# msvc: disable -wno-* flags
if (NOT MSVC)
    target_compile_options(mio INTERFACE -Wno-conversion)
    target_compile_options(squish PRIVATE -Wno-unused-but-set-variable)
endif ()

target_include_directories(squish PUBLIC ${libsquish_SOURCE_DIR})
