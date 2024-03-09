// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
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
	// you want to manually initialize the instance in C++. To do it, first allocate an instance, then initialize it,
	// when needed, like this:
	auto gold = vm.allocate_instance<zenkit::IItem>("ITMI_GOLD");
	vm.init_instance(gold, "ITMI_GOLD");

	// External callbacks may also capture any local state, if needed.
	vm.register_external("NPC_ISPLAYER", [&hero](std::shared_ptr<zenkit::INpc> npc) { //
		return npc->id == hero->id;
	});

	// Some calls to VM function require a certain global context variable to be set. This applies mainly to NPC
	// routine functions, state update functions (like item equip callbacks) and mission state query functions. The
	// available global context variables are:
	//
	//  * `global_self` -> var C_NPC self
	//  * `global_other` -> var C_NPC other
	//  * `global_victim` -> var C_NPC victim
	//  * `global_hero` -> var C_NPC hero
	//  * `global_item` -> var C_ITEM item
	vm.global_item()->set_instance(gold);

	// Call a function defined the script. The first parameter can either be the function's name or the DaedalusSymbol
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
