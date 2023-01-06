# Chunked Binary Encoding

!!! abstract inline end "Quick Infos"
    **Type:** Encoding<br/>
    **Class Name:** `zCFileBIN`<br/>
    **Used By:** [`zCModelAni`](../formats/animation.md)

Many asset files are encoded in [binary](binary.md), splitting the data up into sections called chunks. Each chunk
consists of a type identifier, its size and the actual data wrapped inside it. Files using the chunked binary encoding
do not contain data outside of chunks.

```c title="File Structure"
struct zCFileBIN {
    zCFileBIN_Chunk chunks[];
}

struct zCFileBIN_Chunk {
    ushort type;
    uint size;
    byte data[/* size */];
};
```

It should be noted that, apart from the data being grouped into chunks, files encoded in chunked binary behave the same
as flat [binary](binary.md) files.

## Representation

Wherever the chunked binary format is used in asset format references, the chunks are represented in a tabbed list. To
keep it concise, the names of the tabs contain the chunk type and their content contains a C-style struct defining the
data stored within.

=== "`0x1000`"

    ```c title="Section 0x1000"
    struct MyClass_Section1 {
        uint myValue1;

        // ...
    };
    ```

    This tab describes the section with type `0x1000`. As declared in the struct above, it only contains a 32-bit
    unsigned integer. The definition of all datatypes can be found in the [Datatype Reference](../datatypes.md).

=== "`0x1010`"

    ```c title="Section 0x1010"
    struct MyClass_Section2 {
        string myValue2;
        
        // ...
    };
    ```

    This tab describes the section with type `0x1010`. As you can see, it works the same as the first tab.
   
You may have noticed, that the chunk's size is not mentioned anywhere in these tabs. While it should be taken as the
actual size of the chunk, this reference aims to be as complete as possible, so the size of the struct provided for
each section should be the same as the chunk size. If it is not, there is an error in this documentation.
