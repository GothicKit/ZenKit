cmake_minimum_required(VERSION 3.10)
project(ZenKit VERSION 1.3.0)

set(CMAKE_CXX_STANDARD 20)

option(ZK_BUILD_EXAMPLES "ZenKit: Build the examples." OFF)
option(ZK_BUILD_TESTS "ZenKit: Build the test suite." ON)
option(ZK_BUILD_SHARED "ZenKit: Build a shared library." OFF)

option(ZK_ENABLE_ASAN "ZenKit: Enable sanitizers in debug builds." ON)
option(ZK_ENABLE_DEPRECATION "ZenKit: Enable deprecation warnings." ON)
option(ZK_ENABLE_INSTALL "ZenKit: Enable CMake install target creation." ON)
option(ZK_ENABLE_MMAP "ZenKit: Build ZenKit with memory-mapping if supported by platform." ON)
option(ZK_ENABLE_FUTURE "ZenKit: Enable breaking changes to be release in a future version" OFF)

add_subdirectory(vendor)

# find all header files; required for them to show up properly in VisualStudio
file(GLOB_RECURSE _ZK_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hh")

list(APPEND _ZK_SOURCES
        src/world/BspTree.cc
        src/world/VobTree.cc
        src/world/WayNet.cc

        src/vobs/Camera.cc
        src/vobs/Light.cc
        src/vobs/Misc.cc
        src/vobs/MovableObject.cc
        src/vobs/Sound.cc
        src/vobs/Trigger.cc
        src/vobs/VirtualObject.cc
        src/vobs/Zone.cc

        src/addon/daedalus.cc
        src/addon/texcvt.cc

        src/archive/ArchiveAscii.cc
        src/archive/ArchiveBinary.cc
        src/archive/ArchiveBinsafe.cc

        src/Archive.cc
        src/Boxes.cc
        src/CutsceneLibrary.cc
        src/DaedalusScript.cc
        src/Date.cc
        src/DaedalusVm.cc
        src/Error.cc
        src/Font.cc
        src/Logger.cc
        src/Material.cc
        src/Mesh.cc
        src/Misc.cc
        src/Model.cc
        src/ModelAnimation.cc
        src/ModelHierarchy.cc
        src/ModelMesh.cc
        src/ModelScript.cc
        src/ModelScriptDsl.cc
        src/MorphMesh.cc
        src/MultiResolutionMesh.cc
        src/Object.cc
        src/SaveGame.cc
        src/SoftSkinMesh.cc
        src/Stream.cc
        src/Texture.cc
        src/Vfs.cc
        src/World.cc
)

list(APPEND _ZK_TESTS
        tests/TestArchive.cc
        tests/TestCutsceneLibrary.cc
        tests/TestDaedalusScript.cc
        tests/TestFont.cc
        tests/TestMaterial.cc
        tests/TestModel.cc
        tests/TestModelAnimation.cc
        tests/TestModelHierarchy.cc
        tests/TestModelMesh.cc
        tests/TestModelScript.cc
        tests/TestMorphMesh.cc
        tests/TestMultiResolutionMesh.cc
        tests/TestSaveGame.cc
        tests/TestStream.cc
        tests/TestTexture.cc
        tests/TestVfs.cc
        tests/TestVobsG1.cc
        tests/TestVobsG2.cc
        tests/TestWorld.cc
)

if (BUILD_SHARED_LIBS AND ZK_BUILD_SHARED)
    add_library(zenkit SHARED)
    set_target_properties(zenkit PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN 1)
    target_compile_definitions(zenkit PRIVATE ZKDYN=1)
else ()
    add_library(zenkit STATIC)
endif ()

if (NOT ZK_ENABLE_DEPRECATION)
    target_compile_definitions(zenkit PUBLIC ZKNO_REM=1)
endif ()

if (ZK_ENABLE_FUTURE)
    target_compile_definitions(zenkit PUBLIC ZK_FUTURE=1)
endif ()

include(support/BuildSupport.cmake)

if (ZK_ENABLE_ASAN)
    message(STATUS "ZenKit: ASAN enabled")
else ()
    message(STATUS "ZenKit: ASAN disabled")
endif()
bs_select_cflags(${ZK_ENABLE_ASAN} _ZK_COMPILE_FLAGS _ZK_LINK_FLAGS)

bs_check_posix_mmap(_ZK_HAS_MMAP_POSIX)
bs_check_win32_mmap(_ZK_HAS_MMAP_WIN32)

if (_ZK_HAS_MMAP_POSIX)
    message(STATUS "ZenKit: POSIX memory mapping available")
    list(APPEND _ZK_SOURCES src/MmapPosix.cc)
    target_compile_definitions(zenkit PUBLIC _ZK_MMAP_AVAILABLE=1)
elseif (_ZK_HAS_MMAP_WIN32)
    message(STATUS "ZenKit: Windows memory mapping available")
    list(APPEND _ZK_SOURCES src/MmapWin32.cc)
    target_compile_definitions(zenkit PUBLIC _ZK_MMAP_AVAILABLE=1)
endif ()

if (ZK_ENABLE_MMAP)
    if (_ZK_HAS_MMAP_POSIX OR _ZK_HAS_MMAP_WIN32)
        message(STATUS "ZenKit: Memory mapping enabled")
        target_compile_definitions(zenkit PUBLIC _ZK_WITH_MMAP=1)
    elseif (ZK_ENABLE_MMAP)
        message(WARNING "ZenKit: Memory mapping enabled but not supported!")
    endif ()
else ()
    message(STATUS "ZenKit: Memory mapping disabled")
endif ()

target_sources(zenkit PRIVATE ${_ZK_SOURCES} ${_ZK_HEADERS})
target_include_directories(zenkit PUBLIC include)
target_compile_definitions(zenkit PRIVATE _ZKEXPORT=1 ZKNO_REM=1)
target_compile_options(zenkit PRIVATE ${_ZK_COMPILE_FLAGS})
target_link_options(zenkit PUBLIC ${_ZK_LINK_FLAGS})
target_link_libraries(zenkit PUBLIC squish)
set_target_properties(zenkit PROPERTIES DEBUG_POSTFIX "d" VERSION ${PROJECT_VERSION})

if (ZK_ENABLE_INSTALL)
    install(TARGETS zenkit ARCHIVE LIBRARY RUNTIME)
    install(DIRECTORY "include/phoenix" TYPE INCLUDE)
    install(DIRECTORY "include/zenkit" TYPE INCLUDE)
endif ()

# when building tests, create a test executable and load it into CTest
if (ZK_BUILD_TESTS AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

    add_executable(test-zenkit ${_ZK_TESTS})
    target_link_libraries(test-zenkit PRIVATE zenkit doctest_with_main)
    target_compile_options(test-zenkit PRIVATE ${_ZK_COMPILE_FLAGS})
    target_link_options(test-zenkit PUBLIC ${_ZK_LINK_FLAGS})

    doctest_discover_tests(test-zenkit EXTRA_ARGS -tse=CutsceneLibrary,DaedalusScript,World WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/tests)
endif ()

# when building examples, include the subdirectory
if (ZK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
