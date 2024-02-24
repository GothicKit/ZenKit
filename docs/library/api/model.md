# Models

*Models* are basic containers for a [Model Hierarchy](model-hierarchy.md) and a [Model Mesh](model-mesh.md).

# Loading Models

=== "C"

    ```c title="Example"
    #include <zenkit-capi/Model.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkModel* mesh = ZkModel_loadPath("MyMesh.MDL");
        ZkModel_del(mesh);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Anims.vdf", ZkVfsOverwriteBehavior_OLDER);
        mesh = ZkModel_loadVfs(vfs, "MyMesh.MDL");
        ZkModel_del(mesh);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Model.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::Model mesh {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyMesh.MDL");
        mesh.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Anims.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyMesh.MDL")->open_read();
        mesh.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var mesh = new Model("MyMesh.MDL");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Anims.vdf", VfsOverwriteBehavior.Older);
    mesh = new Model(vfs, "MyMesh.MDL");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.mdl.Model;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var mesh = new Model("MyMesh.MDL");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Anims.vdf", VfsOverwriteBehavior.OLDER)
    mesh = new Model(vfs, "MyMesh.MDL");
    ```

