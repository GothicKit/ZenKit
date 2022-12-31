# File Type Reference

The *ZenGin* uses a lot of custom file formats to store game data like meshes, animations and textures. Originally these
custom formats for specifically made with [Direct3D](https://en.wikipedia.org/wiki/Direct3D#Direct3D_8.0) and later
[DirectX](https://en.wikipedia.org/wiki/DirectX#DirectX_9) in mind and are thus sometimes confusing to understand or
use some proprietary API developed by Microsoft at the time. One example of this is
[DirectMusic](https://en.wikipedia.org/wiki/DirectMusic) which is used for the in-game music.

All files used by the *ZenGin* are binary files with the little-endian byte order or text files encoded with the
[Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) character set.

## 1st-party formats

The following is a list of file types and formats used by ZenGin.

| Format                                                    |           Extension            | Description                                                                                                                | _phoenix_ Class Name |
|-----------------------------------------------------------|:------------------------------:|----------------------------------------------------------------------------------------------------------------------------|----------------------|
| [Model Animation](formats/model-animation.md)             |             `.MAN`             | Contains animations for a model                                                                                            | `animation`          |
| [Model Hierarchy](formats/model-hierarchy.md)             |             `.MDH`             | Contains skeletal information for a model                                                                                  | `model_hierarchy`    |
| [Model Mesh](formats/model-mesh.md)                       |             `.MDM`             | Contains the mesh of a model                                                                                               | `model_mesh`         |
| [Model](formats/model.md)                                 |             `.MDL`             | Contains a mesh and a hierarchy which make up a model                                                                      | `model`              |
| [Morph Mesh Binary](formats/morph-mesh-binary.md)         |             `.MMB`             | Contains a morph mesh with its mesh, skeleton and animation data                                                           | `morph_mesh`         |
| [Multi Resolution Mesh](formats/multi-resolution-mesh.md) |             `.MRM`             | Contains a mesh with [CLOD](https://en.wikipedia.org/wiki/Level_of_detail_(computer_graphics)) information                 | `proto_mesh`         |
| [Mesh](formats/mesh.md)                                   |             `.MSH`             | Contains mesh vertices and vertex features like materials                                                                  | `mesh`               |
| [Daedalus Script Binaries](scripting/script.md)           |             `.DAT`             | Contains a compiled _Daedalus_ script                                                                                      | `script`             |
| [Texture](formats/texture.md)                             |             `.TEX`             | Contains texture data in a variety of formats                                                                              | `texture`            |
| [Font](formats/font.md)                                   |             `.FNT`             | Contains font data                                                                                                         | `font`               |
| [ZenGin Archive](formats/archive.md)                      |             `.ZEN`             | Contains various structured data. Used mostly for world hierarchy data and object persistence.                             | `archive`            |
| [Text/Cutscenes](formats/cutscene.md)                     | `.BIN`, `.CSL`, `.DAT`, `.LSC` | Contains text and cutscene data                                                                                            | `messages`           |
| [Model Script](formats/model-script.md)                   |         `.MDS`, `.MSB`         | Contains model animation script data and associated hierarchy and mesh information                                         | `model_script`       |
| [Virtual Disk](formats/vdf.md)                            |             `.VDF`             | Contains a directory structure containing multiple files; similar to [tar](https://en.wikipedia.org/wiki/Tar_(computing)). | `vdf_file`           |

## 3rd-party formats

The *ZenGin* uses the following 3rd-party file formats:

* [DirectMusic](https://en.wikipedia.org/wiki/DirectMusic) for the soundtrack
* [DXT1](https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT1),
  [DXT3](https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT2_and_DXT3)
  and [DXT5](https://en.wikipedia.org/wiki/S3_Texture_Compression#DXT4_and_DXT5) for texture compression
* [WAV](https://en.wikipedia.org/wiki/WAV) for speech and sound effects
* [TGA](https://en.wikipedia.org/wiki/Truevision_TGA) for uncompressed images
* [Bink](http://www.radgametools.com/bnkmain.htm) for cutscene videos

For all of these, Open Source parsers are available, however some of them are no longer maintained. For *DirectMusic*
there is [libdmusic](https://github.com/libdmusic/libdmusic), for *DXT* decompression there is
[squish](https://sourceforge.net/projects/libsquish/), [stb](https://github.com/nothings/stb) can also decompress
*DXT*-compressed files and can read in *TGA* files and [ffmpeg](https://github.com/FFmpeg/FFmpeg) can load Bink video.
