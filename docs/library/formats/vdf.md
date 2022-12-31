# ZenGin Virtual File System

*Virtual Disk Files* (or `VDFs`) are container files like [ZIP](https://en.wikipedia.org/wiki/ZIP_(file_format)) or,
more accurately [TAR](https://en.wikipedia.org/wiki/Tar_(computing)). A VDF contains a directory structure with
multiple files within it. Originally, *Gothic* and *Gothic II* used a library
called [PhysicsFS](https://icculus.org/physfs/)
to load these files and read from them. While *PhysicsFS* now supports VDFs out-of-the-box, _phoenix_ implements its
own, modern parser for them. This page provides a high-level overview of the VDF implementation in _phoenix_.

The original VDF implementation shipped with *Gothic* and *Gothic II* was not written by *Piranha Bytes* themselves but
by a now defunct company
called [TRIACOM Software](https://www.northdata.com/Triacom+Software+GmbH,+Bochum/Amtsgericht+Gelsenkirchen+HRB+6340).

## Overview

*phoenix'* implementation of VDFs lives in `include/phoenix/vdfs.hh` and `source/vdfs.cc`. The most important data
structure here is `phoenix::vdf_file`. It contains the root entries of the VDF and its header. Each entry in a VDF is
represented by the `phoenix::vdf_entry` class and each entry can have multiple child entries.

### Loading a VDF file

Unlike most data structures in *phoenix*, VDF files can be loaded using the `phoenix::vdf_file::open()` function. It
takes either a `std::filesystem::path` or a `phoenix::buffer` and loads the VDF from it.

```cpp title="Example"
#include <phoenix/vdfs.hh>

int main(int, const char** argv) {
    [[maybe_unused]] auto vdf = phoenix::vdf_file::open("A.vdf");
    return 0;
}
```

### Accessing VDF entries

There are two ways of accessing the entries of a VDF. The `phoenix::vdf_file::find_entry()` method can be used to
find an entry with the given name anywhere in the VDF while the `phoenix::vdf_file::resolve_path()` method can be used
to find the entry at the given path. Both of these methods will return a `phoenix::vdf_entry*` which will be `nullptr`
if the entry was not found.

Note that `find_entry` will only ever return the first result it finds. Which result is the first depends on the way
the VDF was created and in which order entries were written. If you know that there are two files with the same name
in a VDF, you can use `resolve_path` to explicitly select the one you want.

```cpp title="Example"
#include <phoenix/vdfs.hh>

int main(int, const char** argv) {
    auto vdf = phoenix::vdf_file::open("A.vdf");

    [[maybe_unused]] auto* found_entry = vdf.find_entry("SomeFile.txt");
    [[maybe_unused]] auto* resolved_entry = vdf.resolve_entry("some/other/path/SomeFile.txt");

    return 0;
}
```

### Merging VDFs

Another feature of *phoenix'* implementation of VDFs is merging. Sometimes it is necessary or convenient to merge
multiple VDFs into one. In Gothic, for example, there are multiple different VDFs containing texture data:

- `textures_apostroph_patch_neu.VDF`,
- `textures_choicebox_32pixel_modialpha.VDF`,
- `textures_patch.VDF`,
- `textures_Startscreen_ohne_Logo.VDF` and
- `textures.VDF`.

Some of them contain textures meant to be patches and some just add new textures. You can merge them together to
easily access all of the files using a single `phoenix::vdf_file` instance using `phoenix::vdf_file::merge()`.

!!! danger
    The `merge()` implementation currently does not work properly when replacing files. To make sure only the newest
    file in a given VDF is kept, you have to order the VDFs by date before merging them.

```cpp title="Example"
#include <phoenix/vdfs.hh>

int main(int, const char** argv) {
    auto vdf_a = phoenix::vdf_file::open("A.vdf");
    auto vdf_b = phoenix::vdf_file::open("Another.vdf");
    vdf_a.merge(vdf_b, false);

    // both VDFs remain valid on their own but vdf_a now also contains non-duplicate entries of vdf_b.

    return 0;
}
```
