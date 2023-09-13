# Meshes

# Loading Meshes

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Mesh.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyMesh.MSH");
        PxMesh* mesh = pxMshLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxMshDestroy(mesh);
        return 0;
    }
    ```

    !!! info
        As with all resources, meshes can also be loaded from a [virtual file system](virtual-file-system.md) by
        passing a `PxVfs` and the file name to `pxMshLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Mesh.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::Mesh mesh {};
        
        auto r = zenkit::Read::from("MyMesh.MSH");
        mesh.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, meshes can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::Mesh::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.

