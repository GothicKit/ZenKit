# Binary Encoding

!!! abstract inline end "Quick Infos"
    **Type:** Encoding<br/>
    **Class Name:** `zFILE`<br/>
    **Used By:** [`zCFileBIN`](binary-chunked.md), [`zCFont`](../formats/font.md), [`zCTexture`](../formats/texture.md), [`zFILE_VDFS`](../formats/vdf.md)

Most asset files and other encodings are based flat binary files. As their name indicates, files encoded using flat
binary just contain raw bytes. Their structure is fully defined by the asset format using the encoding.

Binary files and other encodings based on them are *little-endian*.

## Representation

Wherever binary data needs to be represented in a structured form, C-style structs are used. In this documentation,
they are intended to be read without any sort of padding (see below). All primitive datatypes and how they should be
read are documented in the [Datatype Reference](../datatypes.md).

```c title="Example Binary Data"
struct Example {
    ushort checksum;
    byte type;
};

static_assert(sizeof(Example) == 3, "A ushort is 2 bytes plus a byte is 3 bytes");
```

Often, binary structures contain arrays of dynamic size. In order to allow for more fluent definitions, the C-style
array declaration syntax is used. The variable containing the dynamic size of the array is put in a comment within
the brackets.

```c title="Example Binary Data Containing Arrays"
struct Example {
    uint staticArray[10]; //< A set of 10 consecutive uints
    
    ushort dynamicLength;
    byte dynamicArray[/* dynamicLength */]; //< A set of `dynamicLength` consecutive bytes
};
```
