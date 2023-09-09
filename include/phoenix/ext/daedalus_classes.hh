// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/addon/daedalus.hh"

#include "phoenix/script.hh"

namespace phoenix {
	using c_gil_values ZKREM("renamed to zenkit::IGuildValues") = zenkit::IGuildValues;
	using c_npc ZKREM("renamed to zenkit::INpc") = zenkit::INpc;
	using c_item ZKREM("renamed to zenkit::IItem") = zenkit::IItem;
	using c_item_react ZKREM("renamed to zenkit::IItemReact") = zenkit::IItemReact;
	using c_mission ZKREM("renamed to zenkit::IMission") = zenkit::IMission;
	using c_focus ZKREM("renamed to zenkit::IFocus") = zenkit::IFocus;
	using c_info ZKREM("renamed to zenkit::IInfo") = zenkit::IInfo;
	using c_info_choice ZKREM("renamed to zenkit::IInfoChoice") = zenkit::IInfoChoice;
	using c_spell ZKREM("renamed to zenkit::ISpell") = zenkit::ISpell;
	using c_svm ZKREM("renamed to zenkit::ISvm") = zenkit::ISvm;
	using c_menu ZKREM("renamed to zenkit::IMenu") = zenkit::IMenu;
	using c_menu_item ZKREM("renamed to zenkit::IMenuItem") = zenkit::IMenuItem;
	using c_camera ZKREM("renamed to zenkit::ICamera") = zenkit::ICamera;
	using c_music_system ZKREM("renamed to zenkit::IMusicSystem") = zenkit::IMusicSystem;
	using c_music_theme ZKREM("renamed to zenkit::IMusicTheme") = zenkit::IMusicTheme;
	using c_music_jingle ZKREM("renamed to zenkit::IMusicJingle") = zenkit::IMusicJingle;
	using c_particle_fx ZKREM("renamed to zenkit::IParticleEffect") = zenkit::IParticleEffect;
	using c_particle_fx_emit_key ZKREM("renamed to zenkit::IParticleEffectEmitKey") = zenkit::IParticleEffectEmitKey;
	using c_fx_base ZKREM("renamed to zenkit::IEffectBase") = zenkit::IEffectBase;
	using c_fight_ai ZKREM("renamed to zenkit::IFightAi") = zenkit::IFightAi;
	using c_sfx ZKREM("renamed to zenkit::ISoundEffect") = zenkit::ISoundEffect;
	using c_sound_system ZKREM("renamed to zenkit::ISoundSystem") = zenkit::ISoundSystem;

	namespace damage_type = zenkit::DamageType;
	namespace npc_attribute = zenkit::NpcAttribute;

	using npc_type ZKREM("renamed to zenkit::NpcType") = zenkit::NpcType;
	using npc_flag ZKREM("renamed to zenkit::NpcFlag") = zenkit::NpcFlag;
	using item_flags ZKREM("renamed to zenkit::ItemFlag") = zenkit::ItemFlag;
	using c_menu_flags ZKREM("renamed to zenkit::MenuFlag") = zenkit::MenuFlag;
	using c_menu_item_flags ZKREM("renamed to zenkit::MenuItemFlag") = zenkit::MenuItemFlag;
	using c_menu_item_type ZKREM("renamed to zenkit::MenuItemType") = zenkit::MenuItemType;
	using c_menu_item_select_event ZKREM("renamed to zenkit::MenuItemSelectEvent") = zenkit::MenuItemSelectEvent;
	using c_menu_item_select_action ZKREM("renamed to zenkit::MenuItemSelectAction") = zenkit::MenuItemSelectAction;
	using music_transition_type ZKREM("renamed to zenkit::MusicTransitionEffect") = zenkit::MusicTransitionEffect;
	using music_transition_subtype ZKREM("renamed to zenkit::MusicTransitionType") = zenkit::MusicTransitionType;
	using c_fight_ai_move ZKREM("renamed to zenkit::FightAiMove") = zenkit::FightAiMove;

	ZKREM("renamed to zenkit::register_all_script_classes")
	inline void register_all_script_classes(zenkit::DaedalusScript& s) {
		zenkit::register_all_script_classes(s);
	}
} // namespace phoenix
