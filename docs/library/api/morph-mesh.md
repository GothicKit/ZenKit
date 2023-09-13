# Morph Meshes

### Loading Morph Meshes

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/MorphMesh.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyMesh.MMB");
        PxMorphMesh* mmb = pxMmbLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxMmbDestroy(mmb);
        return 0;
    }
    ```

    !!! info
        As with all resources, morph meshes can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxMmbLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/MorphMesh.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::MorphMesh mmb {};
        
        auto r = zenkit::Read::from("MyMesh.MMB");
        mmb.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, morph meshes can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::MorphMesh::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
