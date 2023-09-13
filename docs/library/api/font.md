# Fonts

*ZenGin* fonts are stored in a custom format which maps sections of an image to each character of the alphabet. To that
end, each font contains 256 glyphs which correspond to the characters of the [Windows-1252][] character encoding. 

## Overview

Every font contains the name of the image containing the appearances of each glyph as well as a list of glyphs. Each
glyph contains two coordinates which together form a rectangle around the glyph in the image. See
[Dealing with glyphs](#dealing-with-glyphs) for more details

### Loading Fonts

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Font.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("FONT_OLD_20.FNT");
        PxFont* font = pxFntLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxFntDestroy(font);
        return 0;
    }
    ```

    !!! info
        As with all resources, fonts can also be loaded from a [virtual file system](virtual-file-system.md) by
        passing a `PxVfs` and the file name to `pxFntLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Font.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::Font font {};
        
        auto r = zenkit::Read::from("FONT_OLD_20.FNT");
        font.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, fonts can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::Font::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.

### Dealing with glyphs

To render a font, first the font [texture](texture.md) must be loaded. Its name is stored in the `Font::name` field.
Each of the 256 glyphs of the font then contains two UV-coordinates denoting the top and bottom corner of a
subsection of said texture. These UV-coordinates are between 0 and 1, so they have to be scaled to the actual width
and height of the image by multiplying them. The subsection contains the actual image data for the glyph.

Since each glyph is saved at the same index as the number representation of its character in [Windows-1252][], to get
the glyph for the character `'a'`, one can just `font.glyphs[(int) 'a']` to get the correct glyph, assuming that `'a'`
follows  *Windows-1252* encoding.


=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Font.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("FONT_OLD_20.FNT");
        PxFont* font = pxFntLoad(buf);
        pxBufferDestroy(buf);
        
        // The texture (1) must be loaded from some other location,
        // most likely the `Textures.vdf` disk.
        buf = pxBufferMmap(font.name);
        PxTexture* font_texture = pxTexLoad(buf);
        pxBufferDestroy(buf);

        uint8_t glyph_width;
        PxVec2 glyph_upper, glyph_lower;

        // The second parameter denotes the index of the glyph to
        // get. There are 256 glyphs in every font but the number of
        // glyphs can also be retrieved using `pxFntGetGlyphCount`.
        pxFntGetGlyph(font, 0, &glyph_width, &glyph_upper, &glyph_lower);

        // To be able to determine the pixel offset of the glyph we
        // need to retrieve the size of the font texture.
        uint32_t texture_width, texture_height;
        pxTexGetMeta(font_texture, NULL, &texture_width, &texture_height, NULL, NULL);

        // Each UV coordinate contains a value from 0 to 1 which is
        // mapped to the actual with and height of the image
        int actual_top_x = glyph_upper.x * texture_width;
        int actual_top_y = glyph_upper.y * texture_height;

        int actual_bottom_x = glyph_lower.x * texture_width;
        int actual_bottom_y = glyph_lower.y * texture_height;

        // ...

        pxTexDestroy(font_texture);
        pxFntDestroy(font);
        return 0;
    }
    ```

    1. See [Textures](texture.md) for information about loading texture files.

    !!! info
        As with all resources, fonts can also be loaded from a [virtual file system](virtual-file-system.md) by
        passing a `PxVfs` and the file name to `pxFntLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Font.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::Font font {};
        
        auto r = zenkit::Read::from("FONT_OLD_20.FNT");
        font.load(r.get());

        // The texture (1) must be loaded from some other location,
        // most likely the `Textures.vdf` disk.
        auto r_texture = zenkit::Read::from(font.name);
        zenkit::Texture font_texture {};
        font_texture.load(r_texture.get());

        zenkit::FontGlyph glyph0 = font.glyphs[0];

        // Each UV coordinate contains a value from 0 to 1 which is
        // mapped to the actual with and height of the image
        int actual_top_x = glyph0.uv[0].x * font_texture.width();
        int actual_top_y = glyph0.uv[0].y * font_texture.height();

        int actual_bottom_x = glyph0.uv[1].x * font_texture.width();
        int actual_bottom_y = glyph0.uv[1].y * font_texture.height();

        // ...

        return 0;
    }
    ```

    1. See [Textures](texture.md) for information about loading texture files.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.

[Windows-1252]: https://en.wikipedia.org/wiki/Windows-1252
