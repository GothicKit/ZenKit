# Multi Resolution Meshes

### Loading Multi Resolution Meshes

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/MultiResolutionMesh.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyMesh.MRM");
        PxMultiResolutionMesh* mrm = pxMrmLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxMrmDestroy(mrm);
        return 0;
    }
    ```

    !!! info
        As with all resources, multi resolution meshes can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxMrmLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/MultiResolutionMesh.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::MultiResolutionMesh mrm {};
        
        auto r = zenkit::Read::from("MyMesh.MRM");
        mrm.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, multi resolution meshes can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::MultiResolutionMesh::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
