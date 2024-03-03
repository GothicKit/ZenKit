# Multi Resolution Meshes

### Loading Multi Resolution Meshes

=== "C"

    ```c title="Example"
    #include <zenkit-capi/MultiResolutionMesh.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkMultiResolutionMesh* mrm = ZkMultiResolutionMesh_loadPath("MyMesh.MRM");
        ZkMultiResolutionMesh_del(mrm);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Meshes.vdf", ZkVfsOverwriteBehavior_OLDER);
        mrm = ZkMultiResolutionMesh_loadVfs(vfs, "MyMesh.MRM");
        ZkMultiResolutionMesh_del(mrm);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/MultiResolutionMesh.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::MultiResolutionMesh mrm {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyMesh.MRM");
        mrm.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Meshes.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyMesh.MRM")->open_read();
        mrm.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var mrm = new MultiResolutionMesh("MyMesh.MRM");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Meshes.vdf", VfsOverwriteBehavior.Older);
    mrm = new MultiResolutionMesh(vfs, "MyMesh.MRM");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.mrm.MultiResolutionMesh;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var mrm = new MultiResolutionMesh("MyMesh.MRM");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Meshes.vdf", VfsOverwriteBehavior.OLDER)
    mrm = new MultiResolutionMesh(vfs, "MyMesh.MRM");
    ```

=== "Python"

    ```python title="Example"
    from zenkit import Vfs, VfsOverwriteBehavior, MultiResolutionMesh

    # Load from a file on disk:
    mrm = MultiResolutionMesh.load("MyMesh.MRM")

    # ... or from a VFS:
    vfs = Vfs()
    vfs.mount_disk("Meshes.vdf", clobber=VfsOverwriteBehavior.OLDER)
    mrm = MultiResolutionMesh.load(vfs.find("MyMesh.MRM"))
