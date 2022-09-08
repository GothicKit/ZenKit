// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/daedalus/interpreter.hh>
#include <phoenix/ext/daedalus_classes.hh>

#include <iostream>

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Please provide a path to GOTHIC.DAT.";
		return -1;
	}

	phoenix::daedalus::vm vm {phoenix::daedalus::script::parse(argv[1])};
	phoenix::daedalus::register_all_script_classes(vm); // needed for Gothic scripts

	vm.register_default_external([](std::string_view name) { std::cout << "VM: No external for " << name << "\n"; });
	vm.register_external("INTTOSTRING", [](int i) { return std::to_string(i); });
	vm.register_external("CONCATSTRINGS",
	                     [](std::string_view a, std::string_view b) { return std::string {a} + std::string {b}; });

	vm.register_external("AI_PRINTSCREEN", [](std::string_view msg, int, int, std::string_view font, int) {
		std::cout << "AI: print \"" << msg << "\" with font \"" << font << "\"\n";
		return true;
	});

	auto xardas = vm.init_instance<phoenix::daedalus::c_npc>("NONE_100_XARDAS");
	auto hero = vm.init_instance<phoenix::daedalus::c_npc>("PC_HERO");
	auto gold = vm.init_instance<phoenix::daedalus::c_item>("ITMI_GOLD");

	vm.register_external("NPC_ISPLAYER",
	                     [&hero](std::shared_ptr<phoenix::daedalus::c_npc> npc) { return npc->id == hero->id; });

	std::cout << "\nNONE_100_XARDAS = C_NPC(\n"
	          << "    id=" << xardas->id << ",\n"
	          << "    name=" << xardas->name[0] << ",\n"
	          << "    slot=" << xardas->slot << ",\n"
	          << "    effect=" << xardas->effect << ",\n"
	          << "    npc_type=" << (int) xardas->type << ",\n"
	          << "    flags=" << (int) xardas->flags << ",\n"
	          << "    attribute=[" << xardas->attribute[0] << ", " << xardas->attribute[1] << ", "
	          << xardas->attribute[2] << ", " << xardas->attribute[3] << ", " << xardas->attribute[5] << ", "
	          << xardas->attribute[5] << ", " << xardas->attribute[6] << ", " << xardas->attribute[7] << "],\n"
	          << "    hitchance=[" << xardas->hitchance[0] << ", " << xardas->hitchance[1] << ", "
	          << xardas->hitchance[2] << ", " << xardas->hitchance[3] << ", " << xardas->hitchance[4] << ", "
	          << "],\n"
	          << "    protection=[" << xardas->protection[0] << ", " << xardas->protection[1] << ", "
	          << xardas->protection[2] << ", " << xardas->protection[3] << ", " << xardas->protection[4] << ", "
	          << xardas->protection[5] << ", " << xardas->protection[6] << ", " << xardas->protection[7] << ", "
	          << "],\n"
	          << "    damage=[" << xardas->damage[0] << ", " << xardas->damage[1] << ", " << xardas->damage[2] << ", "
	          << xardas->damage[3] << ", " << xardas->damage[4] << ", " << xardas->damage[5] << ", "
	          << xardas->damage[6] << ", " << xardas->damage[7] << ", "
	          << "],\n"
	          << "    damage_type=" << xardas->damage_type << ",\n"
	          << "    guild=" << xardas->guild << ",\n"
	          << "    level=" << xardas->level << ",\n"
	          << "    mission=[" << xardas->mission[0] << ", " << xardas->mission[1] << ", " << xardas->mission[2]
	          << ", " << xardas->mission[3] << ", " << xardas->mission[4] << ", "
	          << "],\n"
	          << "    fight_tactic=" << xardas->fight_tactic << ",\n"
	          << "    weapon=" << xardas->weapon << ",\n"
	          << "    voice=" << xardas->voice << ",\n"
	          << "    voice_pitch=" << xardas->voice_pitch << ",\n"
	          << "    body_mass=" << xardas->body_mass << ",\n"
	          << "    daily_routine=" << xardas->daily_routine << ",\n"
	          << "    start_aistate=" << xardas->start_aistate << ",\n"
	          << "    spawnpoint=" << xardas->spawnpoint << ",\n"
	          << "    spawn_delay=" << xardas->spawn_delay << ",\n"
	          << "    senses=" << xardas->senses << ",\n"
	          << "    senses_range=" << xardas->senses_range << ",\n"
	          << "    wp=" << xardas->wp << ",\n"
	          << "    exp=" << xardas->exp << ",\n"
	          << "    exp_next=" << xardas->exp_next << ",\n"
	          << "    lp=" << xardas->lp << ",\n"
	          << "    bodystate_interruptable_override=" << xardas->bodystate_interruptable_override << ",\n"
	          << "    no_focus=" << xardas->no_focus << "\n)\n\n";

	vm.global_item()->set_instance(gold);

	// A return type is provided in the template parameter (<int>). If it is left empty, the return value is ignored.
	auto ret = vm.call_function<int>("B_GIVEINVITEMS", xardas, hero, static_cast<int>(gold->symbol_index()), 2);

	std::cout << "\nCalling B_GIVEINVITEMS(NONE_100_XARDAS, PC_HERO, " << gold->symbol_index()
	          << ", 1) resulted in return of " << ret << "\n";

	return 0;
}
