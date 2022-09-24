![phoenix logo](assets/logo.png)

# the _phoenix_ project

[![Build](https://img.shields.io/github/workflow/status/lmichaelis/phoenix/Build?label=Build)](https://github.com/lmichaelis/phoenix/actions/workflows/build.yml)
![License](https://img.shields.io/github/license/lmichaelis/phoenix?label=License&color=important)
![C++](https://img.shields.io/static/v1?label=C%2B%2B&message=17&color=informational)
![Platforms](https://img.shields.io/static/v1?label=Supports&message=GCC%20|%20Clang%20|%20MSVC%20|%20Apple%20Clang&color=blueviolet)

**Here be dragons! _phoenix_ is currently changing very rapidly and your application _will_ break! Be prepared to change
a lot of code when updating to new versions!**

The _phoenix_ project aims to re-implement file formats used by the _ZenGin_ made
by [Piranha Bytes](https://www.piranha-bytes.com/)
for their early-2000s games [Gothic](https://en.wikipedia.org/wiki/Gothic_(video_game))
and [Gothic II](https://en.wikipedia.org/wiki/Gothic_II).
It is heavily based on [ZenLib](https://github.com/Try/ZenLib) which is used as a reference implementation for the
different file formats used.

_phoenix_ includes **parsers and basic datastructures** for most file formats used by the _ZenGin_ as well as a
type-safe **VM for _Daedalus_ scripts** and supporting infrastructure like _Gothic II_ class definitions. Tools for
inspecting and converting _ZenGin_ files can be found in [phoenix studio](https://github.com/lmichaelis/phoenix-studio).

## contributing
If you'd like to contribute, please read [contributing](contributing.md) first.

## supported file formats
Currently, the following file formats are supported.

| Format                   |           Extension            | Description                                                                                                                | _phoenix_ Class Name |
|--------------------------|:------------------------------:|----------------------------------------------------------------------------------------------------------------------------|----------------------|
| Model Animation          |             `.MAN`             | Contains animations for a model                                                                                            | `animation`          |
| Model Hierarchy          |             `.MDH`             | Contains skeletal information for a model                                                                                  | `model_hierarchy`    |
| Model Mesh               |             `.MDM`             | Contains the mesh of a model                                                                                               | `model_mesh`         |
| Model                    |             `.MDL`             | Contains a mesh and a hierarchy which make up a model                                                                      | `model`              |
| Morph Mesh Binary        |             `.MMB`             | Contains a morph mesh with its mesh, skeleton and animation data                                                           | `morph_mesh`         |
| Multi Resolution Mesh    |             `.MRM`             | Contains a mesh with [CLOD](https://en.wikipedia.org/wiki/Level_of_detail_(computer_graphics)) information                 | `proto_mesh`         |
| Mesh                     |             `.MSH`             | Contains mesh vertices and vertex features like materials                                                                  | `mesh`               |
| Daedalus Script Binaries |             `.DAT`             | Contains a compiled _Daedalus_ script                                                                                      | `script`             |
| Texture                  |             `.TEX`             | Contains texture data in a variety of formats                                                                              | `texture`            |
| Font                     |             `.FNT`             | Contains font data                                                                                                         | `font`               |
| ZenGin Archive           |             `.ZEN`             | Contains various structured data. Used mostly for world hierarchy data and object persistence.                             | `archive`            |
| Text/Cutscenes           | `.BIN`, `.CSL`, `.DAT`, `.LSC` | Contains text and cutscene data                                                                                            | `messages`           |
| Model Script             |             `.MDS`             | Contains model animation script data and associated hierarchy and mesh information                                         | `model_script`       |
| Model Script Binary      |             `.MSB`             | Contains model animation script data and associated hierarchy and mesh information (binary form)                           | `model_script`       |
| Virtual Disk             |             `.VDF`             | Contains a directory structure containing multiple files; similar to [tar](https://en.wikipedia.org/wiki/Tar_(computing)). | `vdf_file`           |

## currently available tools
_phoenix_ comes with some tools to inspect and convert the _ZenGin_'s file formats. The following is a list of all
tools currently available:

| Name        | Description                                                                                                                                              |
|-------------|----------------------------------------------------------------------------------------------------------------------------------------------------------|
| `pxinfo`    | Dump information about various files.                                                                                                                    |
| `pxmdl`     | Convert `MRM`, `MSH`, `MMB`, `MDL` and `MDM` files as well as world meshes into the [Wavefront](https://en.wikipedia.org/wiki/Wavefront_.obj_file) model |
| `pxscrdump` | Display, disassemble and decompile  compiled _Daedalus_ scripts (similar to `objdump`).                                                                  |
| `pxtex`     | Convert `TEX` files to `TGA`                                                                                                                             |
| `pxvdfs`    | Extract and list contents of `VDF` files.                                                                                                                |

## building
_phoenix_ is currently only tested on Linux and while Windows _should_ be supported you might run into issues. If so,
feel free to create an issue or open a merge request. You will need

* A working compiler which supports C++17, like GCC 9
* CMake 3.10 or above
* Git

To build _phoenix_ from scratch, just open a terminal in a directory of your choice and run

```bash
git clone --recursive https://github.com/lmichaelis/phoenix
cd phoenix
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

You will find the library in `build/lib`.

## using
Using _phoenix_ in your project is pretty straightforward. Just add `include` to your include directories and link
against the _phoenix_ library. To start loading files, you just include the header and call `cls::parse(...)` or 
`cls::open(...)`, depending on the file type on one of the classes from the table above. For example, to load a
model from a VDF file, you do this:

```cpp
#include <phoenix/vdfs.hh>
#include <phoenix/model.hh>

int main(int, char**) {
	// Open the VDF file for reading
	auto vdf = phoenix::vdf_file::open("Models.VDF");
	
	// Find the MyModel.MDL within the VDF
	auto entry = vdf.find_entry("MyModel.MDL");
	
	if (entry == nullptr) {
		// MyModel.MDL was not found in the VDF
		return -1;
	}
	
	// Open MyModel.MDL for reading
	auto buf = entry->open();
	
	// One could also memory-map a normal file from disk:
	//   auto buf = phoenix::buffer::mmap("/path/to/file");

	// Or if you have a vector of data:
	//   std::vector<uint8_t> data { /* ... */ };
	//   auto buf = phoenix::buffer::of(std::move(data));
	
	// Parse the model
	auto mdl = phoenix::model::parse(buf);
	
	// Do something with mdl ...
	
	return 0;
}
```

_phoenix_ also provides a VM implementation for the _Daedalus_ scripting language used by _ZenGin_:

```c++
#include <phoenix/buffer.hh>
#include <phoenix/script.hh>
#include <phoenix/vm.hh>

#include <iostream>
#include <string>

enum class MyScriptEnum : int {
    FANCY = 0,
    PLAIN = 1,
};

// Declare a class to be bound to members in a script. This is used in `main`.
struct MyScriptClass : public phoenix::instance {
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
    auto buf = phoenix::buffer::mmap("MyScript.DAT");
    
    // Create the VM instance
    phoenix::vm vm {phoenix::script::parse(buf)};
    
    // Alternatively, if you just need to inspect the script itself, you can just:
    //   auto script = phoenix::script::parse(buf);
    
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

For more examples on how to use _phoenix_, take a look into the
[`examples`](https://github.com/lmichaelis/phoenix/tree/main/examples) and
[`tools`](https://github.com/lmichaelis/phoenix/tree/main/tools) directories. A working example of using the VM can be
found in [`examples/run_interpreter.cc`](https://github.com/lmichaelis/phoenix/blob/main/examples/run_interpreter.cc).

## licensing

While the source code of _phoenix_ is licensed under the [MIT license](license.md), the
[_phoenix_ logo](assets/logo.svg) is licensed under [CC BY-NC 4.0](https://creativecommons.org/licenses/by-nc/4.0/).
