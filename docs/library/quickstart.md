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

=== "C#"
    
    You can install `ZenKitCS` from the [NuGet Package Gallery](https://www.nuget.org/packages/ZenKit). Simply add the
    following snippet to your `.csproj` file, replacing the version with the approprite version identifier from NuGet.

    ```
    <ItemGroup>
      <PackageReference Include="ZenKit" Version="x.x.x" />
    </ItemGroup>
    ```

    To build your project then, you will need to add a `RuntimeIdentifiers` property to your `.csproj`. You can simply
    use this once and copy it the topmost `<PropertyGroup>` in your `.csproj` like this:

    ```xml
    <PropertyGroup>
        <!-- ... -->
        
        <RuntimeIdentifiers>linux-x64;win-x64;osx-x64;android-arm64</RuntimeIdentifiers>

        <!-- ... -->
    </PropertyGroup>
    ```

    You can now also build your project for those runtimes by supplying the runtime identifier in `dotnet build` using the
    `-r` parameter. This is how you would build your project for Android:

    ```
    dotnet build -r android-arm64 -c Release --self-contained
    ```

=== "Java"

    You can integrate `ZenKit4J` into your Java project from [Maven Central](https://mvnrepository.com/artifact/dev.gothickit/zenkit).
    Refer to its documentation about how to set it up with your package manager. No additional configuration is required.

=== "Python"

    You can add `ZenKit4Py` to you Python project through Pip via [PyPI](https://pypi.org/project/zenkit/):

    ```
    pip install zenkit
    ```

    No further action is required.

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

=== "Python"

    ```py
    import sys
    from zenkit import Vfs, Font

    def main() -> int:
        if len(sys.argv) < 2:
            print("please provide a textures.vdf file to read from", file=sys.stderr)
            return -1
        
        vfs = Vfs()
        vfs.mount_disk(sys.argv[1])

        font_file = vfs.find("font_default.fnt")
        if font_file is None:
            print("FONT_DEFAULT.FNT was not found in the VFS", file=sys.stderr)
            return -2
        
        font = Font.load(font_file)
        print("The associated texture for FONT_DEFAULT.FNT is \"" + font.name + "\"")
        return 0

    if __name__ == "__main__":
        exit(main())
    ```

[CMake]: https://cmake.org/
[FetchContent]: https://cmake.org/cmake/help/latest/module/FetchContent.html

[1]: https://github.com/GothicKit/ZenKit/discussions
[2]: https://github.com/ZenKit/phoenix/tree/main/examples
