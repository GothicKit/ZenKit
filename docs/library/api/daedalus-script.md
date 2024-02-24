# Daedalus Scripts

*Daedalus* is the domain-specific compiled scripting language used by the *ZenGin*. Currently, *ZenKit* supports
loading and executing compiled scripts using the `zenkit::DaedalusScript` API.

# Loading Daedalus Scripts

=== "C"

    ```c title="Example"
    #include <zenkit-capi/DaedalusScript.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkDaedalusScript* script = ZkDaedalusScript_loadPath("MENU.DAT");
        ZkDaedalusScript_del(script);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountHost(vfs, "_work/", "/", ZkVfsOverwriteBehavior_OLDER);
        script = ZkDaedalusScript_loadVfs(vfs, "MENU.DAT");
        ZkDaedalusScript_del(csl);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/DaedalusScript.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::DaedalusScript script {};
        
        auto r = zenkit::Read::from("MENU.DAT");
        script.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_host("_work/", "/", zenkit::VfsOverwriteBehavior::OLDER);

        r = vfs->find("MENU.DAT")->open_read();
        script.load(r.get());

        return 0;
    }
    ```

=== "C#"
    
    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var script = new DaedalusScript("MENU.DAT");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.Mount("_work/", "/", VfsOverwriteBehavior.Older);
    script = new DaedalusScript(vfs, "MENU.DAT");
    ```

=== "Java"
    
    ```java
    import dev.gothickit.zenkit.daedalus.DaedalusScript;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var script = new DaedalusScript("MENU.DAT");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mount("_work/", "/", VfsOverwriteBehavior.Older);
    script = new DaedalusScript(vfs, "MENU.DAT");
    ```
