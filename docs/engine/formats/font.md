# ZenGin Fonts

Font (or *FNT*) files contain metadata of fonts for use with the *ZenGin*. *FNT* files only contain font metadata which
points into a TGA file containing the actual font glyphs.

!!! important
    *FNT* files are binary files which are always encoded with the little-endian byte order. The number `0xCAFEBABE`
    will be represented as `BE BA FE CA` when viewing the file in a hex editor.

Font files are structured like this:

```c title="Font file structure"
#pragma pack(push, 1)
typedef struct vec2 {
    float u;
    float v;
} vec2_t;

struct fnt_file {
    char version[2]; // A fixed string denoting the version of the font file. It's always `"1\n"`
    char* font_image; // A newline (`"\n"`) terminated string containing the name of the font image file.
    uint32_t glyph_height; // The height of each glyph in pixels.
    uint32_t glyph_count; // The number of glyphs stored in this file.
    
    uint8_t glyph_widths[]; // An array of length `glyph_count` containing the width of each glyph.
    vec2_t glyph_top_left_uvs[]; // An array of length `glyph_count` containing the top left UV-coordinate of each glyph.
    vec2_t glyph_bottom_right_uvs[]; // An array of length `glyph_count` containing the bottom right UV-coordinate of each glyph.
};
#pragma pack(pop)
```

!!! note
    To get the actual pixel coordinate in the glyph image for any given UV-coordinate, multiply the `x` UV-coordinate
    by the width of the image and the `y` UV-coordinate by the height.

!!! warning
    Some UV-coordinates are negative. These should be ignored since they don't have a glyph image associated with them.

!!! info "File Location"
    Glyph image files can normally be found in `Textures.vdf` inside the `_WORK/DATA/TEXTURES/FONTS/NOMIP/`
    (*TGA* files) and `_WORK/DATA/TEXTURES/_COMPILED/` (*TEX* files) folders.

    The font (*FNT*) files can be found in `Textures.vdf` inside the `_WORK/DATA/TEXTURES/_COMPILED/` folder.
