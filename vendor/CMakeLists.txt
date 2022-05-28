include(FetchContent)

add_subdirectory(glm)
add_subdirectory(fmt)
add_subdirectory(argh)
add_subdirectory(doctest)
add_subdirectory(mio)

target_compile_options(mio INTERFACE -Wno-conversion)

FetchContent_Declare(
		libsquish
		URL       https://downloads.sourceforge.net/project/libsquish/libsquish-1.15.tgz
		URL_HASH  SHA1=51844b9a8bc815a27e2cc0ffbede5fee3ef75110
)

if(NOT libsquish_POPULATED)
	FetchContent_Populate(libsquish)

	SET(BUILD_SQUISH_WITH_OPENMP OFF CACHE BOOL "" FORCE)
	add_subdirectory(${libsquish_SOURCE_DIR} ${libsquish_BINARY_DIR})
	target_include_directories(squish PUBLIC ${libsquish_SOURCE_DIR})
	target_compile_options(squish PRIVATE -Wno-unused-but-set-variable)
endif()

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)
