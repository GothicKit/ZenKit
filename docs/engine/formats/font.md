# ZenGin Fonts

!!! abstract inline end "Quick Infos"
    **Type:** Asset Format<br/>
    **Format Name:** Font<br/>
    **File Extension:** `.FNT`<br/>
    **Class Name:** `zCFont`<br/>
    **Encoding:** [Binary](../encodings/binary.md)<br/>

*ZenGin* font files contain metadata for bitmap fonts used in the engine. In particular, they contain the position and
size of font glyphs in a texture file.

Fonts are found within the `Textures.vdf` file found in the `Data/` directory in the Gothic installation.

## Format Description

Fonts are stored in a [binary](../encodings/binary.md) file which contains the following data. Also refer to the
[Datatype Reference](../datatypes.md) for general information about often used datatypes.

```c title="Font Structure"
struct zCFont {
    string version; // Always "1".
    string imageName;
    uint glyphHeight;
    uint glyphCount; 
    
    byte glyphWidths[/* glyphCount */];
    zVEC2 glyphTopLeftUVs[/* glyphCount */];
    zVEC2 glyphBottomRightUVs[/* glyphCount */];
};
```

Since *ZenGin* fonts are a kind of bitmap font, a font [texture](texture.md) is required in addition to this font
metadata file. The name of that texture is found in `imageName`.

To get the actual pixel coordinates for each glyph in the texture for any given UV-coordinate, multiply the `x`
coordinate by the width of the texture and the `y` coordinate by the height of the texture.

!!! warning
    Some coordinates are negative. These should be ignored since they don't have a glyph image associated with them.
