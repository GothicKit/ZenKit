# Model Hierarchies

### Loading Model Hierarchies

=== "C"

    ```c title="Example"
    #include <zenkit-capi/ModelHierarchy.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkModelHierarchy* mdh = ZkModelHierarchy_loadPath("MySkeleton.MDH");
        ZkModelHierarchy_del(mdh);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Anims.vdf", ZkVfsOverwriteBehavior_OLDER);
        mdh = ZkModelHierarchy_loadVfs(vfs, "MySkeleton.MDH");
        ZkModelHierarchy_del(mdh);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelHierarchy.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::ModelHierarchy mdh {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MySkeleton.MDH");
        mdh.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Anims.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MySkeleton.MDH")->open_read();
        mdh.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var mdh = new ModelHierarchy("MySkeleton.MDH");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Anims.vdf", VfsOverwriteBehavior.Older);
    mdh = new ModelHierarchy(vfs, "MySkeleton.MDH");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.mdh.ModelHierarchy;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var mdh = new ModelHierarchy("MySkeleton.MDH");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Anims.vdf", VfsOverwriteBehavior.OLDER)
    mdh = new ModelHierarchy(vfs, "MySkeleton.MDH");
    ```

=== "Python"

    ```python title="Example"
    from zenkit import Vfs, VfsOverwriteBehavior, ModelHierarchy

    # Load from a file on disk:
    mdh = ModelHierarchy.load("MySkeleton.MDH")

    # ... or from a VFS:
    vfs = Vfs()
    vfs.mount_disk("Anims.vdf", clobber=VfsOverwriteBehavior.OLDER)
    mdh = ModelHierarchy.load(vfs.find("MySkeleton.MDH"))
