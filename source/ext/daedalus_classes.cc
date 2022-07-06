// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/ext/daedalus_classes.hh>

#define IF_SYM_EXIST(sym, d)                                                                                           \
	do {                                                                                                               \
		if (s.find_symbol_by_name(sym) != nullptr) {                                                                   \
			d;                                                                                                         \
		}                                                                                                              \
	} while (false)

namespace phoenix::daedalus {
	void register_all_script_classes(script& s) {
		IF_SYM_EXIST("C_GILVALUES", c_gil_values::register_(s));
		IF_SYM_EXIST("C_NPC", c_npc::register_(s));
		IF_SYM_EXIST("C_MISSION", c_mission::register_(s));
		IF_SYM_EXIST("C_ITEM", c_item::register_(s));
		IF_SYM_EXIST("C_FOCUS", c_focus::register_(s));
		IF_SYM_EXIST("C_INFO", c_info::register_(s));
		IF_SYM_EXIST("C_ITEMREACT", c_item_react::register_(s));
		IF_SYM_EXIST("C_SPELL", c_spell::register_(s));
		IF_SYM_EXIST("C_SVM", c_svm::register_(s));
		IF_SYM_EXIST("C_MENU", c_menu::register_(s));
		IF_SYM_EXIST("C_MENU_ITEM", c_menu_item::register_(s));
		IF_SYM_EXIST("CCAMSYS", c_camera::register_(s));
		IF_SYM_EXIST("C_MUSICSYS_CFG", c_music_system::register_(s));
		IF_SYM_EXIST("C_MUSICTHEME", c_music_theme::register_(s));
		IF_SYM_EXIST("C_MUSICJINGLE", c_music_jingle::register_(s));
		IF_SYM_EXIST("C_PARTICLEFX", c_particle_fx::register_(s));
		IF_SYM_EXIST("CFX_BASE", c_fx_base::register_(s));
		IF_SYM_EXIST("C_PARTICLEFXEMITKEY", c_particle_fx_emit_key::register_(s));
		IF_SYM_EXIST("C_FIGHTAI", c_fight_ai::register_(s));
		IF_SYM_EXIST("C_SFX", c_sfx::register_(s));
		IF_SYM_EXIST("C_SNDSYS_CFG", c_sound_system::register_(s));
	}
} // namespace phoenix::daedalus

#undef IF_SYM_EXIST
