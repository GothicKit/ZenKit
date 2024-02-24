# Save Games

### Loading Save Games

=== "C"

    ```c title="Example"
    #include <zenkit-capi/SaveGame.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkSaveGame* sav = ZkSaveGame_new(ZkGameVersion_GOTHIC1);
        ZkSaveGame_load(sav, "Gothic/Saves/savegame0/");

        // ...

        ZkSaveGame_del(sav);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/SaveGame.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::SaveGame sav {zenkit::GameVersion::GOTHIC1};
        sav.load("Gothic/Saves/savegame0/");
        
        // ...

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    var sav = new SaveGame(GameVersion.Gothic1);
    sav.Load("Gothic/Saves/savegame0/");

    // ...

    ```

=== "Java"

    !!! warning
        Not yet available.
