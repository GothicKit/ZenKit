include(CheckSymbolExists)
include(CheckCSourceCompiles)

## Compute the compile and link flags for the current compiler.
##
## Args:
##   SANITIZERS(bool): Whether to enable sanitizers or not.
##
## Returns:
##   COMPILE(string): A string containing the compiler flags, separated by spaces
##   LINK(string): A string containing the linker flags, separated by spaces
function(bs_select_cflags SANITIZERS COMPILE LINK)
    if (MSVC)
        bs_internal_select_cflags_msvc(${SANITIZERS} _INTERNAL_COMPILE_FLAGS _INTERNAL_LINK_FLAGS)
    elseif(CMAKE_C_COMPILER_ID MATCHES "^Clang")
        bs_internal_select_cflags_clang(${SANITIZERS} _INTERNAL_COMPILE_FLAGS _INTERNAL_LINK_FLAGS)
    elseif(CMAKE_C_COMPILER_ID MATCHES "^GCC")
        bs_internal_select_cflags_gcc(${SANITIZERS} _INTERNAL_COMPILE_FLAGS _INTERNAL_LINK_FLAGS)
    endif()

    # return _INTERNAL_COMPILE_FLAGS, _INTERNAL_LINK_FLAGS;
    set(${COMPILE} ${_INTERNAL_COMPILE_FLAGS} PARENT_SCOPE)
    set(${LINK} ${_INTERNAL_LINK_FLAGS} PARENT_SCOPE)
    return()
endfunction()

## Compute the compile and link flags for MSVC
##
## Args:
##   SANITIZERS(bool): Whether to enable sanitizers or not.
##
## Returns:
##   COMPILE(list): A list containing the compiler flags
##   LINK(list): A list containing the linker flags
function(bs_internal_select_cflags_msvc SANITIZERS COMPILE LINK)
    list(APPEND _INTERNAL_COMPILE_FLAGS "/W4")

    if (CMAKE_BUILD_TYPE MATCHES "^Debug" AND ${SANITIZERS})
        list(APPEND _INTERNAL_COMPILE_FLAGS "/fsanitize=address")
    endif()

    # return _INTERNAL_COMPILE_FLAGS, _INTERNAL_LINK_FLAGS;
    set(${COMPILE} ${_INTERNAL_COMPILE_FLAGS} PARENT_SCOPE)
    set(${LINK} ${_INTERNAL_LINK_FLAGS} PARENT_SCOPE)
    return()
endfunction()

## Compute the compile and link flags for GCC
##
## Args:
##   SANITIZERS(bool): Whether to enable sanitizers or not.
##
## Returns:
##   COMPILE(list): A list containing the compiler flags
##   LINK(list): A list containing the linker flags
function(bs_internal_select_cflags_gcc SANITIZERS COMPILE LINK)
    list(APPEND _INTERNAL_FLAGS "-Wall" "-Wextra" "-Werror" "-Wconversion" "-Wshadow" "-Wpedantic" "-fPIC" "-Wno-deprecated-declarations")

    if (CMAKE_BUILD_TYPE MATCHES "^Debug" AND ${SANITIZERS} AND NOT WIN32)
        list(APPEND _INTERNAL_FLAGS "-fsanitize=address" "-fsanitize=undefined" "-fsanitize=leak")
    endif()

    # return _INTERNAL_FLAGS, _INTERNAL_FLAGS;
    set(${COMPILE} ${_INTERNAL_FLAGS} PARENT_SCOPE)
    set(${LINK} ${_INTERNAL_FLAGS} PARENT_SCOPE)
    return()
endfunction()

## Compute the compile and link flags for Clang
##
## Args:
##   SANITIZERS(bool): Whether to enable sanitizers or not.
##
## Returns:
##   COMPILE(list): A list containing the compiler flags
##   LINK(list): A list containing the linker flags
function(bs_internal_select_cflags_clang SANITIZERS COMPILE LINK)
    bs_internal_select_cflags_gcc(${SANITIZERS} _INTERNAL_COMPILE_FLAGS _INTERNAL_LINK_FLAGS)

    if (CMAKE_BUILD_TYPE MATCHES "^Debug")
        list(APPEND _INTERNAL_COMPILE_FLAGS "-fstandalone-debug")
        list(APPEND _INTERNAL_LINK_FLAGS "-fstandalone-debug")
    endif()

    # return _INTERNAL_COMPILE_FLAGS, _INTERNAL_LINK_FLAGS;
    set(${COMPILE} ${_INTERNAL_COMPILE_FLAGS} PARENT_SCOPE)
    set(${LINK} ${_INTERNAL_LINK_FLAGS} PARENT_SCOPE)
    return()
endfunction()

function(bs_check_posix_mmap _MMAP_AVAIL)
    check_c_source_compiles("
    #include <sys/fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
    int main(int argc, char** argv) {
		int handle = open(argv[1], O_RDONLY);
		struct stat st;
		fstat(handle, &st);
		void* data = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, handle, 0);
		close(handle);
        munmap(data, st.st_size);
        return 0;
    }" _HAS_POSIX_MMAP)

    # return _HAS_POSIX_MMAP
    set(${_MMAP_AVAIL} ${_HAS_POSIX_MMAP} PARENT_SCOPE)
    return()
endfunction()

function(bs_check_win32_mmap _MMAP_AVAIL)
    check_c_source_compiles("
    #include <windows.h>
    int main(int argc, char** argv) {
        HANDLE hFile = CreateFile(L\"test.txt\", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        DWORD dwFileSize = GetFileSize(hFile, NULL);
        HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL);
        LPVOID lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
        return 0;
    }" _HAS_WIN32_MMAP)

    # return _HAS_WIN32_MMAP
    set(${_MMAP_AVAIL} ${_HAS_WIN32_MMAP} PARENT_SCOPE)
    return()
endfunction()
