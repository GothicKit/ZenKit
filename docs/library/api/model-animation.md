# Model Animations

*Model Animations* form part of the animations system of the *ZenGin* they contain only animation samples, i.e. the
position and orientation of each bone of the skeleton they're applied to. While there is space for additional data
within animation files, it is mostly empty.

## Overview

The most important part of an animation are its samples. They are stored in `ModelAnimation::samples` after it has been
parsed. Animation files themselves don't contain information about when to run animation or any other effects which
should be applied during it. Those parts of the animation system are defined in [Model Script](model-script.md) files
which should be loaded before animations. The `ModelAnimation::events` field will always be empty for that reason[^1].


### Loading Animations

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Animation.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyAnimation.MAN");
        PxAnimation* model = pxAniLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxAniDestroy(model);
        return 0;
    }
    ```

    !!! info
        As with all resources, animations can also be loaded from a [virtual file system](virtual-file-system.md) by
        passing a `PxVfs` and the file name to `pxAniLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelAnimation.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::ModelAnimation ani {};
        
        auto r = zenkit::Read::from("MyAnimation.MAN");
        ani.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, animations can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::ModelAnimation::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.

[^1]: This assertion is made after examining many animations from both *Gothic* and *Gothic II*
      and finding no events being stored.
