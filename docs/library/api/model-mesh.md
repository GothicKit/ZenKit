# Model Meshes

### Loading Model Meshes

=== "C"

    ```c title="Example"
    #include <zenkit-capi/ModelMesh.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkModelMesh* mdm = ZkModelMesh_loadPath("MyMesh.MDM");
        ZkModelMesh_del(mdm);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Anims.vdf", ZkVfsOverwriteBehavior_OLDER);
        mdm = ZkModelMesh_loadVfs(vfs, "MyMesh.MDM");
        ZkModelMesh_del(mdm);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelMesh.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::ModelMesh mesh {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyMesh.MDM");
        mesh.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Anims.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyMesh.MDM")->open_read();
        mesh.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var mesh = new ModelMesh("MyMesh.MDM");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Anims.vdf", VfsOverwriteBehavior.Older);
    mesh = new ModelMesh(vfs, "MyMesh.MDM");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.mdm.ModelMesh;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var mesh = new ModelMesh("MyMesh.MDM");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Anims.vdf", VfsOverwriteBehavior.OLDER)
    mesh = new ModelMesh(vfs, "MyMesh.MDM");
    ```

=== "Python"

    ```python title="Example"
    from zenkit import Vfs, VfsOverwriteBehavior, ModelMesh

    # Load from a file on disk:
    mesh = ModelMesh.load("MyMesh.MDM")

    # ... or from a VFS:
    vfs = Vfs()
    vfs.mount_disk("Anims.vdf", clobber=VfsOverwriteBehavior.OLDER)
    mesh = ModelMesh.load(vfs.find("MyMesh.MDM"))
