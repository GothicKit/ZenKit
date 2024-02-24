# Cutscene Libraries

*Cutscene libraries*, also called *"message databases"*, contain voice lines and a reference to an associated
audio recording. These files are used in conjunction with [scripts](daedalus-script.md) to facilitate PC to NPC
conversations in-game. *Cutscene libraries* are found within the `_work/data/scripts/content/cutscene/` directory
of Gothic and Gothic II installations. 

=== "C"

    ```c title="Example"
    #include <zenkit-capi/CutsceneLibrary.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkCutsceneLibrary* csl = ZkCutsceneLibrary_loadPath("OU.csl");
        ZkCutsceneLibrary_del(csl);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountHost(vfs, "_work/", "/", ZkVfsOverwriteBehavior_OLDER);
        csl = ZkCutsceneLibrary_loadVfs(vfs, "OU.csl");
        ZkCutsceneLibrary_del(csl);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/CutsceneLibrary.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::CutsceneLibrary csl {};
        
        // Load from a file on disk:
        auto r = zenkit::Read::from("OU.csl");
        csl.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_host("_work/", "/", zenkit::VfsOverwriteBehavior::OLDER);

        r = vfs->find("OU.csl")->open_read();
        font.load(r.get());

        return 0;
    }
    ```

=== "C#"
    
    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var csl = new CutsceneLibrary("OU.csl");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.Mount("_work/", "/", VfsOverwriteBehavior.Older);
    csl = new CutsceneLibrary(vfs, "OU.csl");
    ```

=== "Java"
    
    ```java title="Example"
    import dev.gothickit.zenkit.csl.CutsceneLibrary;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var csl = new CutsceneLibrary("OU.csl");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mount("_work/", "/", VfsOverwriteBehavior.OLDER);
    csl = new CutsceneLibrary(vfs, "OU.csl");
    ```
