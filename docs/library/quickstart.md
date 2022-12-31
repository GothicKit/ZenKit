# Getting Started

To get started using *phoenix*, you will first need to add it to your project as a library. This page will show you two
ways of doing that but there are a lot more ways you can use. This page will also give you a brief overview of
*phoenix* and provide a simple example. You can find a list of file types and information about loading them with
*phoenix* in the [File Type Reference](reference.md).

**Feel free to ask any questions you might have over in
the [:octicons-mark-github-16: Discussions](https://github.com/lmichaelis/phoenix/discussions)
section of the GitHub repository!**

## Adding *phoenix* to your project

*phoenix* uses the [CMake](https://cmake.org/) build system which also is the easiest way of adding it to your project.
There are two main ways of doing that

1. Add *phoenix* as a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules) or
2. Let CMake load it at configuration time.

To add *phoenix* as a Git submodule, simply create a directory in your project to house submodules
(common names are `vendor`, `3rdparty` or `lib`) and run the following commands:

```sh
# either this, to use the development version of phoenix
git submodule add https://github.com/lmichaelis/phoenix.git path-to-submodule-directory/phoenix

# ... or this to use a phoenix release (recommended)
git submodule add -b v1.0 https://github.com/lmichaelis/phoenix.git path-to-submodule-directory/phoenix
```

After doing this, open your `CMakeLists.txt` file and add a
line `#!cmake add_subdirectory(path-to-submodule-directory/phoenix)`
to it. You can now add `phoenix` to your `#!cmake target_link_libraries(...)`. Make sure to also commit the new
directory to your repository, otherwise it will only exist in your local file system.

To add *phoenix* to your project without using Git submodules, add the following lines to your `CMakeLists.txt`.

```cmake
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        phoenix
        GIT_REPOSITORY https://github.com/lmichaelis/phoenix
        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
        LOG_DOWNLOAD ON
)
```

You can also do this without using Git by using
CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
interface. Just point it to the URL of the source download of
the [:octicons-mark-github-16: Release](https://github.com/lmichaelis/phoenix/releases)
of *phoenix* you'd like to use. After doing either of these, you can add *phoenix* as a library by adding it to
your `#!cmake target_link_libraries(...)`.

## A basic example

This is a basic example of how to use *phoenix*. It opens a [VDF File](formats/vdf.md) (`.VDF`) and reads a single
entry from it. It then parses that entry as a [Font](formats/font.md) and prints the name of the associated texture
file to the terminal.

```cpp
#include <phoenix/vdfs.hh>  // required to load VDF files
#include <phoenix/font.hh>  // required to parse fonts

#include <iostream>

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "please provide a textures.vdf file to read from\n";
        return 1;
    }

    // Open the VDF file
    phoenix::vdf_file vdf = phoenix::vdf_file::open(argv[1]);

    // Find the font entry in the VDF
    const phoenix::vdf_entry* font_file = vdf.find_entry("font_default.fnt");

    // Make sure the entry actually exists
    if (font_file == nullptr) {
        std::cerr << "FONT_DEFAULT.FNT was not found in the VDF\n";
        return 2;
    }

    // Open the file and parse a font from it
    phoenix::buffer font_data = font_file->open();
    phoenix::font font = phoenix::font::parse(font_data);

    // Print out the associated texture name
    std::cout << "The associated texture for FONT_DEFAULT.FNT is \"" << font.name << "\"\n";

    return 0;
}
```

To run this, compile it while linking against `phoenix` and execute the resulting binary in a terminal giving it the
path to the `Data/textures.vdf` file in any *Gothic* or *Gothic II* installation like this:

```sh
./a.out /path/to/gothic/Data/textures.VDF
```

The most important data structure in *phoenix* is the `buffer`. It encapsulates raw bytes and is used to read other
primitive types from files. Basically every public parsing API in *phoenix* accepts a `buffer` as input. As you can
see, in this case we get a buffer by `open()`-ing a VDF entry, but we can also obtain one by calling `buffer::mmap()`,
`buffer::read()` or `buffer::of()`. The functionality of each is described in the respective C++ documentation strings.

More examples can be found in
the [:octicons-mark-github-16: examples/](https://github.com/lmichaelis/phoenix/tree/main/examples)
directory of the project.
