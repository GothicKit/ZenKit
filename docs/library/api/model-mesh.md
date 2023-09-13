# Model Meshes

### Loading Model Meshes

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/ModelMesh.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyMesh.MDM");
        PxModelMesh* mdm = pxMdmLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxMdmDestroy(mdm);
        return 0;
    }
    ```

    !!! info
        As with all resources, model meshes can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxMdmLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/ModelMesh.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::ModelMesh mdm {};
        
        auto r = zenkit::Read::from("MyMesh.MDM");
        mdm.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, model meshes can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::ModelMesh::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
