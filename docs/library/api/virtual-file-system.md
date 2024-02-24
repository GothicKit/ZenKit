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
    #include <zenkit-capi/Vfs.h>

    #include <stdio.h>

    int main(int, char**) {
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Worlds.vdf", ZkVfsOverwriteBehavior_OLDER);

        ZkVfsNode* node = ZkVfs_findNode(vfs, "OLDWORLD.ZEN");
        if (node == NULL) {
            printf("Error: OLDWORLD.ZEN not found!\n");
            return -1;
        }

        ZkRead* rd = ZkVfsNode_open(node);
        // ...
        ZkRead_del(rd);
        ZkVfs_del(vfs);

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
        vfs.mount_disk("Worlds.vdf");

        zenkit::VfsNode* node = vfs.find("OLDWORLD.ZEN");
        if (node == nullptr) {
            std::cerr << "Error: OLDWORLD.ZEN not found!\n";
            return -1;
        }

        std::unique_ptr<zenkit::Read> r = node->open_read();

        // ...

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    var vfs = new Vfs();
    vfs.MountDisk("Worlds.vdf", VfsOverwriteBehavior.Older);

    VfsNode? node = vfs.Find("OLDWORLD.ZEN");

    if (node == null)
    {
        Console.Error.WriteLine("Error: OLDWORLD.ZEN not found!");
        return;
    }

    Read rd = node.Buffer;

    // ...
    ```


=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsNode;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;
    import dev.gothickit.zenkit.Read;

    var vfs = new Vfs();
    vfs.mountDisk("Worlds.vdf", VfsOverwriteBehavior.OLDER);

    VfsNode node = vfs.find("OLDWORLD.ZEN");

    if (node == null)
    {
        System.err.println("Error: OLDWORLD.ZEN not found!");
        return;
    }

    Read rd = node.open();

    // ...
    ```

[ZIP]: https://en.wikipedia.org/wiki/ZIP_(file_format)
[TAR]: https://en.wikipedia.org/wiki/Tar_(computing)
[PhysicsFS]: https://icculus.org/physfs/
[TRIACOM Software]: https://www.northdata.com/Triacom+Software+GmbH,+Bochum/Amtsgericht+Gelsenkirchen+HRB+6340
