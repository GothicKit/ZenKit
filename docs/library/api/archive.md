# ZenGin Archives

The *ZenGin Archive* format is similar in concept to [JSON][] or [XML][] in that it is used to serialize nested data
into a file which can be read later. There are three different kinds of archives called `ASCII`, `BINARY` and `BIN_SAFE`
but the way these work exactly does not matter when using *ZenKit*.

For detailed documentation on the inner workings of archives, see
the [ZenGin Reference](../../engine/formats/archive.md).

## Overview

Like JSON, archives store values using a key-value structure where every key-value pair belongs to an object. Objects
can also be nested. Unlike JSON, archives do not have first-class support for arrays, instead choosing a special type
and encoding scheme. The class which is responsible for loading *ZenGin Archives* is called `zenkit::ReadArchive`.

### Loading an archive

Since archives are an intermediate format, they are not loaded into memory. Rather, `zenkit::ReadArchive` functions as
a high-level reader-like interface with which data can be read from the archive in a stream. To create one, simply
call `zenkit::ReadArchive::from` (which is analogous to `zenkit::Read::from`) and start reading from it:

=== "C"

    !!! warning
        Archives are currently not supported with the C API.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Archive.hh>  // for `ReadArchive`
    #include <zenkit/Stream.hh>  // for `Read`

    int main(int, const char** argv) {
        auto r = zenkit::Read::from("A.ZEN");
        auto ar = zenkit::ReadArchive::from(r.get());
        
        // ...
        
        return 0;
    }
    ```

    !!! info
        As with all resources, archives can also be loaded from a [virtual file system](virtual-file-system.md), passing the
        input obtained from `zenkit::VfsNode::open_read` into `zenkit::ReadArchive::from`.

        **It must be noted in this case, however, that the unique pointer returned by `VfsNode::open_read` must be saved
        for the lifetime of `ar` since it is the data source for all read operations on it.**

=== "C#"

    !!! warning
        Archives are currently not supported with the C# API since it depends on the C wrapper.

=== "Java"

    !!! warning
        Archives are currently not supported with the Java API since it depends on the C wrapper.

=== "Python"

    !!! warning
        Archives are currently not supported with the Python API since it depends on the C wrapper.


### Reading from an archive

*ZenKit*'s implementation of *ZenGin Archives* is very bare-bones and does not provide a very rich interface. *ZenKit*
will automatically determine the kind of archive but that's about it. Reading more data from it requires knowing the
format you will be reading beforehand, no buffering or preloading of data is done. A more detailed explanation and
demonstration can be found in the example below.

=== "C"

    !!! warning
        Archives are currently not supported with the C API.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Archive.hh>  // for `ReadArchive`
    #include <zenkit/Stream.hh>  // for `Read`

    #include <iostream>

    int main(int, const char** argv) {
        // First, open the archive
        auto buf = zenkit::Read::from("WORLD.ZEN");
        auto zen = zenkit::ReadArchive::from(buf.get());

        // Second, read the definition of the root object.
        //   Every archive has one object at its root which can be used to
        //   identify what kind of data it is storing.
        zenkit::ArchiveObject object {};
        
        if (!zen->read_object_begin(object)) {
            // If no object begins at the current position `read_object_begin`
            // will return `false`.
            std::cerr << "The root object was not found.\n";
            return -1;
        }

        // Here, for example, we're checking whether the root object is a
        // serialized game world.
        if (object.class_name != "oCWorld:zCWorld") {
            std::cerr << "This archive does not contain a serialized game world.\n";
            return -1;
        }

        // From now on, we need to know the format worlds come in. It is not documented here
        // but if necessary, the format of most objects can be found in ZenKit's source code.
        //
        // In this case, we know that worlds contain three sub-objects, one describing the
        // world mesh, one containing the way-net and one containing all dynamic objects in
        // the world.

        while (!zen->read_object_end()) {
            // `read_object_end` is used to read the end of an object. If it returns `false`,
            // no object ends at the current position. It is required to `read_object_end`
            // after reading all entries in that object.
            
            zen->read_object_begin(object);

            if (object.object_name == "VobTree") {
                // Here, we're only interested in finding the sub-object containing the dynamic
                // objects placed into the world.
     
                // The VOb tree consists of more nested objects, each followed by an integer
                // denoting the number entries following it which should be considered children
                // of the VOb. The first integer denotes the number of root objects in the tree.
                
                auto root_vobs = zen->read_int();

                while (!zen->read_object_end()) {
                    zen->read_object_begin(object);
                    
                    if (object.class_name == "zCVob") {
                        // There is a whole system to the class names in the VOb tree but here,
                        // to give an example, we're just interested in the basic `zCVob`. Also
                        // there is a lot more data saved in the `zCVob` object but I will omit
                        // most of it for brevity sake.
                        //
                        // So here's some of the types supported by archives read from one:

                        auto packed = zen->read_int() != 0;
                        auto preset_name = zen->read_string();
                        auto bbox = zen->read_bbox();
                        auto rotation = zen->read_mat3x3();
                        auto position = zen->read_vec3();

                        auto vob_name = zen->read_string();
                        auto visual_name = zen->read_string();
                        auto show_visual = zen->read_bool();
                        auto camera_alignment = zen->read_enum();

                       // As you can see, all fields must be read in order and with the correct type.
                       // If the archive contains a field of type `int` but you try to `read_string()`,
                       // ZenKit will throw an exception (as documented in the docstrings).
                       //
                       // For the full list of supported types, see the table below this example.
                    }

                    if (!zen->read_object_end()) {
                         // (see below for an explanation)
                         zen->skip_object(true);
                    }
                }
            }

            // Now, we read the end of the current object.
            if (!zen->read_object_end()) {
                // But since this small example cannot parse the `WayNet` and `MeshAndBsp`
                // objects of the world, we need to still make sure that the parser is in
                // a sane state. `skip_object` can be used to fully ignore the next object
                // in the archive or, if passed `true`, the object the parser is currently in.
                //
                // We pass `true` here, since we've already `read_object_begin` so the
                // parser is now considered to be in the object we want to skip. Here, skipping
                // the current object also has another great bonus: if, for some reason, one
                // of the objects was not fully parsed, the remaining entries will be skipped
                // so the parser is in a good state for the next iteration of the loop.
                zen->skip_object(true);
            }
        }

        return 0;
    }
    ```

    **Supported data types:**

    | Type Name  | Read Function                           | Description                                                           |
    |------------|-----------------------------------------|-----------------------------------------------------------------------|
    | `string`   | `read_string`                           | A [Windows-1252][] encoded `std::string`                              |
    | `int`      | `read_int`                              | A 32-bit signed integer                                               |
    | `float`    | `read_float`                            | An [IEEE 754][] floating point number                                 |
    | `byte`     | `read_byte`                             | An 8-bit unsigned integer                                             |
    | `word`     | `read_word`                             | A 16-bit unsigned integer                                             |
    | `enum`     | `read_enum`                             | A 32-bit unsigned integer                                             |
    | `bool`     | `read_bool`                             | A boolean value                                                       |
    | `color`    | `read_color`                            | An `RGBA` color quad                                                  |
    | `vec3`     | `read_vec3`                             | A 3-dimensional vector with floating point values                     |
    | `rawFloat` | `read_bbox`, `read_mat3x3`, `read_vec2` | A mathematical structure consisting of multiple floating point values |
    | `raw`      | `read_raw`                              | A set of raw bytes, returned as a `zenkit::Read*`                     |

=== "C#"

    !!! warning
        Archives are currently not supported with the C# API since it depends on the C wrapper.

=== "Java"

    !!! warning
        Archives are currently not supported with the Java API since it depends on the C wrapper.

=== "Python"

    !!! warning
        Archives are currently not supported with the Python API since it depends on the C wrapper.

[JSON]: https://en.wikipedia.org/wiki/JSON
[XML]: https://en.wikipedia.org/wiki/XML
[Windows-1252]: https://en.wikipedia.org/wiki/Windows-1252
[IEEE 754]: https://en.wikipedia.org/wiki/IEEE_754
