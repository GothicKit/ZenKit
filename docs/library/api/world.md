# Worlds

### Loading Worlds

=== "C"

    ```c title="Example"
    #include <zenkit-capi/World.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkWorld* world = ZkWorld_loadPath("OLDWORLD.ZEN");
        ZkWorld_del(world);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Worlds.vdf", ZkVfsOverwriteBehavior_OLDER);
        world = ZkWorld_loadVfs(vfs, "OLDWORLD.ZEN");
        ZkWorld_del(world);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/World.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::World world {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("OLDWORLD.ZEN");
        world.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Worlds.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("OLDWORLD.ZEN")->open_read();
        world.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var world = new World("OLDWORLD.ZEN");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Worlds.vdf", VfsOverwriteBehavior.Older);
    world = new World(vfs, "OLDWORLD.ZEN");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.world.World;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var world = World.load("OLDWORLD.ZEN");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Worlds.vdf", VfsOverwriteBehavior.OLDER)
    world = World.load(vfs, "OLDWORLD.ZEN");
    ```

=== "Python"

    ```python title="Example"
    from zenkit import Vfs, VfsOverwriteBehavior, World

    # Load from a file on disk:
    world = World.load("OLDWORLD.ZEN")

    # ... or from a VFS:
    vfs = Vfs()
    vfs.mount_disk("Worlds.vdf", clobber=VfsOverwriteBehavior.OLDER)
    world = World.load(vfs.find("OLDWORLD.ZEN"))
