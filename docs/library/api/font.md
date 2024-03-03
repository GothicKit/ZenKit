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
    #include <zenkit-capi/Font.h>

    int main(int, char**) {
        // Load from a file on disk:
        ZkFont* font = ZkFont_loadPath("FONT_OLD_20.FNT");
        ZkFont_del(font);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Textures.vdf", ZkVfsOverwriteBehavior_OLDER);
        font = ZkFont_loadVfs(vfs, "FONT_OLD_20.FNT");
        ZkFont_del(font);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Font.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::Font font {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("FONT_OLD_20.FNT");
        font.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Textures.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("FONT_OLD_20.FNT")->open_read();
        font.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var font = new Font("FONT_OLD_20.FNT");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Textures.vdf", VfsOverwriteBehavior.Older);
    font = new Font(vfs, "FONT_OLD_20.FNT");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.fnt.Font;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var fnt = new Font("FONT_OLD_20.FNT");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Textures.vdf", VfsOverwriteBehavior.OLDER)
    fnt = new Font(vfs, "FONT_OLD_20.FNT");
    ```

=== "Python"

    ```python title="Example"
    from zenkit import Vfs, VfsOverwriteBehavior, Font

    # Load from a file on disk:
    fnt = Font.load("FONT_OLD_20.FNT")

    # ... or from a VFS:
    vfs = Vfs()
    vfs.mount_disk("Textures.vdf", clobber=VfsOverwriteBehavior.OLDER)
    fnt = Font.load(vfs.find("FONT_OLD_20.FNT"))
    ```

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
    #include <zenkit-capi/Font.h>
    #include <zenkit-capi/Texture.h>

    int main(int, char**) {
        ZkFont* font = ZkFont_loadPath("FONT_OLD_20.FNT");

        // The texture (1) must be loaded from some other location,
        // most likely the `Textures.vdf` disk.
        ZkTexture* tex = ZkTexture_loadPath(ZkFont_getName(font));

        // The second parameter denotes the index of the glyph to get. There
        // are usually 256 glyphs in every font but the number of glyphs can
        // also be retrieved using `ZkFont_getGlyphCount()`.
        ZkFontGlyph glyph = ZkFont_getGlyph(font, 0);

        // To be able to determine the pixel offset of the glyph we
        // need to retrieve the size of the font texture.
        uint32_t texture_width = ZkTexture_getWidth(tex);
        uint32_t texture_height = ZkTexture_getHeight(tex);

        // Each UV coordinate contains a value from 0 to 1 which is
        // mapped to the actual with and height of the image
        int actual_top_x = glyph.topLeft.x * texture_width;
        int actual_top_y = glyph.topLeft.y * texture_height;

        int actual_bottom_x = glyph.bottomRight.x * texture_width;
        int actual_bottom_y = glyph.bottomRight.y * texture_height;

        // ...

        ZkTexture_del(tex);
        ZkFont_del(font);

        return 0;
    }
    ```

    1. See [Textures](texture.md) for information about loading texture files.

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

    ```c# title="Example"
    using ZenKit;

    var font = new Font("FONT_OLD_20.FNT");

    // The texture (1) must be loaded from some other location,
    // most likely the `Textures.vdf` disk.
    var tex = new Texture(font.Name);

    // The second parameter denotes the index of the glyph to get. There
    // are usually 256 glyphs in every font but the number of glyphs can
    // also be retrieved using `Font.GlyphCount`. You can also access all
    // glyphs at once through `Font.Glyphs`.
    FontGlyph glyph = font.GetGlyph(0);

    // Each UV coordinate contains a value from 0 to 1 which is
    // mapped to the actual with and height of the image
    float actual_top_x = glyph.topLeft.X * tex.Width;
    float actual_top_y = glyph.topLeft.Y * tex.Height;

    float actual_bottom_x = glyph.bottomRight.X * tex.Width;
    float actual_bottom_y = glyph.bottomRight.Y * tex.Height;

    // ...
    ```

    1. See [Textures](texture.md) for information about loading texture files.

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.fnt.Font;
    import dev.gothickit.zenkit.fnt.FontGlyph;
    import dev.gothickit.zenkit.tex.Texture;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    var font = new Font("FONT_OLD_20.FNT");

    // The texture (1) must be loaded from some other location,
    // most likely the `Textures.vdf` disk.
    var tex = new Texture(font.getName());

    // The second parameter denotes the index of the glyph to get. There
    // are usually 256 glyphs in every font but the number of glyphs can
    // also be retrieved using `Font.GlyphCount`. You can also access all
    // glyphs at once through `Font.Glyphs`.
    FontGlyph glyph = font.getGlyph(0);

    // Each UV coordinate contains a value from 0 to 1 which is
    // mapped to the actual with and height of the image
    float actualTopX = glyph.topLeft.x * tex.getWidth();
    float actualTopY = glyph.topLeft.y * tex.getHeight();

    float actualBottomX = glyph.bottomRight.x * tex.getWidth();
    float actualBottomY = glyph.bottomRight.y * tex.getHeight();

    // ...
    ```

    1. See [Textures](texture.md) for information about loading texture files.

[Windows-1252]: https://en.wikipedia.org/wiki/Windows-1252
