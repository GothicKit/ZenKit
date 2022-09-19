include(FetchContent)

option(BUILD_STATIC_LIBS ON)
option(BUILD_SHARED_LIBS OFF)

add_subdirectory(glm)
add_subdirectory(fmt)
add_subdirectory(flags)
add_subdirectory(doctest)
add_subdirectory(mio)
add_subdirectory(lexy)

# msvc: disable -wno-* flags
if (NOT MSVC)
    target_compile_options(mio INTERFACE -Wno-conversion)
endif ()

# add libsquish
FetchContent_Declare(
        libsquish
        URL https://downloads.sourceforge.net/project/libsquish/libsquish-1.15.tgz
        URL_HASH SHA1=51844b9a8bc815a27e2cc0ffbede5fee3ef75110
)

if (NOT libsquish_POPULATED)
    FetchContent_Populate(libsquish)

    SET(BUILD_SQUISH_WITH_OPENMP OFF CACHE BOOL "" FORCE)
    add_subdirectory(${libsquish_SOURCE_DIR} ${libsquish_BINARY_DIR})
    target_include_directories(squish PUBLIC ${libsquish_SOURCE_DIR})

    # msvc: disable -wno-* flags
    if (NOT MSVC)
        target_compile_options(squish PRIVATE -Wno-unused-but-set-variable)
    endif ()
endif ()

# add stb as a library
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)
