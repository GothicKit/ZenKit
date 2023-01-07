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
        uint checksum;
        uint nodeIndices[/* zCModelAni_Header.numNodes */];
        ushort rotation[3];
        ushort position[3];
    };
    ```
