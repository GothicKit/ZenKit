# Virtual File Systems

*Virtual Disk Files* (or `VDFs`) are container files like [ZIP][] or, more accurately [TAR][]. A VDF contains a
directory structure with multiple files within it. Originally, *Gothic* and *Gothic II* used a library called
[PhysicsFS][] to load these files and read from them. While *PhysicsFS* now supports VDFs out-of-the-box, _ZenKit_
implements its own, modern parser for them. This page provides a high-level overview of the VDF implementation in
_ZenKit_.

The original VDF implementation shipped with *Gothic* and *Gothic II* was not written by *Piranha Bytes* themselves but
by a now defunct company called [TRIACOM Software][].

## Overview

### Using Virtual File Systems

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Vfs.h>

    #include <stdio.h>

    int main(int, const char** argv) {
        PxVfs* vfs = pxVfsNew();
        pxVfsMountDisk(vfs, "Worlds.VDF", PxVfsOverwrite_Older)

        PxVfsNode const* node = pxVfsGetNodeByName(vfs, "OLDWORLD.ZEN");
        if (node == NULL) {
            printf("Error: OLDWORLD.ZEN not found!");
            return -1;
        }

        PxBuffer* buf = pxVfsNodeOpenBuffer(node);

        // ...

        pxBufferDestroy(buf);
        pxVfsDestroy(tex);
        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Vfs.hh>
    #include <zenkit/Stream.hh>

    #include <iostream>

    int main(int, char const** argv) {
        zenkit::Vfs vfs {};
        vfs.mount_disk("Worlds.VDF");

        zenkit::VfsNode* node = vfs.find("OLDWORLD.ZEN");
        if (node == nullptr) {
            printf("Error: OLDWORLD.ZEN not found!");
            return -1;
        }

        std::unique_ptr<zenkit::Read> r = node->open_read();

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, texture can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::Texture::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.

[ZIP]: https://en.wikipedia.org/wiki/ZIP_(file_format)
[TAR]: https://en.wikipedia.org/wiki/Tar_(computing)
[PhysicsFS]: https://icculus.org/physfs/
[TRIACOM Software]: https://www.northdata.com/Triacom+Software+GmbH,+Bochum/Amtsgericht+Gelsenkirchen+HRB+6340
