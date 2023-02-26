# ZenGin Animations

!!! abstract inline end "Quick Infos"
    **Type:** Asset Format<br/>
    **Format Name:** Model Animation<br/>
    **File Extension:** `.MAN`<br/>
    **Class Name:** `zCModelAni`<br/>
    **Encoding:** [Binary, Chunked](../encodings/binary-chunked.md)<br/>

*ZenGin* animation files contain skeletal animations for rigged meshes. Each file contains the data for one animation,
including metadata and animation samples.

Animations are found within the `Anims.vdf` file found in the `Data/` directory in the Gothic installation.

## Format Description

Model animations are stored in a [chunked binary](../encodings/binary-chunked.md) file which contains the following
chunks. Also refer to the [Datatype Reference](../datatypes.md) for general information about often used datatypes.

=== "`0xA000`"

    ```c title="Marker Chunk"
    struct zCModelAni_Marker {
    };
    ```

    The marker chunks marks the beginning of a model animation file.

=== "`0xA010`"

    ```c title="Source Chunk"
    struct zCModelAni_Source {
        zDATE date; // Broken. See below.
        string sourcePath;
        string sourceScript;
    };
    ```

    This source chunks was originally written to the file using a function called
    `zCFileBIN::WriteBinSrcFileStats`. It has a bug which causes it to use an uninitialized
    `zDATE` and write it to the output file. This is why the values in the date don't make
    any sense. Tread with care.

=== "`0xA020`"

    ```c title="Header Chunk"
    struct zCModelAni_Header {
        ushort version;
        string name;
        uint layer;
        uint numFrames;
        uint numNodes;
        float fps;
        float fpsSource;

        float samplePositionRangeMin;
        float samplePositionScalar;
        
        zTBBox3D bounds;
        string next;
    };
    ```

=== "`0xA030`"

    ```c title="Events Chunk"
    struct zCModelAni_Events {
        uint numEvents;
        zCModelAni_Events_Entry entries[/* numEvents */];
    };

    struct zCModelAni_Events_Entry {
        uint type;
        uint number;
        string tag;
        string content[4];
        float values[4];
        float probability;
    };
    ```

=== "`0xA090`"

    ```c title="Samples Chunk"
    struct zCModelAni_Samples {
        uint modelId;
        uint nodeIndices[/* zCModelAni_Header.numNodes */];

        struct zTMdl_AniSample {
            ushort rotation[3];
            ushort position[3];
        } samples[/* zCModelAni_Header.numNodes * zCModelAni_Header.numFrames */];
    };
    ```

    The `zCModelAni.modelId` field is used to match compatible the animation to a specific model. It is the same for
    assets belonging to the same model. Linked assets are `zCModelAni.checksum`, `zCModelMeshLib.checksum` and
    `zCModelHierarchy.checksum`.

### (todo) How animated models are loaded

Animated models are loaded by using their associated model script file. While parsing the model script, the parser
will encounter a `meshAndTree` directive (in MSB files a chunk of type `0xf300`) which points to a model hierarchy.

This model hierarchy is then loaded and acts as the ground truth of the model. If a hierarchy can't be loaded for any
reason, the engine tries to load a model mesh instead.

The model script parser continues and eventually hits the `aniEnum` section. While loading it, it will come across
`ani` sections. Those sections contain a model and animation name which are then loaded as animations and model meshes
respectively.

Linking these parts together is a checksum calculates from the list of hierarchy nodes. It is just the CRC32 checksum
of all node names appended after each other in the order they are saved in. I.e. a model hierarchy with two nodes
`"BIP01"` and `"BIP01 LEVER STICK"` (in that order) will have a checksum of `ea809bf7` (which
is `crc32("BIP01BIP01 LEVER STICK")`.
