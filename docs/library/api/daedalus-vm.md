# Daedalus VMs

## Loading Daedalus VMs


=== "C"

    ```c title="Example"
    #include <zenkit-capi/DaedalusScript.h>
    #include <zenkit-capi/Vfs.h>

    int main(int, const char** argv) {
        // Load from a file on disk:
        ZkDaedalusScript* script = ZkDaedalusScript_loadPath("MENU.DAT");
        ZkDaedalusScript_del(script);

        // ... or from a VFS:
        ZkVfs* vfs = ZkVfs_new();
        ZkVfs_mountHost(vfs, "_work/", "/", ZkVfsOverwriteBehavior_OLDER);
        script = ZkDaedalusScript_loadVfs(vfs, "MENU.DAT");
        ZkDaedalusScript_del(csl);
        ZkVfs_del(vfs);

        return 0;
    }
    ```

=== "C++"

    ```cpp title="Example"
    #include <zenkit/DaedalusScript.hh>
    #include <zenkit/Stream.hh>
    #include <zenkit/Vfs.hh>

    int main(int, char const** argv) {
        zenkit::DaedalusScript script {};
        
        auto r = zenkit::Read::from("MENU.DAT");
        script.load(r.get());

        // ... or from a VFS
        zenkit::Vfs vfs;
        vfs.mount_host("_work/", "/", zenkit::VfsOverwriteBehavior::OLDER);

        r = vfs->find("MENU.DAT")->open_read();
        script.load(r.get());

        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;

    // Load from a file on disk:
    var script = new DaedalusScript("MENU.DAT");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.Mount("_work/", "/", VfsOverwriteBehavior.Older);
    script = new DaedalusScript(vfs, "MENU.DAT");
    ```

=== "Java"

    ```java
    import dev.gothickit.zenkit.daedalus.DaedalusScript;
    import dev.gothickit.zenkit.vfs.Vfs;
    import dev.gothickit.zenkit.vfs.VfsOverwriteBehavior;

    // Load from a file on disk:
    var script = new DaedalusScript("MENU.DAT");

    // ... or from a VFS:
    var vfs = new Vfs();
    vfs.mount("_work/", "/", VfsOverwriteBehavior.Older);
    script = new DaedalusScript(vfs, "MENU.DAT");
    ```

=== "Python"

    ```python title="Example"
    from zenkit import Vfs, VfsOverwriteBehavior, DaedalusScript

    # Load from a file on disk:
    script = DaedalusScript.load("MENU.DAT")

    # ... or from a VFS:
    vfs = Vfs()
    vfs.mount_path("_work/", "/", clobber=VfsOverwriteBehavior.OLDER)
    script = DaedalusScript.load(vfs.find("MENU.DAT"))
    ```


## Using the Daedalus VM

