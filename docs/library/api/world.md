# Worlds

### Loading Worlds

=== "C"

    ```c title="Example"
    #include <phoenix/cffi/World.h>
    #include <phoenix/cffi/Buffer.h>

    int main(int, const char** argv) {
        PxBuffer* buf = pxBufferMmap("OLDWORLD.ZEN");
        PxWorld* world = pxWorldLoad(buf);
        pxBufferDestroy(buf);
        
        // ...

        pxWorldDestroy(world);
        return 0;
    }
    ```

    !!! info
        As with all resources, worlds can also be loaded from a [virtual file system](virtual-file-system.md)
        by passing a `PxVfs` and the file name to `pxWorldLoadFromVfs`.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/World.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::World world {};
        
        auto r = zenkit::Read::from("OLDWORLD.ZEN");
        world.load(r.get());

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
