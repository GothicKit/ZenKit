# Save Games

### Loading Save Games

=== "C"

    !!! warning
        Archives are currently not supported with the C API.

=== "C++"

    ```cpp title="Example"
    #include <zenkit/SaveGame.hh>
    #include <zenkit/Stream.hh>

    int main(int, char const** argv) {
        zenkit::SaveGame sav {};
        sav.load("/path/to/save/game/");
        
        // ...

        return 0;
    }
    ```

=== "C#"

    !!! warning
        Archives are currently not supported with the C# API since it depends on the C wrapper.

=== "Java"

    !!! warning
        Archives are currently not supported with the Java API since it depends on the C wrapper.