=== "C++"

    ```c++ title="Example"
    #include <zenkit/DaedalusScript.hh>
    #include <zenkit/DaedalusVm.hh>
    #include <zenkit/Logger.hh>
    #include <zenkit/addon/daedalus.hh>

    #include <iostream>

    int main(int argc, char** argv) {
    if (argc != 2) {
    std::cerr << "Please provide a path to GOTHIC.DAT.";
    return -1;
    }

        zenkit::Logger::set_default(zenkit::LogLevel::DEBUG);

        // Load the script file
        zenkit::DaedalusScript script;
        auto rd = zenkit::Read::from(argv[1]);
        script.load(rd.get());

        // Create a VM from the script
        zenkit::DaedalusVm vm {std::move(script)};

        // Register default script classes. Their implementation can be found in `zenkit/addon/daedalus.hh`. You are able
        // to define your own classes through DaedalusScript::register_member if your use-case requires it.
        // Generally, registering class definitions is required for scripts to work correctly.
        zenkit::register_all_script_classes(vm);

        // Register a catch-all callback for all calls to un-registered external functions. ZenKit will handle all required
        // internal VM state as required so as to not corrupt the stack.
        //
        // If you need to restore custom VM state when external calls fail (such as setting a global variable or returning
        // a custom value), you can use the unmanaged `DaedalusVm::register_default_external_custom` which leaves VM state
        // fixing up to you. This not not recommended, however, and you should only use it when its absolutely required.
        // Prefer actually implementing externals to messing with internal VM state.
        vm.register_default_external([](zenkit::DaedalusSymbol const& sym) { //
            std::cout << "VM: No external for " << sym.name() << "\n";
        });

        // Register implementations for external functions. Parameters and return values are checked to match their
        // definitions in the script at runtime and any mismatches between definition and implementation will cause runtime
        // exceptions to be thrown.
        vm.register_external("INTTOSTRING", [](int i) { return std::to_string(i); });
        vm.register_external("CONCATSTRINGS", [](std::string_view a, std::string_view b) { //
            return std::string {a} + std::string {b};
        });

        vm.register_external("AI_PRINTSCREEN", [](std::string_view msg, int, int, std::string_view font, int) {
            std::cout << "AI: print \"" << msg << "\" with font \"" << font << "\"\n";
            return true;
        });

        // Initialize some instances. Essentially, each Daedalus instance needs to be initialized in C++ which causes the
        // Daedalus code defining that instance to be executed. This needs to be done for every Daedalus instance BEFORE
        // it can be used in a script. The fields of the Daedalus instances are synced, so any change in C++ will be
        // reflected in Daedalus and vice-versa.
        auto xardas = vm.init_instance<zenkit::INpc>("NONE_100_XARDAS");
        auto hero = vm.init_instance<zenkit::INpc>("PC_HERO");
        
        // You can also move the actual instance initialization call to a later point. This might be useful when you need
        // you want to manually initialze the instance in C++. To do it, first allocate an instance, then initialize it,
        // when needed, like this:
        auto gold = vm.allocate_instance<zenkit::IItem>("ITMI_GOLD");
        vm.init_instance(gold, "ITMI_GOLD");

        // External callbacks may also capture any local state, if needed.
        vm.register_external("NPC_ISPLAYER", [&hero](std::shared_ptr<zenkit::INpc> npc) { //
            return npc->id == hero->id;
        });

        // Some calls to VM function require a certain global context variable to be set. This applies mainly to NPC
        // routine functions, state update functions (like item equip callbacks) and mision state query functions. The
        // available global context variables are:
        //
        //  * `global_self` -> var C_NPC self
        //  * `global_other` -> var C_NPC other
        //  * `global_victim` -> var C_NPC victim
        //  * `global_hero` -> var C_NPC hero
        //  * `global_item` -> var C_ITEM item
        vm.global_item()->set_instance(gold);

        // Call a function defined the script. The first parameter can either be the function's name or a the DaedalusSymbol
        // for the function. The following parameters are passed to the Daedalus function as arguments. If set, the explicit
        // template parameter indicates the return value. If it is not set, the return value (if any) is ignored.
        //
        // In this case, the Daedalus function definition is:
        //
        //     func int B_GIVEINVITEMS(var C_NPC giver, var C_NPC taker, var int itemInstance, var int amount)
        auto ret = vm.call_function<int>("B_GIVEINVITEMS", xardas, hero, static_cast<int>(gold->symbol_index()), 2);

        std::cout << "\nCalling B_GIVEINVITEMS(NONE_100_XARDAS, PC_HERO, " << gold->symbol_index()
                  << ", 1) resulted in return of " << ret << "\n";
        return 0;
    }
    ```

=== "C#"

    ```c# title="Example"
    using ZenKit;
    using ZenKit.Daedalus;

    var argv = Environment.GetCommandLineArgs();
    if (argv.Length != 2)
    {
        Console.Error.WriteLine("Please provide a path to GOTHIC.DAT.");
        return -1;
    }

    Logger.SetDefault(LogLevel.Debug);

    // Load the script file into a VM
    var vm = new DaedalusVm(argv[1]);

    // Register a catch-all callback for all calls to un-registered external functions. ZenKit will handle all required
    // internal VM state as required so as to not corrupt the stack.
    vm.RegisterExternalDefault((vm, sym) =>
    {
        //
        Console.Error.WriteLine("VM: No external for " + sym.Name);
    });

    // Register implementations for external functions. Parameters and return values are checked to match their
    // definitions in the script at runtime and any mismatches between definition and implementation will cause runtime
    // exceptions to be thrown.
    vm.RegisterExternal("INTTOSTRING", (int i) => i.ToString());
    vm.RegisterExternal("CONCATSTRINGS", (string a, string b) => a + b);

    vm.RegisterExternal("AI_PRINTSCREEN", (string msg, int _, int _, string font, int _) =>
    {
        Console.WriteLine("AI: print \"" + msg + "\" with font \"" + font + "\"");
        return true;
    });

    // Initialize some instances. Essentially, each Daedalus instance needs to be initialized in C# which causes the
    // Daedalus code defining that instance to be executed. This needs to be done for every Daedalus instance BEFORE
    // it can be used in a script. The fields of the Daedalus instances are synced, so any change in C# will be
    // reflected in Daedalus and vice-versa.
    var xardas = vm.InitInstance<NpcInstance>("NONE_100_XARDAS");
    var hero = vm.InitInstance<NpcInstance>("PC_HERO");

    // You can also move the actual instance initialization call to a later point. This might be useful when you need
    // you want to manually initialize the instance in C#. To do it, first allocate an instance, then initialize it,
    // when needed, like this:
    var gold = vm.AllocInstance<ItemInstance>("ITMI_GOLD");
    vm.InitInstance(gold);

    // External callbacks may also capture any local state, if needed.
    vm.RegisterExternal("NPC_ISPLAYER", (NpcInstance npc) => npc.Id == hero.Id);

    // Some calls to VM function require a certain global context variable to be set. This applies mainly to NPC
    // routine functions, state update functions (like item equip callbacks) and mission state query functions. The
    // available global context variables are:
    //
    //  * `GlobalSelf` -> var C_NPC self
    //  * `GlobalOther` -> var C_NPC other
    //  * `GlobalVictim` -> var C_NPC victim
    //  * `GlobalHero` -> var C_NPC hero
    //  * `GlobalItem` -> var C_ITEM item
    vm.GlobalItem = gold;

    // Call a function defined the script. The first parameter can either be the function's name or the DaedalusSymbol
    // for the function. The following parameters are passed to the Daedalus function as arguments.
    //
    // In this case, the Daedalus function definition is:
    //
    //     func int B_GIVEINVITEMS(var C_NPC giver, var C_NPC taker, var int itemInstance, var int amount)
    //
    // Notice that you have to specify argument and return types manually. The first generic is the return type (which can
    // be omitted if the function returns void) and all other ones correspond to the function arguments.
    var ret = vm.Call<int, NpcInstance, NpcInstance, int, int>("B_GIVEINVITEMS", xardas, hero, gold.Index, 2);

    Console.WriteLine(
        "Calling B_GIVEINVITEMS(NONE_100_XARDAS, PC_HERO, " + gold.Index + ", 1) resulted in return of " + ret);
    return 0;
    ```

