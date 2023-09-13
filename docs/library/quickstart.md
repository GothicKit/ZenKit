# Getting Started

## Adding *ZenKit* to your project

To get started using *ZenKit*, you will first need to add it to your project as a library. The process of doing so is
different between depending on which programming language you choose. Select the language of your choice below and
continue.

=== "C/C++"
    
    *ZenKit* uses [CMake][] as its build system and thus it may be integrated into existing CMake projects using CMake's
    [FetchContent][] API or by adding it as a submodule. A system-wide installation via `cmake --install` is currently
    **not** supported.

    Adding *ZenKit* through [FetchContent][] is the easiest way of obtaining a working copy of the library. To do this,
    simply add the following lines to your `CMakeLists.txt` file:

    ```cmake
    include(ExternalProject)
    find_package(Git REQUIRED)

    ExternalProject_Add(
            ZenKit
            GIT_REPOSITORY https://github.com/GothicKit/ZenKit.git
            UPDATE_COMMAND ${GIT_EXECUTABLE} pull
            LOG_DOWNLOAD ON
    )
    ```

    After doing this, you'll need to link against `zenkit` using the `#!cmake target_link_libraries(your-target PRIVATE zenkit)`
    directive. With that, everything should be set up correctly for you to use *ZenKit* in your application.

!!! info "Getting Help"
    **Feel free to ask any questions you might have over in the [:material-github:/discussions][1] section of the
    GitHub repository!**

## A basic example

These are some basic examples of how to use *ZenKit* for each supported language. In this example, we load a `.VDF`-file
into a [`Vfs`](api/virtual-file-system.md), find a [Font](api/font.md) definition within it and load it into memory.
The general mode of operation here is applicable to most assets that can be loaded using *ZenKit*


=== "C++"

    ```cpp
    #include <zenkit/Vfs.hh>  // required to load VDF files
    #include <zenkit/Font.hh>  // required to load fonts

    #include <iostream>

    int main(int argc, const char** argv) {
        if (argc < 2) {
            std::cerr << "please provide a textures.vdf file to read from\n";
            return 1;
        }

        // Create a new Vfs
        zenkit::Vfs vfs {};

        // Load the VDF-file into the Vfs
        vfs.mount_disk(argv[1]);

        // Find the font file in the Vfs
        zenkit::VfsNode const* font_file = vfs.find("font_default.fnt");

        // Make sure the entry actually exists
        if (font_file == nullptr) {
            std::cerr << "FONT_DEFAULT.FNT was not found in the VFS\n";
            return 2;
        }

        // Open the file and parse a font from it
        zenkit::Font font {};
        font.load(font_file->open_read().get());

        // Print out the associated texture name
        std::cout << "The associated texture for FONT_DEFAULT.FNT is \"" << font.name << "\"\n";

        return 0;
    }
    ```

    To run this, compile it while linking against `zenkit` and execute the resulting binary in a terminal giving it the
    path to the `Data/textures.vdf` file in any *Gothic* or *Gothic II* installation:

    ```sh
    ./a.out /path/to/gothic/Data/textures.VDF
    ```

    More examples can be found in the [:material-github:/examples/][2] directory of the project.


[CMake]: https://cmake.org/
[FetchContent]: https://cmake.org/cmake/help/latest/module/FetchContent.html

[1]: https://github.com/GothicKit/ZenKit/discussions
[2]: https://github.com/ZenKit/phoenix/tree/main/examples
