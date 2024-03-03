# Save Games

The save-game APIs exposed by ZenKit allow you to load, modify and save Gothic saves. As with all APIs, both Gothic 1
and Gothic 2 save-games are supported by this API. For more detailed information about the save-game format, consult
the [ZenGin Engine Documentation](../../engine/formats/savegame.md).

### Loading save games

Loading a save game is relatively straightforward and example implementations can be found below. It should be noted
however, that worlds loaded from save games do not contain any mesh information. They only contain the VObject-tree,
the way-net and some additional metadata information related to the world state (such as the state of the sky controller).
This means, that save games are loaded *in addition* to the original world and essentially just replace the way-net and
VOb-tree of the base world.

=== "C"

    ```c title="Example"
    #include <zenkit-capi/SaveGame.h>
    #include <zenkit-capi/Vfs.h>
    #include <zenkit-capi/World.h>

    #include <stdio.h>

    int main(int, const char** argv) {
        // Initialize the save-game controller for use with Gothic 1
        ZkSaveGame* sav = ZkSaveGame_new(ZkGameVersion_GOTHIC1);

        // Try to load the save
        if (!ZkSaveGame_load(sav, "Gothic/Saves/savegame0/")) {
            fprintf(stderr, "Failed to load the save-game!\n");
            return -1;
        }

        // Load the world the player was in when creating the save ...
        ZkWorld* world = ZkSaveGame_loadCurrentWorld(sav);
        if (world == NULL) {
            fprintf(stderr, "No default world found!\n");
            return -1;
        }

        // Since you own the world, don't forget to delete it when its no longer needed
        ZkWorld_del(world);

        // ... or load an arbitrary world from the save-game
        world = ZkSaveGame_loadCurrentWorld(sav, "DRAGONISLAND.ZEN");
        if (world == NULL) {
            fprintf(stderr, "DRAGONISLAND.ZEN not found!\n");
            return -1;
        }

        // ...

        ZkWorld_del(world);
        ZkSaveGame_del(sav);
        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/SaveGame.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/World.hh>

    #include <iostream>

    int main(int, char const** argv) {
        // Initialize the save-game controller for use with Gothic 1
        zenkit::SaveGame sav {zenkit::GameVersion::GOTHIC1};

        // Try to load the save (throws if it fails)
        sav.load("Gothic/Saves/savegame0/");

        // Load the world the player was in when creating the save ...
        std::shared_ptr<World> world = sav.load_world();
        if (world == nullptr) {
            std::cerr << "No default world found!\n";
            return -1;
        }

        // ... or load an arbitrary world from the save-game
        world = sav.load_world("DRAGONISLAND.ZEN");
        if (world == nullptr) {
            std::cerr << "DRAGONISLAND.ZEN not found!\n";
            return -1;
        }
        
        // ...

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Initialize the save-game controller for use with Gothic 1
    var sav = new SaveGame(GameVersion.Gothic1);
    
    // Try to load the save 
    if (!sav.Load("Gothic/Saves/savegame0/")) {
        Console.Error.WriteLine("Failed to load the save-game!");
        return;
    }

    // Load the world the player was in when creating the save ...
    World world = sav.LoadWorld();
    if (world != null) {
        Console.Error.WriteLine("Failed to load the save-game!");
        return;
    }

    // ... or load an arbitrary world from the save-game
    world = sav.LoadWorld("DRAGONISLAND.ZEN");
    if (world == null) {
        Console.Error.WriteLine("DRAGONISLAND.ZEN not found!");
        return;
    }

    // ...
    ```

=== "Java"

    !!! warning
        Not yet available.

=== "Python"

    !!! warning
        Not yet available.

### Creating or modifying save games

You can easily create a new save-game or modify an exising one using the save game APIs. To do so you need to have
access to a [world](world.md) from somewhere (could also be another save game).

!!! tip
    In addition to simply saving the world as shown below, you can also alter other save state such as script state,
    in-game information state, log entries and the save-game's name, thumbnail and other state through instance
    attributes of the `SaveGame`. Simply set these attributes to the values you want before calling `Save` to set them
    in the newly created save-game.

=== "C"

    ```c title="Example"
    #include <zenkit-capi/SaveGame.h>
    #include <zenkit-capi/Vfs.h>
    #include <zenkit-capi/World.h>

    #include <stdio.h>

    int main(int, const char** argv) {
        // Initialize the save-game controller for use with Gothic 1
        ZkSaveGame* sav = ZkSaveGame_new(ZkGameVersion_GOTHIC1);

        // Try to load an exising save-game (can be skipped if you already have access to a `ZkWorld` from elsewhere)
        if (!ZkSaveGame_load(sav, "Gothic/Saves/savegame0/")) {
            fprintf(stderr, "Failed to load the save-game!\n");
            return -1;
        }

        // Get a world from somewhere (does not necessarily have to be from an existing save-game)
        ZkWorld* world = ZkSaveGame_loadCurrentWorld(sav);

        // Create a new save-game which contains the given world's VOb-tree and way-net under the name "WORLD". If the
        // given path already exists, all data within is deleted recursively, otherwise it and all its parent
        // directories are created automatically.
        // Since in this case, we're altering an existing save-game, all other worlds already present in the previous 
        // save are copied into the new location and the metadata (unless modified) is also copied.
        ZkSaveGame_save(sav, "Gothic/Saves/savegame1", world, "WORLD");

        ZkWorld_del(world);
        ZkSaveGame_del(sav);
        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/SaveGame.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/World.hh>

    #include <iostream>

    int main(int, char const** argv) {
        // Initialize the save-game controller for use with Gothic 1
        zenkit::SaveGame sav {zenkit::GameVersion::GOTHIC1};

        // Try to load an exising save-game (can be skipped if you already have access to a world from elsewhere)
        sav.load("Gothic/Saves/savegame0/");

        // Get a world from somewhere (does not necessarily have to be from an existing save-game)
        std::shared_ptr<World> world = sav.load_world();

        // Create a new save-game which contains the given world's VOb-tree and way-net under the name "WORLD". If the
        // given path already exists, all data within is deleted recursively, otherwise it and all its parent
        // directories are created automatically.
        // Since in this case, we're altering an existing save-game, all other worlds already present in the previous 
        // save are copied into the new location and the metadata (unless modified) is also copied.
        sav.save("Gothic/Saves/savegame1", *world, "WORLD");
        
        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Initialize the save-game controller for use with Gothic 1
    var sav = new SaveGame(GameVersion.Gothic1);
    
    // Try to load an exising save-game (can be skipped if you already have access to a world from elsewhere)
    if (!sav.Load("Gothic/Saves/savegame0/")) {
        Console.Error.WriteLine("Failed to load the save-game!");
        return;
    }

    // Get a world from somewhere (does not necessarily have to be from an existing save-game)
    World world = sav.LoadWorld();

    // Create a new save-game which contains the given world's VOb-tree and way-net under the name "WORLD". If the
    // given path already exists, all data within is deleted recursively, otherwise it and all its parent
    // directories are created automatically.
    // Since in this case, we're altering an existing save-game, all other worlds already present in the previous 
    // save are copied into the new location and the metadata (unless modified) is also copied.
    sav.Save("Gothic/Saves/savegame1", world, "WORLD");

    // ...
    ```

=== "Java"

    !!! warning
        Not yet available.

=== "Python"

    !!! warning
        Not yet available.