=== "Python"

    ```python title="Example"
    import sys

    from zenkit import DaedalusInstance
    from zenkit import DaedalusInstanceType
    from zenkit import DaedalusVm
    from zenkit import LogLevel
    from zenkit import set_logger_default

    if len(sys.argv) != 2:
        print("Please provide a path to GOTHIC.DAT.", file=sys.stderr)
        sys.exit(-1)

    set_logger_default(LogLevel.DEBUG)

    # Load the script file into a VM
    vm = DaedalusVm.load(sys.argv[1])

    # Register a catch-all callback for all calls to un-registered external functions. ZenKit will handle all required
    # internal VM state as required so as to not corrupt the stack.
    vm.register_external_default(lambda sym: print("VM: No external for", sym.name))

    # Register implementations for external functions. Parameters and return values are checked to match their
    # definitions in the script at runtime and any mismatches between definition and implementation will cause runtime
    # exceptions to be thrown. Notice that you have to specify the argument types after providing the callback function.
    vm.register_external("INTTOSTRING", lambda i: str(i), int)
    vm.register_external("CONCATSTRINGS", lambda a, b: a + b, str, str)


    def _ai_printscreen(msg: str, _i0: int, _i1: int, font: str, _i2: int) -> bool:
        print('AI: print "' + msg + '" with font "' + font + '"')
        return True


    vm.register_external("AI_PRINTSCREEN", _ai_printscreen, str, int, int, str, int)

    # Initialize some instances. Essentially, each Daedalus instance needs to be initialized in Python which causes the
    # Daedalus code defining that instance to be executed. This needs to be done for every Daedalus instance BEFORE
    # it can be used in a script. The fields of the Daedalus instances are synced, so any change in Python will be
    # reflected in Daedalus and vice-versa.
    xardas = vm.init_instance("NONE_100_XARDAS", DaedalusInstanceType.NPC)
    hero = vm.init_instance("PC_HERO", DaedalusInstanceType.NPC)

    # You can also move the actual instance initialization call to a later point. This might be useful when you need
    # you want to manually initialize the instance in Python. To do it, first allocate an instance, then initialize it,
    # when needed, like this:
    gold = vm.alloc_instance("ITMI_GOLD", DaedalusInstanceType.ITEM)
    vm.init_instance_direct(gold)

    vm.register_external("NPC_ISPLAYER", lambda npc: npc.index == hero.index, DaedalusInstance)

    # Some calls to VM function require a certain global context variable to be set. This applies mainly to NPC
    # routine functions, state update functions (like item equip callbacks) and mission state query functions. The
    # available global context variables are:
    #
    #  * `global_self` -> var C_NPC self
    #  * `global_other` -> var C_NPC other
    #  * `global_victim` -> var C_NPC victim
    #  * `global_hero` -> var C_NPC hero
    #  * `global_item` -> var C_ITEM item
    vm.global_item = gold

    # Call a function defined the script. The first parameter can either be the function's name or the DaedalusSymbol
    # for the function. The following parameters are passed to the Daedalus function as arguments.
    #
    # In this case, the Daedalus function definition is:
    #
    #   func int B_GIVEINVITEMS(var C_NPC giver, var C_NPC taker, var int itemInstance, var int amount)
    #
    # Notice that you have to specify the return type manually.
    ret = vm.call("B_GIVEINVITEMS", xardas, hero, gold.index, 2, rtype=int)

    print("Calling B_GIVEINVITEMS(NONE_100_XARDAS, PC_HERO, " + str(gold.index) + ", 1) resulted in return of " + str(ret))
    ```
