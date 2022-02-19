# the _phoenix_ project
[![pipeline status](https://gitlab.com/lmichaelis/phoenix/badges/main/pipeline.svg)](https://gitlab.com/lmichaelis/phoenix/-/commits/main)

The _phoenix_ project aims to re-implement the _ZenGin_ made by [Piranha Bytes](https://www.piranha-bytes.com/) for their early-2000s games [Gothic](https://en.wikipedia.org/wiki/Gothic_(video_game)) and [Gothic II](https://en.wikipedia.org/wiki/Gothic_II). It is heavily based on [ZenLib](https://github.com/Try/ZenLib) which is used as a reference implementation for the different file formats used.

File formats currently supported are:
* `.VDF` __-__ container for a file/directory structure similar to _tar_
* `.MAN` __-__ basic model animations
* `.FNT` __-__ font UV mappings
* `.MRM` __-__ basic static mesh with attached materials
* `OU.BIN`/`OU.CSL`/`OU.DAT`/`OU.LSC` __-__ speech/dialog text to sound mappings
* `.DAT` __-__ compiled daedalus script
* `.TEX` __-__ textures
* `.ZEN` __-__ world meshes, BSP trees and the world's VOB (entity or object) tree
* `.MSH` __-__ basic mesh
* `.MMB` __-__ _"morph mesh"_ which wraps a static mesh (`MRM`)
* `.MDH` __-__ _"model hierarchy"_
* `.MDM` __-__ _"model mesh"_ __/__ _"soft skin mesh"_
* `.MDL` __-__ combined `MDH` and `MDM`

File formats currently __NOT__ supported:
* `.MDS` __-__ _"model script"_ files containing mesh/animation components, transitions and sound effects
