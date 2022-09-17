# the _phoenix_ project
[![Build](https://github.com/lmichaelis/phoenix/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/lmichaelis/phoenix/actions/workflows/build.yml)

**Here be dragons! _phoenix_ is currently changing very rapidly and your application _will_ break! Be prepared to change
a lot of code when updating to new versions!**

The _phoenix_ project aims to re-implement file formats used by the _ZenGin_ made by [Piranha Bytes](https://www.piranha-bytes.com/)
for their early-2000s games [Gothic](https://en.wikipedia.org/wiki/Gothic_(video_game)) and [Gothic II](https://en.wikipedia.org/wiki/Gothic_II).
It is heavily based on [ZenLib](https://github.com/Try/ZenLib) which is used as a reference implementation for the
different file formats used.

_phoenix_ includes **parsers and basic datastructures** for most file formats used by the _ZenGin_ as well as **tools 
designed to inspect and convert** these files. Additionally, it includes a type-safe **VM for _Daedalus_ scripts** and
supporting infrastructure like _Gothic II_ class definitions.

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

You will find the tool binaries in `build/bin` and the library in `build/lib`.

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

For more examples on how to use _phoenix_, take a look into the `examples` or `tools` directories.
