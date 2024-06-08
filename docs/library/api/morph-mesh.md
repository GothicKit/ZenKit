# Morph Meshes

### Loading Morph Meshes

=== "C"

    ```c title="Example"
    #include <zenkit-capi/MorphMesh.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkMorphMesh* mmb = ZkMorphMesh_loadPath("MyMesh.MMB");
        ZkMorphMesh_del(mmb);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Anims.vdf", ZkVfsOverwriteBehavior_OLDER);
        mmb = ZkMorphMesh_loadVfs(vfs, "MyMesh.MMB");
        ZkMorphMesh_del(mmb);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/MorphMesh.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::MorphMesh mmb {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyMesh.MMB");
        mmb.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Anims.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyMesh.MMB")->open_read();
        mmb.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var mmb = new MorphMesh("MyMesh.MMB");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Anims.vdf", VfsOverwriteBehavior.Older);
    mmb = new MorphMesh(vfs, "MyMesh.MMB");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.mmb.MorphMesh;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var mmb = MorphMesh.load("MyMesh.MMB");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Anims.vdf", VfsOverwriteBehavior.OLDER)
    mmb = MorphMesh.load(vfs, "MyMesh.MMB");
    ```

=== "Python"

    ```python title="Example"
    from zenkit import Vfs, VfsOverwriteBehavior, MorphMesh

    # Load from a file on disk:
    mmb = MorphMesh.load("MyMesh.MMB")

    # ... or from a VFS:
    vfs = Vfs()
    vfs.mount_disk("Anims.vdf", clobber=VfsOverwriteBehavior.OLDER)
    mmb = MorphMesh.load(vfs.find("MyMesh.MMB"))
