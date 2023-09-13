# Models

*Models* are basic containers for a [Model Hierarchy](model-hierarchy.md) and a [Model Mesh](model-mesh.md).

# Loading Models

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Model.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyMesh.MDL");
        PxModel* model = pxMdlLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxMdlDestroy(model);
        return 0;
    }
    ```

    !!! info
        As with all resources, models can also be loaded from a [virtual file system](virtual-file-system.md) by
        passing a `PxVfs` and the file name to `pxMdlLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Model.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::Model model {};
        
        auto r = zenkit::Read::from("MyModel.MDL");
        model.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, models can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::Model::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.

