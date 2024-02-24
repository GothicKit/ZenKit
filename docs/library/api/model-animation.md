# Model Animations

*Model Animations* form part of the animations system of the *ZenGin* they contain only animation samples, i.e. the
position and orientation of each bone of the skeleton they're applied to. While there is space for additional data
within animation files, it is mostly empty.

## Overview

The most important part of an animation are its samples. They are stored in `ModelAnimation::samples` after it has been
parsed. Animation files themselves don't contain information about when to run animation or any other effects which
should be applied during it. Those parts of the animation system are defined in [Model Script](model-script.md) files
which should be loaded before animations. The `ModelAnimation::events` field will always be empty for that reason[^1].


### Loading Animations

=== "C"

    ```c title="Example"
    #include <zenkit-capi/ModelAnimation.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkModelAnimation* man = ZkModelAnimation_loadPath("MyAnimation.MAN");
        ZkModelAnimation_del(man);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountDiskHost(vfs, "Anims.vdf", ZkVfsOverwriteBehavior_OLDER);
        man = ZkModelAnimation_loadVfs(vfs, "MyAnimation.MAN");
        ZkModelAnimation_del(man);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelAnimation.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::ModelAnimation man {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("MyAnimation.MAN");
        man.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_disk("Anims.vdf", zenkit::VfsOverwriteBehavior::OLDER)

        r = vfs->find("MyAnimation.MAN")->open_read();
        man.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var man = new ModelAnimation("MyAnimation.MAN");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.MountDisk("Anims.vdf", VfsOverwriteBehavior.Older);
    man = new ModelAnimation(vfs, "MyAnimation.MAN");
    ```

=== "Java"

    ```java title="Example"
    import dev.gothickit.zenkit.ani.ModelAnimation;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var man = new ModelAnimation("MyAnimation.MAN");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mountDisk("Anims.vdf", VfsOverwriteBehavior.OLDER)
    man = new ModelAnimation(vfs, "MyAnimation.MAN");
    ```


[^1]: This assertion is made after examining many animations from both *Gothic* and *Gothic II*
      and finding no events being stored.
