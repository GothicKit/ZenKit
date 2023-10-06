# Migration Guide v1 to v2

With the update to version 2.0, the library previously known as *"phoenix"* has bee re-branded to *"ZenKit"* to avoid
confusion with [PhoenixTales' Game](https://phoenixthegame.com/main) of the same name. In this update, the library's
API has changed significantly but a mostly backwards-compatible API has been kept intact.

## Required Migrations

!!! danger
    These changes are required for your application to build with ZenKit.

After updating *ZenKit* to v2, the first thing you have to do is update your CMake configuration. The `phoenix` target
has been renamed to `zenkit`. Thus, you must now link against `zenkit` instead. The CMake variables for configuring
*ZenKit* have also been renamed and a new one has been added:

* `PHOENIX_BUILD_EXAMPLES` has been renamed to `ZK_BUILD_EXAMPLES`
* `PHOENIX_BUILD_TESTS` has been renamed to `ZK_BUILD_TESTS`
* `PHOENIX_BUILD_SHARED` has been renamed to `ZK_BUILD_SHARED`
* `PHOENIX_INSTALL` has been renamed to `ZK_ENABLE_INSTALL`
* `PHOENIX_DISABLE_SANITIZERS` has been removed and is replaced by `ZK_ENABLE_ASAN` which is `ON` by default.
* `ZK_ENABLE_DEPRECATION` has been added and is `ON` by default. This is the recommended setting.

After updating you configuration accordingly, assuming you have not used any already deprecated APIs from v1, your code
should now build again. However, you will get a lot of deprecation warnings if you keep `ZK_ENABLE_DEPRECATION=ON`.

### Replacing `phoenix::vdf_file`

The `phoenix::vdf_file` has been deprecated since v1.2 in favour of `phoenix::Vfs` and is removed in v2. Here's an
example which shows how to migrate to the new implementation.

=== "<= v1.1"

    ```c++
    phoenix::vdf_file anims = phoenix::vdf_file::open("Anims.vdf");
    phoenix::vdf_file worlds = phoenix::vdf_file::open("Worlds.vdf");

    phoenix::vdf_file root {""};
    root.merge(worlds, false);
    root.merge(anims, false);

    phoenix::vdf_entry* world = root.find_entry("NewWorld.zen");
    if (world == nullptr) {
        throw std::runtime_error {"NewWorld.zen not found!"};
    }

    phoenix::buffer buf = world->open();

    // use `buf`
    ```

=== ">= v1.2"

    ```c++
    phoenix::Vfs vfs {};
    vfs.mount_disk("Anims.vdf", phoenix::VfsOverwriteBehavior::OLDER);
    vfs.mount_disk("Worlds.vdf", phoenix::VfsOverwriteBehavior::OLDER);

    phoenix::VfsNode* world = vfs.find("NewWorld.zen");
    if (world == nullptr) {
        throw std::runtime_error {"NewWorld.zen not found!"};
    }

    phoenix::buffer buf = world->open();

    // use `buf`
    ```

### Replacing other APIs

The following APIs have been deprecated v1 and removed in v2:

* `way_net::waypoint(std::string const&)` - Directly access the `waypoints` member instead
* `archive_reader::read_raw_bytes()` - Replaced by `archive_reader::read_raw_bytes(size_t)`
* `model_script::parse_binary(buffer)` - Replaced by `model_script::parse(buffer)`
* `vobs::camera_lock_mode` - Renamed to `sprite_alignment`
* `vob.camera_alignment` - Renamed to `vob.sprite_camera_facing_mode`

## Performance Critical Migrations

!!! warning
    These changes are heavily encouraged since not migrating will have a sizable impact on the performance of *ZenKit*.

While your app will compile after applying the required migrations, you will notice a performance dip, especially in
debug builds. This is because the central `phoenix::buffer`-API has been deprecated in favour of `zenkit::Read` which
is faster and simpler when used correctly.

To remedy this, all you need to do is use the `.load(Read*)` API instead of the `::parse(buffer)` API when loading in
any *ZenKit* asset. Here's an example which shows the differences between both versions when loading animations.

=== "< v2.0"

    ```cpp title="Example"
    #include <phoenix/Animation.hh>

    int main(int, char const** argv) {
        phoenix::buffer buf = phoenix::buffer::mmap("MyAnimation.MAN");
        phoenix::animation ani = phoenix::animation::parse(buf);
        
        // ...

        return 0;
    }
    ```

=== ">= v2.0"

    ```cpp title="Example"
    #include <phoenix/animation.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        phoenix::animation ani {};
        
        auto r = zenkit::Read::from("MyAnimation.MAN");
        ani.load(r.get());

        // ...

        return 0;
    }
    ```

`zenkit::Read::from` has many different overloads from reading from memory buffers to reading from C++'s `istream`.
Just choose the one you need. When reading from raw files, the overload taking `std::filesystem::path` is recommended
since it performs memory-mapped I/O in the background.

When loading a file from a `zenkit::Vfs`, you can simply use the `VfsNode.open_read` API instead of the old
`VfsNode.open` API. Most of this should be self-explanatory.

## Recommended Migrations

!!! note
    These changes are recommended but not required. They concern changing class, struct and enum names to fit the new
    APIs. It is recommended to apply these to your code gradually.


After these migrations, one thing remains: switching to the redesigned API. Generally this just means changing your
includes and using the new names for classes, structs and enums. All of these have been renamed from the old
`snake_case` naming convention to the more easily understandable `PascalCase` convention and some of their names have
been polished up a bit. This is what you should do:

1. Change your includes: generally, just change `#include <phoenix/$include.hh>` to `#include <zenkit/$include.hh>` while
   changing `$include` from `snake_case` to `PascalCase`. Some files have been renamed completely:
    * `animation.hh` is now `ModelAnimation.hh`
    * `math.hh` is now `Boxes.hh`
    * `messages.hh` is now `CutsceneLibrary.hh`
    * `script.hh` is now `DaedalusScript.hh`
    * `vm.hh` is now `DaedalusVm.hh`
    * `phoenix.hh` has been split into `Error.hh`, `Date.hh`, `Logger.hh` and `Misc.hh`
    * `proto_mesh.hh` is now `MultiResolutionMesh.hh`
    * `vobs/vob.hh` is now `vobs/VirtualObject.hh`
    * `vobs/mob.hh` is now `vobs/MovableObject.hh`
    * `ext/daedalus_class.hh` is now `addon/daedalus.hh`
    * `ext/dds_convert.hh` is now `addon/texcvt.hh`
2. Changes usages of the `phoenix` namespace to the `zenkit` namespace (i.e. `phoenix::Vfs` becomes `zenkit::Vfs`.
3. Change the names of all classes, structs and enums imported from *ZenKit* from `snake_case` to `PascalCase` except
   for in the following cases:
    * `animation` is now `ModelAnimation`
    * `bounding_box` is now `AxisAlignedBoundingBox`
    * `obb` is now `OrientedBoundingBox`
    * `messages` is now `CutsceneLibrary`
    * `message_block` is now `CutsceneBlock`
    * `atomic_message` is now `CutsceneMessage`
    * `script` is now `DaedalusScript` (other related classes and structs have been prefixed with `Daedalus` too)
    * `vm` is now `DaedalusVm` (other related classes and structs have been prefixed with `Daedalus` too)
4. Change all enum member accesses from their `snake_case` names to the new `SCREAMING_SNAKE_CASE` names.

The deprecation warnings from your compiler should tell you these changes too. Alternatively, you can look into the old
header files in `include/phoenix/` to see the type aliases which have been put in place.

Generally these changes may be applied gradually but note that the old API will be removed in v3 at the latest, so
switching to the new names is highly recommended.
