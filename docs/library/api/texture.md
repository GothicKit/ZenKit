# Textures

### Loading Textures

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/Texture.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("MyTexture.TEX");
        PxTexture* tex = pxTexLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxTexDestroy(tex);
        return 0;
    }
    ```

    !!! info
        As with all resources, textures can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxTexLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/Texture.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::Texture tex {};
        
        auto r = zenkit::Read::from("MyTexture.TEX");
        tex.load(r.get());

        // ...

        return 0;
    }
    ```

    !!! info
        As with all resources, texture can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing the input obtained from `zenkit::VfsNode::open_read` into `zenkit::Texture::load`.

=== "C#"

    !!! note
        No documentation available.

=== "Java"

    !!! note
        No documentation available.
