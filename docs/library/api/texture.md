# Textures

### Loading Textures

=== "C"

    ```c title="Example"
    #include <zenkit-capi/Texture.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkTexture* tex = ZkTexture_loadPath("MyTexture.TEX");
        ZkTexture_del(tex);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Textures.vdf", ZkVfsOverwriteBehavior_OLDER);
        tex = ZkTexture_loadVfs(vfs, "MyTexture.TEX");
        ZkTexture_del(tex);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Texture.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::Texture tex {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyTexture.TEX");
        tex.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Textures.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyTexture.TEX")->open_read();
        tex.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var tex = new Texture("MyTexture.TEX");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Textures.vdf", VfsOverwriteBehavior.Older);
    tex = new Texture(vfs, "MyTexture.TEX");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.tex.Texture;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var tex = new Texture("MyTexture.TEX");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Textures.vdf", VfsOverwriteBehavior.OLDER)
    tex = new Texture(vfs, "MyTexture.TEX");
    ```
