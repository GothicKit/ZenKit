# Meshes

# Loading Meshes

=== "C"

    ```c title="Example"
    #include <zenkit-capi/Mesh.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkMesh* mesh = ZkMesh_loadPath("MyMesh.MSH");
        ZkMesh_del(mesh);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Meshes.vdf", ZkVfsOverwriteBehavior_OLDER);
        mesh = ZkMesh_loadVfs(vfs, "MyMesh.MSH");
        ZkMesh_del(mesh);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Mesh.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::Mesh mesh {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyMesh.MSH");
        mesh.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Meshes.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyMesh.MSH")->open_read();
        mesh.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var mesh = new Mesh("MyMesh.MSH");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Meshes.vdf", VfsOverwriteBehavior.Older);
    mesh = new Mesh(vfs, "MyMesh.MSH");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.msh.Mesh;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var mesh = new Mesh("MyMesh.MSH");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Meshes.vdf", VfsOverwriteBehavior.OLDER)
    mesh = new Mesh(vfs, "MyMesh.MSH");
    ```
