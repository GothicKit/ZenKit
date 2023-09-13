# Daedalus Scripts

*Daedalus* is the domain-specific compiled scripting language used by the *ZenGin*. Currently, *ZenKit* supports
loading and executing compiled scripts using the `zenkit::DaedalusScript` API.

# Loading Daedalus Scripts

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/DaedalusScript.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MENU.DAT");
        PxDaedalusScript* script = pxScriptLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxScriptDestroy(script);
        return 0;
    }
    ```

    !!! info
        As with all resources, scripts can also be loaded from a [virtual file system](virtual-file-system.md) by
        passing a `PxVfs` and the file name to `pxScriptLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/DaedalusScript.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::DaedalusScript script {};
        
        auto r = zenkit::Read::from("MENU.DAT");
        script.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, scripts can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::DaedalusScript::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
