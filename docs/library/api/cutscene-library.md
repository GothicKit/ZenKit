# Cutscene Libraries

*Cutscene libraries*, also called *"message databases"*, contain voice lines and a reference to an associated
audio recording. These files are used in conjunction with [scripts](daedalus-script.md) to facilitate PC to NPC
conversations in-game. *Cutscene libraries* are found within the `_work/data/scripts/content/cutscene/` directory
of Gothic and Gothic II installations. 

# Loading Cutscene Libraries

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Cutscene.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("OU.csl");
        PxCutsceneLib* csl = pxCslLoad(buf);
        pxBufferDestroy(csl);
        
        // ...

        pxCslDestroy(man);
        return 0;
    }
    ```

    !!! info
        As with all resources, cutscene libraries can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxCslLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/CutsceneLibrary.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::CutsceneLibrary csl {};
        
        auto r = zenkit::Read::from("OU.csl");
        csl.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, cutscene libraries can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::CutsceneLibrary::load`.

=== "C#"
    
    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
