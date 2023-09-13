# Model Hierarchies

### Loading Model Hierarchies

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/ModelHierarchy.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MySkeleton.MDH");
        PxModelHierarchy* mdh = pxMdhLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxMdhDestroy(mdh);
        return 0;
    }
    ```

    !!! info
        As with all resources, model hierarchies can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxMdhLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelHierarchy.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::ModelHierarchy mdh {};
        
        auto r = zenkit::Read::from("MySkeleton.MDH");
        mdh.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, model hierarchies can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::ModelHierarchy::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
