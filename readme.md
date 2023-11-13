![ZenKit Logo](assets/logo.png)

# The _ZenKit_ Project

[![Build](https://img.shields.io/github/actions/workflow/status/GothicKit/phoenix/build.yml?label=Build&branch=main)](https://github.com/GothicKit/phoenix/actions/workflows/build.yml)
[![License](https://img.shields.io/github/license/GothicKit/phoenix?label=License&color=important)](https://github.com/GothicKit/phoenix/blob/main/license.md)
[![C++](https://img.shields.io/static/v1?label=C%2B%2B&message=17&color=informational)]()
[![Platforms](https://img.shields.io/static/v1?label=Supports&message=GCC%20|%20Clang%20|%20MSVC%20|%20Apple%20Clang&color=blueviolet)]()
[![Version](https://img.shields.io/github/v/tag/GothicKit/phoenix?label=Version&sort=semver)](https://github.com/GothicKit/phoenix/releases/latest)

**_ZenKit_ has recently been updated to version 1.3! For information about how to migrate your projects, see the
[documentation](https://zk.gothickit.dev/library/misc/v1.2-to-v1.3/).**

> [!IMPORTANT]
> **NEW: C# and Java bindings for ZenKit are now available! See the [NuGet Package Gallery](https://www.nuget.org/packages/ZenKit) and [Maven Central](https://central.sonatype.com/artifact/dev.gothickit/zenkit).**

The _ZenKit_ project aims to re-implement file formats used by the _ZenGin_ made by [Piranha Bytes][] for their
early-2000s games [Gothic][] and [Gothic II][]. It is heavily based on [ZenLib][] which is used as a reference
implementation for the different file formats used.

_ZenKit_ includes parsers and basic datastructures for most file formats used by the _ZenGin_ as well as a
type-safe VM for _Daedalus_ scripts and supporting infrastructure like [Gothic II][] class definitions. Tools for
inspecting and converting _ZenGin_ files can be found in [phoenix studio][].

To get started, take a look in the [Reference Documentation][]. Don't hesitate to open a discussion thread over in
[Discussions][] if you have a question or need help. Please open an issue for any bug you encounter!

You can also contact me directly on Discord, ideally by pinging me (@lmichaelis) in the [GMC Discord](https://discord.gg/mCpS5b5SUY) in the _tools_ channel.

## Supported File Formats

Currently, the following file formats are supported.

| Format                    |           Extension            | Description                                                                        | _ZenKit_ Class Name   |
|---------------------------|:------------------------------:|------------------------------------------------------------------------------------|-----------------------|
| [Model Animation][]       |             `.MAN`             | Contains animations for a model                                                    | `ModelAnimation`      |
| [Model Hierarchy][]       |             `.MDH`             | Contains skeletal information for a model                                          | `ModelHierarchy`      |
| [Model Mesh][]            |             `.MDM`             | Contains the mesh of a model                                                       | `ModelMesh`           |
| [Model][]                 |             `.MDL`             | Contains a mesh and a hierarchy which make up a model                              | `Model`               |
| [Morph Mesh][]            |             `.MMB`             | Contains a morph mesh with its mesh, skeleton and animation data                   | `MorphMesh`           |
| [Multi Resolution Mesh][] |             `.MRM`             | Contains a mesh with [LOD][] information                                           | `MultiResolutionMesh` |
| [Mesh][]                  |             `.MSH`             | Contains mesh vertices and vertex features like materials                          | `Mesh`                |
| [Daedalus Script][]       |             `.DAT`             | Contains a compiled _Daedalus_ script                                              | `DaedalusScript`      |
| [Texture][]               |             `.TEX`             | Contains texture data in a variety of formats                                      | `Texture`             |
| [Font][]                  |             `.FNT`             | Contains font data                                                                 | `Font`                |
| [ZenGin Archive][]        |             `.ZEN`             | Contains various structured data (general object persistence).                     | `ReadArchive`         |
| [Text/Cutscenes][]        | `.BIN`, `.CSL`, `.DAT`, `.LSC` | Contains text and cutscene data                                                    | `CutsceneLibrary`     |
| [Model Script][]          |         `.MDS`, `.MSB`         | Contains model animation script data and associated hierarchy and mesh information | `ModelScript`         |
| [Virtual File System][]   |             `.VDF`             | Contains a directory structure containing multiple files; similar to [TAR][].      | `Vfs`                 |

## Contributing

If you'd like to contribute, please read [Contributing](contributing.md) first.

## Building

_ZenKit_ is currently only tested on Linux and while Windows _should_ be supported you might run into issues. If so,
feel free to create an issue or open a merge request. You will need

* A working compiler which supports C++17, like GCC 9
* CMake 3.10 or above
* Git

To build _ZenKit_ from scratch, just open a terminal in a directory of your choice and run

```bash
git clone --recursive https://github.com/GothicKit/phoenix
cd phoenix
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

You will find the library in `build/`.

## Using

```cpp
#include <zenkit/Vfs.hh>
#include <zenkit/Model.hh>
#include <zenkit/Stream.hh>

int main(int, char**) {
	// Open the VDF file for reading
	zenkit::Vfs vfs {};
	vfs.mount_disk("Models.VDF");
	
	// Find the MyModel.MDL within the VDF
	auto entry = vdf.find("MyModel.MDL");
	
	if (entry == nullptr) {
		// MyModel.MDL was not found in the VDF
		return -1;
	}
	
	// Open MyModel.MDL for reading
	auto buf = entry->open_read();
	
	// One could also memory-map a normal file from disk:
	//   auto buf = zenkit::Read::from("/path/to/file");

	// Or if you have a vector of data:
	//   std::vector<uint8_t> data { /* ... */ };
	//   auto buf = zenkit::Read::from(std::move(data));
	
	// Parse the model
	zeknit::Model mdl {};
	mdl.load(buf.get());
	
	// Do something with mdl ...
	
	return 0;
}
```

_ZenKit_ also provides a VM implementation for the _Daedalus_ scripting language used by _ZenGin_:

```c++
#include <zenkit/Stream.hh>
#include <zenkit/DaedalusScript.hh>
#include <zenkit/DaedalusVm.hh>

#include <iostream>
#include <string>

enum class MyScriptEnum : int {
    FANCY = 0,
    PLAIN = 1,
};

// Declare a class to be bound to members in a script. This is used in `main`.
struct MyScriptClass : public zenkit::DaedalusInstance {
    // Declare the members present in the script class.
    // Supported types are:
    //   * int
    //   * float
    //   * std::string
    //   * `enum` types with sizeof(enum) == 4
    // and their C-Style array versions.
    
    std::string myStringVar;
    int someIntegers[10];
    float aFloat;
    MyScriptEnum anEnum;
};

// Define a function to be bound to an external definition in a script. This is used in `main`.
// Supported parameter types are:
//   * int
//   * float
//   * bool
//   * std::string_view
//   * std::shared_ptr<instance> or any subclass of instance
// Supported return types are:
//   * int (or anything convertible to int32_t)
//   * float (or anything convertible to float)
//   * bool
//   * void
//   * std::shared_ptr<instance> or any subclass of instance
bool MyExternalFunction(std::string_view param1, int param2, std::shared_ptr<MyScriptClass> param3) {
    std::cout << "Calling MyExternalFunction(" << param1 << ", " << param2 << ", " << param3->symbol_index() << ")\n";
    return true;
}

// Define a function to be bound to an internal definition in a script. This is used in `main`.
// Supported parameter and return types are the same as for external functions. 
std::string MyInternalFunction(int param1) {
    return std::to_string(param1);
}

int main(int, char**) {
    // Open a buffer containing the script.
    auto buf = zenkit::Read::from("MyScript.DAT");
    
    // Create the VM instance
    zenkit::DaedalusScript script {};
    script.load(buf.get());
    
    zenkit::DaedalusVm vm {std::move(script)};
    
    // You can register Daedalus -> C++ shared classes. The `register_member` function will automatically
    // validate that the definitions match at runtime.
    vm.register_member("MyScriptClass.myStringVar", &MyScriptClass::myStringVar);
    vm.register_member("MyScriptClass.someIntegers", &MyScriptClass::someIntegers);
    vm.register_member("MyScriptClass.aFloat", &MyScriptClass::aFloat);
    vm.register_member("MyScriptClass.anEnum", &MyScriptClass::anEnum);
    
    // You could also have differing member and/or class names:
    //   vm.register_member("SomeOtherClass.fancyness", &MyScriptClass::anEnum);
    
    // phoenix supports registering external script functions to a C++ function. The function signature is
    // validated at runtime to match the definition of the function in the script file.
    vm.register_external("MyExternalFunction", &MyExternalFunction);
    
    // You can also register a function to be called if an external is not registered:
    vm.register_default_external([](std::string_view name) {
        std::cout << "External " << name << " not registered\n";
    });
    
    // phoenix allows you to override internal script functions as well. The signature of the function
    // is also validated at runtime.
    vm.override_function("MyInternalFunction", &MyInternalFunction);
    
    // You can call the instance initializer like this:
    auto myNpc = vm.init_instance<MyScriptClass>("MyInstance");
    
    // Alternatively, you can also provide a pointer to the instance instead of having it be allocated
    // automatically:
    //
    //   auto ptr = std::make_shared<MyScriptClass>();
    //   vm.init_instance(ptr, "MyInstance");
    
    // Calling internal script function is also easy:
    std::cout << "Result of MyInternalFunction(10): " 
              << vm.call_function<std::string>("MyInternalFunction", 10) << "\n";
    
    // If you'd like to avoid passing a string to this function, you can also fetch the
    // function symbol beforehand and pass it instead:
    //
    //   auto* functionSym = vm.find_symbol_by_name("MyInternalFunction");
    //   auto result = vm.call_function<std::string>(functionSym, 10);

    // Sometimes it is required to set the HERO, SELF, OTHER, ITEM, or VICTIM  global instance variables.
    // This can be done like this:
    auto oldValue = vm.global_other()->get_instance();
    vm.global_other()->set_instance(myNpc);

    // The other global variables can be accessed using:
    //   * vm.global_self()
    //   * vm.global_other()
    //   * vm.global_victim()
    //   * vm.global_hero()
    //   * vm.global_item()
    
    // No special clean-up logic is required. All initialized instances will be
    // valid even after the script is destructed because they are shared_ptrs.
    
    return 0;
}
```

For more examples on how to use _ZenKit_, take a look into the [examples][] directory and [phoenix studio][] repository.

## Licensing

While the source code of _ZenKit_ is licensed under the [MIT license](license.md), the
[_ZenKit_ logo](assets/logo.svg) is licensed under [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/).

---

[Piranha Bytes]: https://www.piranha-bytes.com/
[Gothic]: https://en.wikipedia.org/wiki/Gothic_(video_game)
[Gothic II]: https://en.wikipedia.org/wiki/Gothic_II
[ZenLib]: https://github.com/ataulien/ZenLib
[phoenix studio]: https://github.com/GothicKit/phoenix-studio
[examples]: https://github.com/lmichaelis/phoenix/tree/main/examples

[TAR]: https://en.wikipedia.org/wiki/Tar_(computing)
[LOD]: https://en.wikipedia.org/wiki/Level_of_detail_(computer_graphics)

[Reference Documentation]: https://phoenix.gothickit.dev/library/overview
[Discussions]: https://github.com/GothicKit/phoenix/discussions

[Model Animation]: https://phoenix.gothickit.dev/library/api/model-animation/
[Model Hierarchy]: https://phoenix.gothickit.dev/library/api/model-hierarchy/
[Model Mesh]: https://phoenix.gothickit.dev/library/api/model-mesh/
[Model]: https://phoenix.gothickit.dev/library/api/model/
[Morph Mesh]: https://phoenix.gothickit.dev/library/api/morph-mesh/
[Multi Resolution Mesh]: https://phoenix.gothickit.dev/library/api/multi-resolution-mesh/
[Mesh]: https://phoenix.gothickit.dev/library/api/mesh/
[Daedalus Script]: https://phoenix.gothickit.dev/library/api/daedalus-script/
[Texture]: https://phoenix.gothickit.dev/library/api/texture/
[Font]: https://phoenix.gothickit.dev/library/api/font/
[ZenGin Archive]: https://phoenix.gothickit.dev/library/api/archive/
[Text/Cutscenes]: https://phoenix.gothickit.dev/library/api/cutscene-library/
[Model Script]: https://phoenix.gothickit.dev/library/api/model-script/
[Virtual File System]: https://phoenix.gothickit.dev/library/api/virtual-file-system/
