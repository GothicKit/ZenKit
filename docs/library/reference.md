# File Type Reference

The *ZenGin* uses a lot of custom file formats to store game data like meshes, animations and textures. Originally these
custom formats for specifically made with [Direct3D][] and later [DirectX][] in mind and are thus sometimes confusing to
understand or use some old, proprietary APIs common at the time. One example of this is [DirectMusic][] which is used 
or the in-game music.

All files used by the *ZenGin* are binary files with the little-endian byte order or text files encoded with the
[Windows-1252][] character set.

## 1st-party formats

The following is a list of file types and formats used by ZenGin.

| Format                                                |           Extension            | Description                                                                        | _ZenKit_ Class Name   |
|-------------------------------------------------------|:------------------------------:|------------------------------------------------------------------------------------|-----------------------|
| [Model Animation](api/model-animation.md)             |             `.MAN`             | Contains animations for a model                                                    | `ModelAnimation`      |
| [Model Hierarchy](api/model-hierarchy.md)             |             `.MDH`             | Contains skeletal information for a model                                          | `ModelHierarchy`      |
| [Model Mesh](api/model-mesh.md)                       |             `.MDM`             | Contains the mesh of a model                                                       | `ModelMesh`           |
| [Model](api/model.md)                                 |             `.MDL`             | Contains a mesh and a hierarchy which make up a model                              | `Model`               |
| [Morph Mesh](api/morph-mesh.md)                       |             `.MMB`             | Contains a morph mesh with its mesh, skeleton and animation data                   | `MorphMesh`           |
| [Multi Resolution Mesh](api/multi-resolution-mesh.md) |             `.MRM`             | Contains a mesh with [LOD][] information                                           | `MultiResolutionMesh` |
| [Mesh](api/mesh.md)                                   |             `.MSH`             | Contains mesh vertices and vertex features like materials                          | `Mesh`                |
| [Daedalus Script](api/daedalus-script.md)             |             `.DAT`             | Contains a compiled _Daedalus_ script                                              | `DaedalusScript`      |
| [Texture](api/texture.md)                             |             `.TEX`             | Contains texture data in a variety of formats                                      | `Texture`             |
| [Font](api/font.md)                                   |             `.FNT`             | Contains font data                                                                 | `Font`                |
| [ZenGin Archive](api/archive.md)                      |             `.ZEN`             | Contains various structured data (general object persistence).                     | `ReadArchive`         |
| [Text/Cutscenes](api/cutscene-library.md)             | `.BIN`, `.CSL`, `.DAT`, `.LSC` | Contains text and cutscene data                                                    | `CutsceneLibrary`     |
| [Model Script](api/model-script.md)                   |         `.MDS`, `.MSB`         | Contains model animation script data and associated hierarchy and mesh information | `ModelScript`         |
| [Virtual File System](api/virtual-file-system.md)     |             `.VDF`             | Contains a directory structure containing multiple files; similar to [TAR][].      | `Vfs`                 |

## 3rd-party formats

The *ZenGin* uses the following 3rd-party file formats:

- [DirectMusic][] for the soundtrack
- [DXT1][], [DXT3][] and [DXT5][] for texture compression
- [WAV][] for speech and sound effects
- [TGA][] for uncompressed images
- [Bink][] for cutscene videos

For all of these, Open Source parsers are available, however some of them are no longer maintained. For *DirectMusic*
there is [libdmusic][] (_unmaintained_), for *DXT* decompression there is [libsquish][] (_unmaintained_), [stb][] can
also decompress *DXT*-compressed files and can read in *TGA* files and the amazing [ffmpeg][] can decode Bink video.

---

[Direct3D]: https://en.wikipedia.org/wiki/Direct3D#Direct3D_8.0
[DirectX]: https://en.wikipedia.org/wiki/DirectX#DirectX_9
[DirectMusic]: https://en.wikipedia.org/wiki/DirectMusic
[Windows-1252]: https://en.wikipedia.org/wiki/Windows-1252
[LOD]: https://en.wikipedia.org/wiki/Level_of_detail_(computer_graphics)
[DXT1]: https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT1
[DXT3]: https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT2_and_DXT3
[DXT5]: https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT4_and_DXT5
[WAV]: https://en.wikipedia.org/wiki/WAV
[TGA]: https://en.wikipedia.org/wiki/Truevision_TGA
[Bink]: http://www.radgametools.com/bnkmain.htm
[TAR]: https://en.wikipedia.org/wiki/Tar_(computing)
[libdmusic]: https://github.com/libdmusic/libdmusic
[libsquish]: https://sourceforge.net/projects/libsquish/
[stb]: https://github.com/nothings/stb
[ffmpeg]: https://github.com/FFmpeg/FFmpeg
