include(FetchContent)

option(BUILD_STATIC_LIBS ON)
option(BUILD_SHARED_LIBS OFF)
 

# add doctest
FetchContent_Declare(
        doctest
        URL https://github.com/doctest/doctest/archive/refs/tags/v2.4.9.zip
        URL_HASH SHA1=d1563419fa370c34c90e028c2e903a70c8dc07b2
)

if (NOT doctest_POPULATED)
    message(STATUS "Downloading doctest")
    FetchContent_Populate(doctest)
    add_subdirectory(${doctest_SOURCE_DIR} ${doctest_BINARY_DIR})
endif ()
 
  
# add mio
FetchContent_Declare(
        mio
        URL https://github.com/mandreyel/mio/archive/3f86a95c0784d73ce6815237ec33ed25f233b643.zip
        URL_HASH SHA1=62a0e43b07a6b66e415ad01f4b225c72a3c9935b
)

if (NOT mio_POPULATED)
    message(STATUS "Downloading mio")
    FetchContent_Populate(mio)
    add_subdirectory(${mio_SOURCE_DIR} ${mio_BINARY_DIR})
endif ()


# add lexy
FetchContent_Declare(
        lexy
        URL https://github.com/foonathan/lexy/releases/download/v2022.05.1/lexy-src.zip
        URL_HASH SHA1=252185ebbf004047725f9ed98b4e8b6e4aa06d98
)

if (NOT lexy_POPULATED)
    message(STATUS "Downloading lexy")
    FetchContent_Populate(lexy)
    add_subdirectory(${lexy_SOURCE_DIR} ${lexy_BINARY_DIR})
endif ()


# add fmt
FetchContent_Declare(
        fmt
        URL https://github.com/fmtlib/fmt/releases/download/9.1.0/fmt-9.1.0.zip
        URL_HASH SHA1=8de922ba88fccaec1e7778bc069f342084573486
)

if (NOT fmt_POPULATED)
    message(STATUS "Downloading fmt")
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif ()


# add glm
FetchContent_Declare(
        glm
        URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
        URL_HASH SHA1=45408897f419944fb28d8fd835791f237be2ec19
)

if (NOT glm_POPULATED)
    message(STATUS "Downloading glm")
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif ()


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
    message(STATUS "Downloading squish")
    FetchContent_Populate(libsquish)

    SET(BUILD_SQUISH_WITH_OPENMP OFF CACHE BOOL "" FORCE)
    add_subdirectory(${libsquish_SOURCE_DIR} ${libsquish_BINARY_DIR})
    target_include_directories(squish PUBLIC ${libsquish_SOURCE_DIR})

    # msvc: disable -wno-* flags
    if (NOT MSVC)
        target_compile_options(squish PRIVATE -Wno-unused-but-set-variable)
    endif ()
endif ()
