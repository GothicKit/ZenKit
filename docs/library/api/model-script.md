# Model Scripts

### Loading Model Scripts

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/ModelScript.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyScript.MDS");
        PxModelScript* mds = pxMdsLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxMdsDestroy(mds);
        return 0;
    }
    ```

    !!! info
        As with all resources, model scripts can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxMdsLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelScript.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::ModelScript mdm {};
        
        auto r = zenkit::Read::from("MyScript.MDS");
        mds.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, model scripts can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::ModelScript::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
