# Model Scripts

### Loading Model Scripts

=== "C"

    ```c title="Example"
    #include <zenkit-capi/ModelScript.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkModelScript* mds = ZkModelScript_loadPath("MyScript.MDS");
        ZkModelScript_del(mds);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Anims.vdf", ZkVfsOverwriteBehavior_OLDER);
        mds = ZkModelScript_loadVfs(vfs, "MyScript.MDS");
        ZkModelScript_del(mds);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelScript.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::ModelScript mds {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyScript.MDS");
        mds.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Anims.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyScript.MDS")->open_read();
        mds.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var mds = new ModelScript("MyScript.MDS");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Anims.vdf", VfsOverwriteBehavior.Older);
    mds = new ModelScript(vfs, "MyScript.MDS");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.mds.ModelScript;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var mds = new ModelScript("MyScript.MDS");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Anims.vdf", VfsOverwriteBehavior.OLDER)
    mds = new ModelScript(vfs, "MyScript.MDS");
    ```
