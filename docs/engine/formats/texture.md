# ZenGin Textures

!!! abstract inline end "Quick Infos"
    **Type:** Asset Format<br/>
    **Format Name:** Texture<br/>
    **File Extension:** `.TEX`<br/>
    **Class Name:** `zCTexture`<br/>
    **Encoding:** [Binary](../encodings/binary.md)<br/>
    **Used By:** [`zCFont`](font.md)

*ZenGin* texture files contain [DXT1-5](#dxt), [palette](#palette) or [raw](#raw) textures alongside [mipmaps](#mipmaps)
of each texture. Textures are contained within binary files which start with a header followed by palette and image data.

Standalone textures are found within the `Textures.vdf` file found in the `Data/` directory in the Gothic installation.
They are sometimes embedded into other formats too, though.

## Format Description

Textures are stored in a [binary](../encodings/binary.md) file which contains the following data. Also refer to the
[Datatype Reference](../datatypes.md) for general information about often used datatypes.

```c title="Texture Structure"
enum zCTexture_Format {
    bgra8  = (uint32_t) 0, // 32-bit BGRA pixel format with alpha, using 8 bits per channel
    rgba8  = (uint32_t) 1, // 32-bit RGBA pixel format with alpha, using 8 bits per channel
    abgr8  = (uint32_t) 2, // 32-bit ABGR pixel format with alpha, using 8 bits per channel
    argb8  = (uint32_t) 3, // 32-bit ARGB pixel format with alpha, using 8 bits per channel
    bgr8   = (uint32_t) 4, // 24-bit BGR pixel format with 8 bits per channel
    rgb8   = (uint32_t) 5, // 24-bit RGB pixel format with 8 bits per channel
    argb4  = (uint32_t) 6, // 16-bit ARGB pixel format with 4 bits for each channel
    a1rgb5 = (uint32_t) 7, // 16-bit ARGB pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha
    r5g6b5 = (uint32_t) 8, // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue
    p8     = (uint32_t) 9, // 8-bit color indexed
    dxt1   = (uint32_t) 10, // DXT1 compression texture format
    dxt2   = (uint32_t) 11, // DXT2 compression texture format
    dxt3   = (uint32_t) 12, // DXT3 compression texture format
    dxt4   = (uint32_t) 13, // DXT4 compression texture format
    dxt5   = (uint32_t) 14, // DXT5 compression texture format
};

struct zCTexture {
    char magic[4]; // Always `"ZTEX"`.
    uint version; // Always 0.
    uint format; // One of enum zCTexture_Format
    uint width; // The width of the texture in pixels.
    uint height; // The height of the texture in pixels.
    uint numMipmaps; // The number of mipmap stored.
    uint referenceWidth;
    uint referenceHeight;
    uint averageColor; // A 32-bit ARGB value containing the average color of the image.
    
    // Texture data. See below for more information.
    union {
        struct {
            uint palette[256];
            byte palettePixels[/* (width * height) */];
        };
        
        byte rawPixels[/* (width * height * pixelsize(format) */]; // Depends on the pixel format (see zCTexture_Format)
    } mipmaps[/* numMipmaps */];
};
```

!!! note
    All color values should be understood as [word-order](https://en.wikipedia.org/wiki/RGBA_color_model#Representation),
    so an `RGBA` color `#aabbccdd` would be represented as the bytes `dd cc bb aa` in little-endian byte order. An alpha
    byte of `0xff` means full opacity.

### Mipmaps

Mipmaps are stored from the smallest to the largest image. If the mipmap-image with the level `0` has a with of `w`
and a height of `h` pixels then the mipmap image with the level `n` has a width of `w >> n` and a height of `h >> n`
pixels (each mipmap-level is half the size of the previous one).

### Format: DXT

If the texture format is `dxt1`, `dxt2`, `dxt3`, `dxt4` or `dxt5` the data stored right after the header is DXT
compressed. To figure out the size of each mipmap-level in bytes, for DXT1 compressed data calculate
`max(1, w / 4) * max(1, h / 4) * 8`[^1] and for DXT2-DXT5 calculate `max(1, w / 4) * max(1, h / 4) * 16`[^1] where `w`
and `h` depend on the [mipmap](#mipmaps) level. In practice, only DXT1, DXT3 AND DXT5 are used.

### Format: Palette

With the `p8` format, the texture file contains a palette-list before the actual texture data. This palette follows
right after the header and contains `256` 32-bit ARGB8 colors. These are then referenced in the texture data section
right after the palette data. Each byte in the texture data represents one pixel. Its color is the color in the palette
at the index indicated by the byte's value.

### Format: Raw

If the format is not `p8` and not one of the `dxt` formats, the texture stores raw data according to the format
described above. For `bgra8`, `rgba8`, `abgr8` and `argb8` formats, each pixel is 4 bytes in size. With
formats `bgr8` and `rgb8`, each pixel is 3 bytes in size. `argb4`, `a1rgb5` and `r5g6b5` contain
2-byte sized pixels.

[^1]: This calculation has been adopted from [ZenLib](https://github.com/ataulien/ZenLib/blob/master/zenload/ztex2dds.cpp#L71-L77).
