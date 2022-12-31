# ZenGin Fonts

Fonts for use by the *ZenGin* are saved in a custom format which maps sections of an image to each character of the
alphabet. To that end, each font contains 256 glyphs which correspond to the characters of the
[Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) character encoding. For example, the glyph at index `0xE4`
corresponds to the character `ä`.

## Overview

*phoenix'* implementation of fonts lives in `include/phoenix/font.hh` and `source/font.cc`. Every font contains the
name of the image containing the appearances of each glyph as well as a list of glyphs. Each glyph contains two
coordinates which together form a rectangle around the glyph in the image.
See [Dealing with glyphs](#dealing-with-glyphs)
for more details

### Loading a Font

Like most data structures in *phoenix*, fonts can be loaded using the `#!cpp phoenix::font::parse()` function.
It takes a `phoenix::buffer` as a parameter and loads the font from it.

```cpp title="Example"
#include <phoenix/font.hh>

int main(int, const char** argv) {
    auto font_buffer = phoenix::buffer::mmap("A.fnt");
    [[maybe_unused]] auto font = phoenix::font::parse(font_buffer);
    return 0;
}
```

### Dealing with glyphs

To render a font, first the font [Texture](texture.md) must be loaded. Its name is saved in the `phoenix::font::name`
variable. Each of the 256 glyphs of the font then contains two UV-coordinates denoting the top and bottom corner of a
subsection of said texture. These UV-coordinates are between 0 and 1, so they have to be scaled to the actual width
and height of the image by multiplying them. The subsection contains the actual image data for the glyph.

Since each glyph is saved at the same index as the number representation of its character in
[Windows-1252](https://en.wikipedia.org/wiki/Windows-1252), to get the glyph for the character `'a'`, one can just
`font.glyphs[(int) 'a']` to get the correct glyph name, assuming that `'a'` follows  *Windows-1252* encoding.

```cpp title="Example"
#include <phoenix/font.hh>
#include <phoenix/texture.hh>

int main(int, const char** argv) {
    auto font_buffer = phoenix::buffer::mmap("A.fnt");
    auto font = phoenix::font::parse(font_buffer)

    // The texture is loaded from some other location, probably the `Textures.vdf` file.
    auto font_texture = phoenix::texture::parse(phoenix::buffer::mmap(font.name));

    phoenix::glyph glyph0 = font.glyphs[0];

    // Each UV coordinate contains a value from 0 to 1 which is mapped to the actual with and height of the image
    int actual_top_x = glyph0.uv[0].x * font_texture.width();
    int actual_top_y = glyph0.uv[0].y * font_texture.height();

    int actual_bottom_x = glyph0.uv[1].x * font_texture.width();
    int actual_bottom_y = glyph0.uv[1].y * font_texture.height();
    
    return 0;
}
```
