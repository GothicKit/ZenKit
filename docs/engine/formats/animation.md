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

        float samplePositionMin;
        float samplePositionScale;
        
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

        struct zTMdl_AniSample {
            ushort rotation[3];
            ushort position[3];
        } samples[/* zCModelAni_Header.numNodes * zCModelAni_Header.numFrames */];
    };
    ```

    The `zCModelAni.checksum` field is used to match compatible the animation to a specific model. It is the same for
    assets belonging to the same model. Linked assets are `zCModelAni.checksum`, `zCModelMeshLib.checksum` and
    `zCModelHierarchy.checksum`.
    
    It is important to understand that the animation samples do not contain the actual values for the rotation and
    position. To save on memory and disk space consumption, the values are packed into shorts which are converted
    back to floats on demand. See [Sample Positions](#sample-positions) and [Sample Rotations](#sample-rotations) for
    more information.

--- 

### Sample Positions

The positions are represented as multiples of `zCModelAni_Header.samplePositionScale`. The formula for packing a
given `zVEC3` position into the stored format can be summarized like this:

$$
    \begin{bmatrix}
        x_{p} \\
        y_{p} \\
        z_{p}
    \end{bmatrix}
    =
    \Bigg(-v_{min} +
    \begin{bmatrix}
        x \\
        y \\
        z
    \end{bmatrix}
    \Bigg) \cdot \frac{65535}{v_{max} - v_{min}}
$$

where $v_{min}$ is the smallest value across all components of all sample positions of the animation and $v_{max}$
is the maximum value. The output vector $\begin{smallmatrix}x_{p} \\ y_{p} \\ z_{p} \end{smallmatrix}$ then contains a
scaled representation of the input vector $\begin{smallmatrix}x \\ y \\ z\end{smallmatrix}$ as three 16-bit unsigned
integers. This operation is performed by `zCModelAni::AddTrafoMatrix` and `zTMdl_AniSample::PackTrans`.

Alongside these packed positions, both $v_{min}$ and the constant $s = (\frac{65535}{v_{max} - v_{min}})^{-1}$ are saved
as `zCModelAni_Header.samplePositionMin` and `zCModelAni_Header.samplePositionScale` respectively. Thus, to convert
the stored position back to its floating point representation, the following calculation may be applied:

$$
    \begin{bmatrix}
        x \\
        y \\
        z
    \end{bmatrix}
    = 
    \begin{bmatrix}
        x_{p} \\
        y_{p} \\
        z_{p}
    \end{bmatrix}
    \cdot s + v_{min}
$$

where $x_{p}$ , $y_{p}$ and $z_{p}$ are `zTMdl_AniSample.position[0]`, `zTMdl_AniSample.position[1]` and
`zTMdl_AniSample.position[2]` respectively. This operation is performed by `zTMdl_AniSample::UnpackTrans` .

### Sample Rotations

The rotations are represented as a packed quaternion which is calculated in the following way:

$$
    \begin{bmatrix}
        x_{p} \\
        y_{p} \\
        z_{p}
    \end{bmatrix}
    =
    \begin{cases}
        \begin{bmatrix}
            x \\
            y \\
            z
        \end{bmatrix}
        \div s + h  & \quad \text{if } w \geq 0 \\
        -\Bigg(
        \begin{bmatrix}
            x \\
            y \\
            z
        \end{bmatrix}
        \div s + h\Bigg) - 2  & \quad \text{if } w < 0 \\
    \end{cases}
$$

where

$$
    s = \frac{1}{2^{16} - 1} * 2.1 = \frac{1}{65535} * 2.1
$$

and

$$
    h = 2^{15} - 1 = 32767
$$

This operation packs a quaternion of the form $r = \begin{smallmatrix}x \\ y \\ z \\ w\end{smallmatrix}$ into a set of
three 16-bit unsigned integers $x_{p}$ , $y_{p}$ and $z_{p}$ which are stored into the sample. **The input quaternion
$r$ is required to be a unit vector**. This operation was performed by `zCModelAni::AddTrafoMatrix` and
`zTMdl_AniSample::PackQuat`.

They can be unpacked using some quaternion wizardry as described below.

$$
    \begin{bmatrix}
        x \\
        y \\
        z \\
        w 
    \end{bmatrix}
    =
    \begin{cases}
        \begin{bmatrix}
            x_{t} \\
            y_{t} \\
            z_{t} \\
            \sqrt{1 - l}
        \end{bmatrix} & \quad \text{if } l \leq 1 \\
        \begin{bmatrix}
            x_{t} \\
            y_{t} \\
            z_{t} \\
            0
        \end{bmatrix}
        \cdot \frac{1}{\sqrt{l}} & \quad \text{if } l > 1
    \end{cases} 
$$

where

$$
    \begin{bmatrix}
        x_{t} \\
        y_{t} \\
        z_{t} \\
    \end{bmatrix}
    =
    \Bigg(\begin{bmatrix}
        x_{p} \\
        y_{p} \\
        z_{p} \\
    \end{bmatrix} - h\Bigg) \cdot s
$$

and

$$
    l = (x_{t})^2 + (y_{t})^2 + (z_{t})^2
$$

The values $x_{p}$ , $y_{p}$ and $z_{p}$ for this computation are stored in `zTMdl_AniSample.rotation[0]`,
`zTMdl_AniSample.rotation[1]` and `zTMdl_AniSample.rotation[2]` respectively. This is possible since the quaternion
$r$ used to calculate the packed rotation is guaranteed to be a unit-quaternion, thus, if we have three components
(like we do), it is possible to calculate the third component. This operation is performed by
`zTMdl_AniSample::UnpackQuat`.

---

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
