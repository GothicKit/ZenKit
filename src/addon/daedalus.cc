// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/addon/daedalus.hh"

#include "../Internal.hh"

#define IF_SYM_EXIST(sym, d)                                                                                           \
	do {                                                                                                               \
		if (s.find_symbol_by_name(sym) != nullptr) {                                                                   \
			d;                                                                                                         \
		}                                                                                                              \
	} while (false)

namespace zenkit {
	void register_all_script_classes(DaedalusScript& s) {
		IF_SYM_EXIST("C_GILVALUES", IGuildValues::register_(s));
		IF_SYM_EXIST("C_NPC", INpc::register_(s));
		IF_SYM_EXIST("C_MISSION", IMission::register_(s));
		IF_SYM_EXIST("C_ITEM", IItem::register_(s));
		IF_SYM_EXIST("C_FOCUS", IFocus::register_(s));
		IF_SYM_EXIST("C_INFO", IInfo::register_(s));
		IF_SYM_EXIST("C_ITEMREACT", IItemReact::register_(s));
		IF_SYM_EXIST("C_SPELL", ISpell::register_(s));
		IF_SYM_EXIST("C_SVM", ISvm::register_(s));
		IF_SYM_EXIST("C_MENU", IMenu::register_(s));
		IF_SYM_EXIST("C_MENU_ITEM", IMenuItem::register_(s));
		IF_SYM_EXIST("CCAMSYS", ICamera::register_(s));
		IF_SYM_EXIST("C_MUSICSYS_CFG", IMusicSystem::register_(s));
		IF_SYM_EXIST("C_MUSICTHEME", IMusicTheme::register_(s));
		IF_SYM_EXIST("C_MUSICJINGLE", IMusicJingle::register_(s));
		IF_SYM_EXIST("C_PARTICLEFX", IParticleEffect::register_(s));
		IF_SYM_EXIST("CFX_BASE", IEffectBase::register_(s));
		IF_SYM_EXIST("C_PARTICLEFXEMITKEY", IParticleEffectEmitKey::register_(s));
		IF_SYM_EXIST("C_FIGHTAI", IFightAi::register_(s));
		IF_SYM_EXIST("C_SFX", ISoundEffect::register_(s));
		IF_SYM_EXIST("C_SNDSYS_CFG", ISoundSystem::register_(s));
	}
} // namespace zenkit

#undef IF_SYM_EXIST

#define ZKLOG_CLASS(a, b) ZKLOGD("Daedalus", "Registering script class \"%s\" as zenkit::%s", a, b)

void zenkit::IGuildValues::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_GILVALUES", "IGuildValues");
	s.register_member("C_GILVALUES.WATER_DEPTH_KNEE", &IGuildValues::water_depth_knee);
	s.register_member("C_GILVALUES.WATER_DEPTH_CHEST", &IGuildValues::water_depth_chest);
	s.register_member("C_GILVALUES.JUMPUP_HEIGHT", &IGuildValues::jumpup_height);
	s.register_member("C_GILVALUES.SWIM_TIME", &IGuildValues::swim_time);
	s.register_member("C_GILVALUES.DIVE_TIME", &IGuildValues::dive_time);
	s.register_member("C_GILVALUES.STEP_HEIGHT", &IGuildValues::step_height);
	s.register_member("C_GILVALUES.JUMPLOW_HEIGHT", &IGuildValues::jumplow_height);
	s.register_member("C_GILVALUES.JUMPMID_HEIGHT", &IGuildValues::jumpmid_height);
	s.register_member("C_GILVALUES.SLIDE_ANGLE", &IGuildValues::slide_angle);
	s.register_member("C_GILVALUES.SLIDE_ANGLE2", &IGuildValues::slide_angle2);
	s.register_member("C_GILVALUES.DISABLE_AUTOROLL", &IGuildValues::disable_autoroll);
	s.register_member("C_GILVALUES.SURFACE_ALIGN", &IGuildValues::surface_align);
	s.register_member("C_GILVALUES.CLIMB_HEADING_ANGLE", &IGuildValues::climb_heading_angle);
	s.register_member("C_GILVALUES.CLIMB_HORIZ_ANGLE", &IGuildValues::climb_horiz_angle);
	s.register_member("C_GILVALUES.CLIMB_GROUND_ANGLE", &IGuildValues::climb_ground_angle);
	s.register_member("C_GILVALUES.FIGHT_RANGE_BASE", &IGuildValues::fight_range_base);
	s.register_member("C_GILVALUES.FIGHT_RANGE_FIST", &IGuildValues::fight_range_fist);
	s.register_member("C_GILVALUES.FIGHT_RANGE_1HS", &IGuildValues::fight_range_1hs);
	s.register_member("C_GILVALUES.FIGHT_RANGE_1HA", &IGuildValues::fight_range_1ha);
	s.register_member("C_GILVALUES.FIGHT_RANGE_2HS", &IGuildValues::fight_range_2hs);
	s.register_member("C_GILVALUES.FIGHT_RANGE_2HA", &IGuildValues::fight_range_2ha);
	s.register_member("C_GILVALUES.FALLDOWN_HEIGHT", &IGuildValues::falldown_height);
	s.register_member("C_GILVALUES.FALLDOWN_DAMAGE", &IGuildValues::falldown_damage);
	s.register_member("C_GILVALUES.BLOOD_DISABLED", &IGuildValues::blood_disabled);
	s.register_member("C_GILVALUES.BLOOD_MAX_DISTANCE", &IGuildValues::blood_max_distance);
	s.register_member("C_GILVALUES.BLOOD_AMOUNT", &IGuildValues::blood_amount);
	s.register_member("C_GILVALUES.BLOOD_FLOW", &IGuildValues::blood_flow);
	s.register_member("C_GILVALUES.BLOOD_EMITTER", &IGuildValues::blood_emitter);
	s.register_member("C_GILVALUES.BLOOD_TEXTURE", &IGuildValues::blood_texture);
	s.register_member("C_GILVALUES.TURN_SPEED", &IGuildValues::turn_speed);

	// Gothic 2 only
	if (s.find_symbol_by_name("C_GILVALUES.FIGHT_RANGE_G") != nullptr) {
		s.register_member("C_GILVALUES.FIGHT_RANGE_G", &IGuildValues::fight_range_g);
	}
}

void zenkit::IFightAi::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_FIGHTAI", "IFightAi");
	s.register_member("C_FIGHTAI.MOVE", &IFightAi::move);
}

void zenkit::ISoundEffect::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_SFX", "ISoundEffect");
	s.register_member("C_SFX.FILE", &ISoundEffect::file);
	s.register_member("C_SFX.PITCHOFF", &ISoundEffect::pitch_off);
	s.register_member("C_SFX.PITCHVAR", &ISoundEffect::pitch_var);
	s.register_member("C_SFX.VOL", &ISoundEffect::vol);
	s.register_member("C_SFX.LOOP", &ISoundEffect::loop);
	s.register_member("C_SFX.LOOPSTARTOFFSET", &ISoundEffect::loop_start_offset);
	s.register_member("C_SFX.LOOPENDOFFSET", &ISoundEffect::loop_end_offset);
	s.register_member("C_SFX.REVERBLEVEL", &ISoundEffect::reverb_level);
	s.register_member("C_SFX.PFXNAME", &ISoundEffect::pfx_name);
}

void zenkit::ISoundSystem::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_SNDSYS_CFG", "ISoundSystem");
	s.register_member("C_SNDSYS_CFG.VOLUME", &ISoundSystem::volume);
	s.register_member("C_SNDSYS_CFG.BITRESOLUTION", &ISoundSystem::bit_resolution);
	s.register_member("C_SNDSYS_CFG.SAMPLERATE", &ISoundSystem::sample_rate);
	s.register_member("C_SNDSYS_CFG.USESTEREO", &ISoundSystem::use_stereo);
	s.register_member("C_SNDSYS_CFG.NUMSFXCHANNELS", &ISoundSystem::num_sfx_channels);
	s.register_member("C_SNDSYS_CFG.USED3DPROVIDERNAME", &ISoundSystem::used_3d_provider_name);
}

void zenkit::IParticleEffectEmitKey::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_PARTICLEFXEMITKEY", "IParticleEffectEmitKey");
	s.register_member("C_PARTICLEFXEMITKEY.VISNAME_S", &IParticleEffectEmitKey::vis_name_s);
	s.register_member("C_PARTICLEFXEMITKEY.VISSIZESCALE", &IParticleEffectEmitKey::vis_size_scale);
	s.register_member("C_PARTICLEFXEMITKEY.SCALEDURATION", &IParticleEffectEmitKey::scale_duration);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_PPSVALUE", &IParticleEffectEmitKey::pfx_pps_value);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_PPSISSMOOTHCHG", &IParticleEffectEmitKey::pfx_pps_is_smooth_chg);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_PPSISLOOPINGCHG", &IParticleEffectEmitKey::pfx_pps_is_looping_chg);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_SCTIME", &IParticleEffectEmitKey::pfx_sc_time);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_FLYGRAVITY_S", &IParticleEffectEmitKey::pfx_fly_gravity_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPDIM_S", &IParticleEffectEmitKey::pfx_shp_dim_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPISVOLUMECHG", &IParticleEffectEmitKey::pfx_shp_is_volume_chg);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPSCALEFPS", &IParticleEffectEmitKey::pfx_shp_scale_fps);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPDISTRIBWALKSPEED",
	                  &IParticleEffectEmitKey::pfx_shp_distrib_walks_peed);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPOFFSETVEC_S", &IParticleEffectEmitKey::pfx_shp_offset_vec_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPDISTRIBTYPE_S", &IParticleEffectEmitKey::pfx_shp_distrib_type_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRMODE_S", &IParticleEffectEmitKey::pfx_dir_mode_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRFOR_S", &IParticleEffectEmitKey::pfx_dir_for_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRMODETARGETFOR_S", &IParticleEffectEmitKey::pfx_dir_mode_target_for_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRMODETARGETPOS_S", &IParticleEffectEmitKey::pfx_dir_mode_target_pos_s);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_VELAVG", &IParticleEffectEmitKey::pfx_vel_avg);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_LSPPARTAVG", &IParticleEffectEmitKey::pfx_lsp_part_avg);
	s.register_member("C_PARTICLEFXEMITKEY.PFX_VISALPHASTART", &IParticleEffectEmitKey::pfx_vis_alpha_start);
	s.register_member("C_PARTICLEFXEMITKEY.LIGHTPRESETNAME", &IParticleEffectEmitKey::light_preset_name);
	s.register_member("C_PARTICLEFXEMITKEY.LIGHTRANGE", &IParticleEffectEmitKey::light_range);
	s.register_member("C_PARTICLEFXEMITKEY.SFXID", &IParticleEffectEmitKey::sfx_id);
	s.register_member("C_PARTICLEFXEMITKEY.SFXISAMBIENT", &IParticleEffectEmitKey::sfx_is_ambient);
	s.register_member("C_PARTICLEFXEMITKEY.EMCREATEFXID", &IParticleEffectEmitKey::em_create_fx_id);
	s.register_member("C_PARTICLEFXEMITKEY.EMFLYGRAVITY", &IParticleEffectEmitKey::em_fly_gravity);
	s.register_member("C_PARTICLEFXEMITKEY.EMSELFROTVEL_S", &IParticleEffectEmitKey::em_self_rot_vel_s);
	s.register_member("C_PARTICLEFXEMITKEY.EMTRJMODE_S", &IParticleEffectEmitKey::em_trj_mode_s);
	s.register_member("C_PARTICLEFXEMITKEY.EMTRJEASEVEL", &IParticleEffectEmitKey::em_trj_ease_vel);
	s.register_member("C_PARTICLEFXEMITKEY.EMCHECKCOLLISION", &IParticleEffectEmitKey::em_check_collision);
	s.register_member("C_PARTICLEFXEMITKEY.EMFXLIFESPAN", &IParticleEffectEmitKey::em_fx_lifespan);
}

void zenkit::INpc::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_NPC", "INpc");
	s.register_member("C_NPC.ID", &INpc::id);
	s.register_member("C_NPC.NAME", &INpc::name);
	s.register_member("C_NPC.SLOT", &INpc::slot);
	s.register_member("C_NPC.NPCTYPE", &INpc::type);
	s.register_member("C_NPC.FLAGS", &INpc::flags);
	s.register_member("C_NPC.ATTRIBUTE", &INpc::attribute);
	s.register_member("C_NPC.PROTECTION", &INpc::protection);
	s.register_member("C_NPC.DAMAGE", &INpc::damage);
	s.register_member("C_NPC.DAMAGETYPE", &INpc::damage_type);
	s.register_member("C_NPC.GUILD", &INpc::guild);
	s.register_member("C_NPC.LEVEL", &INpc::level);
	s.register_member("C_NPC.MISSION", &INpc::mission);
	s.register_member("C_NPC.FIGHT_TACTIC", &INpc::fight_tactic);
	s.register_member("C_NPC.WEAPON", &INpc::weapon);
	s.register_member("C_NPC.VOICE", &INpc::voice);
	s.register_member("C_NPC.VOICEPITCH", &INpc::voice_pitch);
	s.register_member("C_NPC.BODYMASS", &INpc::body_mass);
	s.register_member("C_NPC.DAILY_ROUTINE", &INpc::daily_routine);
	s.register_member("C_NPC.START_AISTATE", &INpc::start_aistate);
	s.register_member("C_NPC.SPAWNPOINT", &INpc::spawnpoint);
	s.register_member("C_NPC.SPAWNDELAY", &INpc::spawn_delay);
	s.register_member("C_NPC.SENSES", &INpc::senses);
	s.register_member("C_NPC.SENSES_RANGE", &INpc::senses_range);
	s.register_member("C_NPC.AIVAR", &INpc::aivar);
	s.register_member("C_NPC.WP", &INpc::wp);
	s.register_member("C_NPC.EXP", &INpc::exp);
	s.register_member("C_NPC.EXP_NEXT", &INpc::exp_next);
	s.register_member("C_NPC.LP", &INpc::lp);

	// Gothic 2 only
	if (s.find_symbol_by_name("C_NPC.EFFECT") != nullptr) {
		s.register_member("C_NPC.EFFECT", &INpc::effect);
		s.register_member("C_NPC.HITCHANCE", &INpc::hitchance);
		s.register_member("C_NPC.BODYSTATEINTERRUPTABLEOVERRIDE", &INpc::bodystate_interruptable_override);
		s.register_member("C_NPC.NOFOCUS", &INpc::no_focus);
	}
}

void zenkit::IMission::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_MISSION", "IMission");
	s.register_member("C_MISSION.NAME", &IMission::name);
	s.register_member("C_MISSION.DESCRIPTION", &IMission::description);
	s.register_member("C_MISSION.DURATION", &IMission::duration);
	s.register_member("C_MISSION.IMPORTANT", &IMission::important);
	s.register_member("C_MISSION.OFFERCONDITIONS", &IMission::offer_conditions);
	s.register_member("C_MISSION.OFFER", &IMission::offer);
	s.register_member("C_MISSION.SUCCESSCONDITIONS", &IMission::success_conditions);
	s.register_member("C_MISSION.SUCCESS", &IMission::success);
	s.register_member("C_MISSION.FAILURECONDITIONS", &IMission::failure_conditions);
	s.register_member("C_MISSION.FAILURE", &IMission::failure);
	s.register_member("C_MISSION.OBSOLETECONDITIONS", &IMission::obsolete_conditions);
	s.register_member("C_MISSION.OBSOLETE", &IMission::obsolete);
	s.register_member("C_MISSION.RUNNING", &IMission::running);
}

void zenkit::IItem::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_ITEM", "IItem");
	s.register_member("C_ITEM.ID", &IItem::id);
	s.register_member("C_ITEM.NAME", &IItem::name);
	s.register_member("C_ITEM.NAMEID", &IItem::name_id);
	s.register_member("C_ITEM.HP", &IItem::hp);
	s.register_member("C_ITEM.HP_MAX", &IItem::hp_max);
	s.register_member("C_ITEM.MAINFLAG", &IItem::main_flag);
	s.register_member("C_ITEM.FLAGS", &IItem::flags);
	s.register_member("C_ITEM.WEIGHT", &IItem::weight);
	s.register_member("C_ITEM.VALUE", &IItem::value);
	s.register_member("C_ITEM.DAMAGETYPE", &IItem::damage_type);
	s.register_member("C_ITEM.DAMAGETOTAL", &IItem::damage_total);
	s.register_member("C_ITEM.DAMAGE", &IItem::damage);
	s.register_member("C_ITEM.WEAR", &IItem::wear);
	s.register_member("C_ITEM.PROTECTION", &IItem::protection);
	s.register_member("C_ITEM.NUTRITION", &IItem::nutrition);
	s.register_member("C_ITEM.COND_ATR", &IItem::cond_atr);
	s.register_member("C_ITEM.COND_VALUE", &IItem::cond_value);
	s.register_member("C_ITEM.CHANGE_ATR", &IItem::change_atr);
	s.register_member("C_ITEM.CHANGE_VALUE", &IItem::change_value);
	s.register_member("C_ITEM.MAGIC", &IItem::magic);
	s.register_member("C_ITEM.ON_EQUIP", &IItem::on_equip);
	s.register_member("C_ITEM.ON_UNEQUIP", &IItem::on_unequip);
	s.register_member("C_ITEM.ON_STATE", &IItem::on_state);
	s.register_member("C_ITEM.OWNER", &IItem::owner);
	s.register_member("C_ITEM.OWNERGUILD", &IItem::owner_guild);
	s.register_member("C_ITEM.DISGUISEGUILD", &IItem::disguise_guild);
	s.register_member("C_ITEM.VISUAL", &IItem::visual);
	s.register_member("C_ITEM.VISUAL_CHANGE", &IItem::visual_change);
	s.register_member("C_ITEM.VISUAL_SKIN", &IItem::visual_skin);
	s.register_member("C_ITEM.SCEMENAME", &IItem::scheme_name);
	s.register_member("C_ITEM.MATERIAL", &IItem::material);
	s.register_member("C_ITEM.MUNITION", &IItem::munition);
	s.register_member("C_ITEM.SPELL", &IItem::spell);
	s.register_member("C_ITEM.RANGE", &IItem::range);
	s.register_member("C_ITEM.MAG_CIRCLE", &IItem::mag_circle);
	s.register_member("C_ITEM.DESCRIPTION", &IItem::description);
	s.register_member("C_ITEM.TEXT", &IItem::text);
	s.register_member("C_ITEM.COUNT", &IItem::count);

	// Gothic 2 only
	if (s.find_symbol_by_name("C_ITEM.EFFECT") != nullptr) {
		s.register_member("C_ITEM.EFFECT", &IItem::effect);
		s.register_member("C_ITEM.INV_ZBIAS", &IItem::inv_zbias);
		s.register_member("C_ITEM.INV_ROTX", &IItem::inv_rot_x);
		s.register_member("C_ITEM.INV_ROTY", &IItem::inv_rot_y);
		s.register_member("C_ITEM.INV_ROTZ", &IItem::inv_rot_z);
		s.register_member("C_ITEM.INV_ANIMATE", &IItem::inv_animate);
	}
}

void zenkit::IFocus::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_FOCUS", "IFocus");
	s.register_member("C_FOCUS.NPC_LONGRANGE", &IFocus::npc_longrange);
	s.register_member("C_FOCUS.NPC_RANGE1", &IFocus::npc_range1);
	s.register_member("C_FOCUS.NPC_RANGE2", &IFocus::npc_range2);
	s.register_member("C_FOCUS.NPC_AZI", &IFocus::npc_azi);
	s.register_member("C_FOCUS.NPC_ELEVDO", &IFocus::npc_elevdo);
	s.register_member("C_FOCUS.NPC_ELEVUP", &IFocus::npc_elevup);
	s.register_member("C_FOCUS.NPC_PRIO", &IFocus::npc_prio);
	s.register_member("C_FOCUS.ITEM_RANGE1", &IFocus::item_range1);
	s.register_member("C_FOCUS.ITEM_RANGE2", &IFocus::item_range2);
	s.register_member("C_FOCUS.ITEM_AZI", &IFocus::item_azi);
	s.register_member("C_FOCUS.ITEM_ELEVDO", &IFocus::item_elevdo);
	s.register_member("C_FOCUS.ITEM_ELEVUP", &IFocus::item_elevup);
	s.register_member("C_FOCUS.ITEM_PRIO", &IFocus::item_prio);
	s.register_member("C_FOCUS.MOB_RANGE1", &IFocus::mob_range1);
	s.register_member("C_FOCUS.MOB_RANGE2", &IFocus::mob_range2);
	s.register_member("C_FOCUS.MOB_AZI", &IFocus::mob_azi);
	s.register_member("C_FOCUS.MOB_ELEVDO", &IFocus::mob_elevdo);
	s.register_member("C_FOCUS.MOB_ELEVUP", &IFocus::mob_elevup);
	s.register_member("C_FOCUS.MOB_PRIO", &IFocus::mob_prio);
}

void zenkit::IInfo::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_INFO", "IInfo");
	s.register_member("C_INFO.NPC", &IInfo::npc);
	s.register_member("C_INFO.NR", &IInfo::nr);
	s.register_member("C_INFO.IMPORTANT", &IInfo::important);
	s.register_member("C_INFO.CONDITION", &IInfo::condition);
	s.register_member("C_INFO.INFORMATION", &IInfo::information);
	s.register_member("C_INFO.DESCRIPTION", &IInfo::description);
	s.register_member("C_INFO.TRADE", &IInfo::trade);
	s.register_member("C_INFO.PERMANENT", &IInfo::permanent);
}

void zenkit::IInfo::add_choice(zenkit::IInfoChoice const& ch) {
	choices.insert(choices.begin(), ch);
}

void zenkit::IInfo::remove_choice(std::size_t index) {
	choices.erase(choices.begin() + static_cast<decltype(choices)::difference_type>(index));
}

void zenkit::IItemReact::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_ITEMREACT", "IItemReact");
	s.register_member("C_ITEMREACT.NPC", &IItemReact::npc);
	s.register_member("C_ITEMREACT.TRADE_ITEM", &IItemReact::trade_item);
	s.register_member("C_ITEMREACT.TRADE_AMOUNT", &IItemReact::trade_amount);
	s.register_member("C_ITEMREACT.REQUESTED_CAT", &IItemReact::requested_cat);
	s.register_member("C_ITEMREACT.REQUESTED_ITEM", &IItemReact::requested_item);
	s.register_member("C_ITEMREACT.REQUESTED_AMOUNT", &IItemReact::requested_amount);
	s.register_member("C_ITEMREACT.REACTION", &IItemReact::reaction);
}

void zenkit::ISpell::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_SPELL", "ISpell");
	s.register_member("C_SPELL.TIME_PER_MANA", &ISpell::time_per_mana);
	s.register_member("C_SPELL.DAMAGE_PER_LEVEL", &ISpell::damage_per_level);
	s.register_member("C_SPELL.DAMAGETYPE", &ISpell::damage_type);
	s.register_member("C_SPELL.SPELLTYPE", &ISpell::spell_type);
	s.register_member("C_SPELL.CANTURNDURINGINVEST", &ISpell::can_turn_during_invest);
	s.register_member("C_SPELL.CANCHANGETARGETDURINGINVEST", &ISpell::can_change_target_during_invest);
	s.register_member("C_SPELL.ISMULTIEFFECT", &ISpell::is_multi_effect);
	s.register_member("C_SPELL.TARGETCOLLECTALGO", &ISpell::target_collect_algo);
	s.register_member("C_SPELL.TARGETCOLLECTTYPE", &ISpell::target_collect_type);
	s.register_member("C_SPELL.TARGETCOLLECTRANGE", &ISpell::target_collect_range);
	s.register_member("C_SPELL.TARGETCOLLECTAZI", &ISpell::target_collect_azi);
	s.register_member("C_SPELL.TARGETCOLLECTELEV", &ISpell::target_collect_elev);
}

#define REG_IF_SYM_EXIST(sym, ref)                                                                                     \
	do {                                                                                                               \
		if (s.find_symbol_by_name(sym)) {                                                                              \
			s.register_member(sym, ref);                                                                               \
		}                                                                                                              \
	} while (false)

void zenkit::ISvm::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_SVM", "ISvm");
	REG_IF_SYM_EXIST("C_SVM.MILGREETINGS", &ISvm::MILGREETINGS);
	REG_IF_SYM_EXIST("C_SVM.PALGREETINGS", &ISvm::PALGREETINGS);
	REG_IF_SYM_EXIST("C_SVM.WEATHER", &ISvm::WEATHER);
	REG_IF_SYM_EXIST("C_SVM.IGETYOUSTILL", &ISvm::IGETYOUSTILL);
	REG_IF_SYM_EXIST("C_SVM.DIEENEMY", &ISvm::DIEENEMY);
	REG_IF_SYM_EXIST("C_SVM.DIEMONSTER", &ISvm::DIEMONSTER);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIEMONSTER", &ISvm::ADDON_DIEMONSTER);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIEMONSTER2", &ISvm::ADDON_DIEMONSTER2);
	REG_IF_SYM_EXIST("C_SVM.DIRTYTHIEF", &ISvm::DIRTYTHIEF);
	REG_IF_SYM_EXIST("C_SVM.HANDSOFF", &ISvm::HANDSOFF);
	REG_IF_SYM_EXIST("C_SVM.SHEEPKILLER", &ISvm::SHEEPKILLER);
	REG_IF_SYM_EXIST("C_SVM.SHEEPKILLERMONSTER", &ISvm::SHEEPKILLERMONSTER);
	REG_IF_SYM_EXIST("C_SVM.YOUMURDERER", &ISvm::YOUMURDERER);
	REG_IF_SYM_EXIST("C_SVM.DIESTUPIDBEAST", &ISvm::DIESTUPIDBEAST);
	REG_IF_SYM_EXIST("C_SVM.YOUDAREHITME", &ISvm::YOUDAREHITME);
	REG_IF_SYM_EXIST("C_SVM.YOUASKEDFORIT", &ISvm::YOUASKEDFORIT);
	REG_IF_SYM_EXIST("C_SVM.THENIBEATYOUOUTOFHERE", &ISvm::THENIBEATYOUOUTOFHERE);
	REG_IF_SYM_EXIST("C_SVM.WHATDIDYOUDOINTHERE", &ISvm::WHATDIDYOUDOINTHERE);
	REG_IF_SYM_EXIST("C_SVM.WILLYOUSTOPFIGHTING", &ISvm::WILLYOUSTOPFIGHTING);
	REG_IF_SYM_EXIST("C_SVM.KILLENEMY", &ISvm::KILLENEMY);
	REG_IF_SYM_EXIST("C_SVM.ENEMYKILLED", &ISvm::ENEMYKILLED);
	REG_IF_SYM_EXIST("C_SVM.MONSTERKILLED", &ISvm::MONSTERKILLED);
	REG_IF_SYM_EXIST("C_SVM.ADDON_MONSTERKILLED", &ISvm::ADDON_MONSTERKILLED);
	REG_IF_SYM_EXIST("C_SVM.ADDON_MONSTERKILLED2", &ISvm::ADDON_MONSTERKILLED2);
	REG_IF_SYM_EXIST("C_SVM.THIEFDOWN", &ISvm::THIEFDOWN);
	REG_IF_SYM_EXIST("C_SVM.RUMFUMMLERDOWN", &ISvm::RUMFUMMLERDOWN);
	REG_IF_SYM_EXIST("C_SVM.SHEEPATTACKERDOWN", &ISvm::SHEEPATTACKERDOWN);
	REG_IF_SYM_EXIST("C_SVM.KILLMURDERER", &ISvm::KILLMURDERER);
	REG_IF_SYM_EXIST("C_SVM.STUPIDBEASTKILLED", &ISvm::STUPIDBEASTKILLED);
	REG_IF_SYM_EXIST("C_SVM.NEVERHITMEAGAIN", &ISvm::NEVERHITMEAGAIN);
	REG_IF_SYM_EXIST("C_SVM.YOUBETTERSHOULDHAVELISTENED", &ISvm::YOUBETTERSHOULDHAVELISTENED);
	REG_IF_SYM_EXIST("C_SVM.GETUPANDBEGONE", &ISvm::GETUPANDBEGONE);
	REG_IF_SYM_EXIST("C_SVM.NEVERENTERROOMAGAIN", &ISvm::NEVERENTERROOMAGAIN);
	REG_IF_SYM_EXIST("C_SVM.THEREISNOFIGHTINGHERE", &ISvm::THEREISNOFIGHTINGHERE);
	REG_IF_SYM_EXIST("C_SVM.SPAREME", &ISvm::SPAREME);
	REG_IF_SYM_EXIST("C_SVM.RUNAWAY", &ISvm::RUNAWAY);
	REG_IF_SYM_EXIST("C_SVM.ALARM", &ISvm::ALARM);
	REG_IF_SYM_EXIST("C_SVM.GUARDS", &ISvm::GUARDS);
	REG_IF_SYM_EXIST("C_SVM.HELP", &ISvm::HELP);
	REG_IF_SYM_EXIST("C_SVM.GOODMONSTERKILL", &ISvm::GOODMONSTERKILL);
	REG_IF_SYM_EXIST("C_SVM.GOODKILL", &ISvm::GOODKILL);
	REG_IF_SYM_EXIST("C_SVM.NOTNOW", &ISvm::NOTNOW);
	REG_IF_SYM_EXIST("C_SVM.RUNCOWARD", &ISvm::RUNCOWARD);
	REG_IF_SYM_EXIST("C_SVM.GETOUTOFHERE", &ISvm::GETOUTOFHERE);
	REG_IF_SYM_EXIST("C_SVM.WHYAREYOUINHERE", &ISvm::WHYAREYOUINHERE);
	REG_IF_SYM_EXIST("C_SVM.YESGOOUTOFHERE", &ISvm::YESGOOUTOFHERE);
	REG_IF_SYM_EXIST("C_SVM.WHATSTHISSUPPOSEDTOBE", &ISvm::WHATSTHISSUPPOSEDTOBE);
	REG_IF_SYM_EXIST("C_SVM.YOUDISTURBEDMYSLUMBER", &ISvm::YOUDISTURBEDMYSLUMBER);
	REG_IF_SYM_EXIST("C_SVM.ITOOKYOURGOLD", &ISvm::ITOOKYOURGOLD);
	REG_IF_SYM_EXIST("C_SVM.SHITNOGOLD", &ISvm::SHITNOGOLD);
	REG_IF_SYM_EXIST("C_SVM.ITAKEYOURWEAPON", &ISvm::ITAKEYOURWEAPON);
	REG_IF_SYM_EXIST("C_SVM.WHATAREYOUDOING", &ISvm::WHATAREYOUDOING);
	REG_IF_SYM_EXIST("C_SVM.LOOKINGFORTROUBLEAGAIN", &ISvm::LOOKINGFORTROUBLEAGAIN);
	REG_IF_SYM_EXIST("C_SVM.STOPMAGIC", &ISvm::STOPMAGIC);
	REG_IF_SYM_EXIST("C_SVM.ISAIDSTOPMAGIC", &ISvm::ISAIDSTOPMAGIC);
	REG_IF_SYM_EXIST("C_SVM.WEAPONDOWN", &ISvm::WEAPONDOWN);
	REG_IF_SYM_EXIST("C_SVM.ISAIDWEAPONDOWN", &ISvm::ISAIDWEAPONDOWN);
	REG_IF_SYM_EXIST("C_SVM.WISEMOVE", &ISvm::WISEMOVE);
	REG_IF_SYM_EXIST("C_SVM.NEXTTIMEYOUREINFORIT", &ISvm::NEXTTIMEYOUREINFORIT);
	REG_IF_SYM_EXIST("C_SVM.OHMYHEAD", &ISvm::OHMYHEAD);
	REG_IF_SYM_EXIST("C_SVM.THERESAFIGHT", &ISvm::THERESAFIGHT);
	REG_IF_SYM_EXIST("C_SVM.OHMYGODITSAFIGHT", &ISvm::OHMYGODITSAFIGHT);
	REG_IF_SYM_EXIST("C_SVM.GOODVICTORY", &ISvm::GOODVICTORY);
	REG_IF_SYM_EXIST("C_SVM.NOTBAD", &ISvm::NOTBAD);
	REG_IF_SYM_EXIST("C_SVM.OHMYGODHESDOWN", &ISvm::OHMYGODHESDOWN);
	REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND01", &ISvm::CHEERFRIEND01);
	REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND02", &ISvm::CHEERFRIEND02);
	REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND03", &ISvm::CHEERFRIEND03);
	REG_IF_SYM_EXIST("C_SVM.OOH01", &ISvm::OOH01);
	REG_IF_SYM_EXIST("C_SVM.OOH02", &ISvm::OOH02);
	REG_IF_SYM_EXIST("C_SVM.OOH03", &ISvm::OOH03);
	REG_IF_SYM_EXIST("C_SVM.WHATWASTHAT", &ISvm::WHATWASTHAT);
	REG_IF_SYM_EXIST("C_SVM.GETOUTOFMYBED", &ISvm::GETOUTOFMYBED);
	REG_IF_SYM_EXIST("C_SVM.AWAKE", &ISvm::AWAKE);
	REG_IF_SYM_EXIST("C_SVM.ABS_COMMANDER", &ISvm::ABS_COMMANDER);
	REG_IF_SYM_EXIST("C_SVM.ABS_MONASTERY", &ISvm::ABS_MONASTERY);
	REG_IF_SYM_EXIST("C_SVM.ABS_FARM", &ISvm::ABS_FARM);
	REG_IF_SYM_EXIST("C_SVM.ABS_GOOD", &ISvm::ABS_GOOD);
	REG_IF_SYM_EXIST("C_SVM.SHEEPKILLER_CRIME", &ISvm::SHEEPKILLER_CRIME);
	REG_IF_SYM_EXIST("C_SVM.ATTACK_CRIME", &ISvm::ATTACK_CRIME);
	REG_IF_SYM_EXIST("C_SVM.THEFT_CRIME", &ISvm::THEFT_CRIME);
	REG_IF_SYM_EXIST("C_SVM.MURDER_CRIME", &ISvm::MURDER_CRIME);
	REG_IF_SYM_EXIST("C_SVM.PAL_CITY_CRIME", &ISvm::PAL_CITY_CRIME);
	REG_IF_SYM_EXIST("C_SVM.MIL_CITY_CRIME", &ISvm::MIL_CITY_CRIME);
	REG_IF_SYM_EXIST("C_SVM.CITY_CRIME", &ISvm::CITY_CRIME);
	REG_IF_SYM_EXIST("C_SVM.MONA_CRIME", &ISvm::MONA_CRIME);
	REG_IF_SYM_EXIST("C_SVM.FARM_CRIME", &ISvm::FARM_CRIME);
	REG_IF_SYM_EXIST("C_SVM.OC_CRIME", &ISvm::OC_CRIME);
	REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_ATTACKLOST", &ISvm::TOUGHGUY_ATTACKLOST);
	REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_ATTACKWON", &ISvm::TOUGHGUY_ATTACKWON);
	REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_PLAYERATTACK", &ISvm::TOUGHGUY_PLAYERATTACK);
	REG_IF_SYM_EXIST("C_SVM.GOLD_1000", &ISvm::GOLD_1000);
	REG_IF_SYM_EXIST("C_SVM.GOLD_950", &ISvm::GOLD_950);
	REG_IF_SYM_EXIST("C_SVM.GOLD_900", &ISvm::GOLD_900);
	REG_IF_SYM_EXIST("C_SVM.GOLD_850", &ISvm::GOLD_850);
	REG_IF_SYM_EXIST("C_SVM.GOLD_800", &ISvm::GOLD_800);
	REG_IF_SYM_EXIST("C_SVM.GOLD_750", &ISvm::GOLD_750);
	REG_IF_SYM_EXIST("C_SVM.GOLD_700", &ISvm::GOLD_700);
	REG_IF_SYM_EXIST("C_SVM.GOLD_650", &ISvm::GOLD_650);
	REG_IF_SYM_EXIST("C_SVM.GOLD_600", &ISvm::GOLD_600);
	REG_IF_SYM_EXIST("C_SVM.GOLD_550", &ISvm::GOLD_550);
	REG_IF_SYM_EXIST("C_SVM.GOLD_500", &ISvm::GOLD_500);
	REG_IF_SYM_EXIST("C_SVM.GOLD_450", &ISvm::GOLD_450);
	REG_IF_SYM_EXIST("C_SVM.GOLD_400", &ISvm::GOLD_400);
	REG_IF_SYM_EXIST("C_SVM.GOLD_350", &ISvm::GOLD_350);
	REG_IF_SYM_EXIST("C_SVM.GOLD_300", &ISvm::GOLD_300);
	REG_IF_SYM_EXIST("C_SVM.GOLD_250", &ISvm::GOLD_250);
	REG_IF_SYM_EXIST("C_SVM.GOLD_200", &ISvm::GOLD_200);
	REG_IF_SYM_EXIST("C_SVM.GOLD_150", &ISvm::GOLD_150);
	REG_IF_SYM_EXIST("C_SVM.GOLD_100", &ISvm::GOLD_100);
	REG_IF_SYM_EXIST("C_SVM.GOLD_90", &ISvm::GOLD_90);
	REG_IF_SYM_EXIST("C_SVM.GOLD_80", &ISvm::GOLD_80);
	REG_IF_SYM_EXIST("C_SVM.GOLD_70", &ISvm::GOLD_70);
	REG_IF_SYM_EXIST("C_SVM.GOLD_60", &ISvm::GOLD_60);
	REG_IF_SYM_EXIST("C_SVM.GOLD_50", &ISvm::GOLD_50);
	REG_IF_SYM_EXIST("C_SVM.GOLD_40", &ISvm::GOLD_40);
	REG_IF_SYM_EXIST("C_SVM.GOLD_30", &ISvm::GOLD_30);
	REG_IF_SYM_EXIST("C_SVM.GOLD_20", &ISvm::GOLD_20);
	REG_IF_SYM_EXIST("C_SVM.GOLD_10", &ISvm::GOLD_10);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK01", &ISvm::SMALLTALK01);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK02", &ISvm::SMALLTALK02);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK03", &ISvm::SMALLTALK03);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK04", &ISvm::SMALLTALK04);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK05", &ISvm::SMALLTALK05);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK06", &ISvm::SMALLTALK06);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK07", &ISvm::SMALLTALK07);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK08", &ISvm::SMALLTALK08);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK09", &ISvm::SMALLTALK09);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK10", &ISvm::SMALLTALK10);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK11", &ISvm::SMALLTALK11);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK12", &ISvm::SMALLTALK12);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK13", &ISvm::SMALLTALK13);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK14", &ISvm::SMALLTALK14);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK15", &ISvm::SMALLTALK15);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK16", &ISvm::SMALLTALK16);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK17", &ISvm::SMALLTALK17);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK18", &ISvm::SMALLTALK18);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK19", &ISvm::SMALLTALK19);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK20", &ISvm::SMALLTALK20);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK21", &ISvm::SMALLTALK21);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK22", &ISvm::SMALLTALK22);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK23", &ISvm::SMALLTALK23);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK24", &ISvm::SMALLTALK24);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK25", &ISvm::SMALLTALK25);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK26", &ISvm::SMALLTALK26);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK27", &ISvm::SMALLTALK27);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK28", &ISvm::SMALLTALK28);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK29", &ISvm::SMALLTALK29);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK30", &ISvm::SMALLTALK30);
	REG_IF_SYM_EXIST("C_SVM.NOLEARNNOPOINTS", &ISvm::NOLEARNNOPOINTS);
	REG_IF_SYM_EXIST("C_SVM.NOLEARNOVERPERSONALMAX", &ISvm::NOLEARNOVERPERSONALMAX);
	REG_IF_SYM_EXIST("C_SVM.NOLEARNYOUREBETTER", &ISvm::NOLEARNYOUREBETTER);
	REG_IF_SYM_EXIST("C_SVM.YOULEARNEDSOMETHING", &ISvm::YOULEARNEDSOMETHING);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT", &ISvm::UNTERSTADT);
	REG_IF_SYM_EXIST("C_SVM.OBERSTADT", &ISvm::OBERSTADT);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL", &ISvm::TEMPEL);
	REG_IF_SYM_EXIST("C_SVM.MARKT", &ISvm::MARKT);
	REG_IF_SYM_EXIST("C_SVM.GALGEN", &ISvm::GALGEN);
	REG_IF_SYM_EXIST("C_SVM.KASERNE", &ISvm::KASERNE);
	REG_IF_SYM_EXIST("C_SVM.HAFEN", &ISvm::HAFEN);
	REG_IF_SYM_EXIST("C_SVM.WHERETO", &ISvm::WHERETO);
	REG_IF_SYM_EXIST("C_SVM.OBERSTADT_2_UNTERSTADT", &ISvm::OBERSTADT_2_UNTERSTADT);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_OBERSTADT", &ISvm::UNTERSTADT_2_OBERSTADT);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_TEMPEL", &ISvm::UNTERSTADT_2_TEMPEL);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_HAFEN", &ISvm::UNTERSTADT_2_HAFEN);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_UNTERSTADT", &ISvm::TEMPEL_2_UNTERSTADT);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_MARKT", &ISvm::TEMPEL_2_MARKT);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_GALGEN", &ISvm::TEMPEL_2_GALGEN);
	REG_IF_SYM_EXIST("C_SVM.MARKT_2_TEMPEL", &ISvm::MARKT_2_TEMPEL);
	REG_IF_SYM_EXIST("C_SVM.MARKT_2_KASERNE", &ISvm::MARKT_2_KASERNE);
	REG_IF_SYM_EXIST("C_SVM.MARKT_2_GALGEN", &ISvm::MARKT_2_GALGEN);
	REG_IF_SYM_EXIST("C_SVM.GALGEN_2_TEMPEL", &ISvm::GALGEN_2_TEMPEL);
	REG_IF_SYM_EXIST("C_SVM.GALGEN_2_MARKT", &ISvm::GALGEN_2_MARKT);
	REG_IF_SYM_EXIST("C_SVM.GALGEN_2_KASERNE", &ISvm::GALGEN_2_KASERNE);
	REG_IF_SYM_EXIST("C_SVM.KASERNE_2_MARKT", &ISvm::KASERNE_2_MARKT);
	REG_IF_SYM_EXIST("C_SVM.KASERNE_2_GALGEN", &ISvm::KASERNE_2_GALGEN);
	REG_IF_SYM_EXIST("C_SVM.HAFEN_2_UNTERSTADT", &ISvm::HAFEN_2_UNTERSTADT);
	REG_IF_SYM_EXIST("C_SVM.DEAD", &ISvm::DEAD);
	REG_IF_SYM_EXIST("C_SVM.AARGH_1", &ISvm::AARGH_1);
	REG_IF_SYM_EXIST("C_SVM.AARGH_2", &ISvm::AARGH_2);
	REG_IF_SYM_EXIST("C_SVM.AARGH_3", &ISvm::AARGH_3);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR", &ISvm::ADDON_WRONGARMOR);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_SLD", &ISvm::ADDON_WRONGARMOR_SLD);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_MIL", &ISvm::ADDON_WRONGARMOR_MIL);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_KDF", &ISvm::ADDON_WRONGARMOR_KDF);
	REG_IF_SYM_EXIST("C_SVM.ADDON_NOARMOR_BDT", &ISvm::ADDON_NOARMOR_BDT);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIEBANDIT", &ISvm::ADDON_DIEBANDIT);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIRTYPIRATE", &ISvm::ADDON_DIRTYPIRATE);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND", &ISvm::SC_HEYTURNAROUND);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND02", &ISvm::SC_HEYTURNAROUND02);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND03", &ISvm::SC_HEYTURNAROUND03);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND04", &ISvm::SC_HEYTURNAROUND04);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYWAITASECOND", &ISvm::SC_HEYWAITASECOND);
	REG_IF_SYM_EXIST("C_SVM.DOESNTWORK", &ISvm::DOESNTWORK);
	REG_IF_SYM_EXIST("C_SVM.PICKBROKE", &ISvm::PICKBROKE);
	REG_IF_SYM_EXIST("C_SVM.NEEDKEY", &ISvm::NEEDKEY);
	REG_IF_SYM_EXIST("C_SVM.NOMOREPICKS", &ISvm::NOMOREPICKS);
	REG_IF_SYM_EXIST("C_SVM.NOPICKLOCKTALENT", &ISvm::NOPICKLOCKTALENT);
	REG_IF_SYM_EXIST("C_SVM.NOSWEEPING", &ISvm::NOSWEEPING);
	REG_IF_SYM_EXIST("C_SVM.PICKLOCKORKEYMISSING", &ISvm::PICKLOCKORKEYMISSING);
	REG_IF_SYM_EXIST("C_SVM.KEYMISSING", &ISvm::KEYMISSING);
	REG_IF_SYM_EXIST("C_SVM.PICKLOCKMISSING", &ISvm::PICKLOCKMISSING);
	REG_IF_SYM_EXIST("C_SVM.NEVEROPEN", &ISvm::NEVEROPEN);
	REG_IF_SYM_EXIST("C_SVM.MISSINGITEM", &ISvm::MISSINGITEM);
	REG_IF_SYM_EXIST("C_SVM.DONTKNOW", &ISvm::DONTKNOW);
	REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET", &ISvm::NOTHINGTOGET);
	REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET02", &ISvm::NOTHINGTOGET02);
	REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET03", &ISvm::NOTHINGTOGET03);
	REG_IF_SYM_EXIST("C_SVM.HEALSHRINE", &ISvm::HEALSHRINE);
	REG_IF_SYM_EXIST("C_SVM.HEALLASTSHRINE", &ISvm::HEALLASTSHRINE);
	REG_IF_SYM_EXIST("C_SVM.IRDORATHTHEREYOUARE", &ISvm::IRDORATHTHEREYOUARE);
	REG_IF_SYM_EXIST("C_SVM.SCOPENSIRDORATHBOOK", &ISvm::SCOPENSIRDORATHBOOK);
	REG_IF_SYM_EXIST("C_SVM.SCOPENSLASTDOOR", &ISvm::SCOPENSLASTDOOR);
	REG_IF_SYM_EXIST("C_SVM.TRADE_1", &ISvm::TRADE_1);
	REG_IF_SYM_EXIST("C_SVM.TRADE_2", &ISvm::TRADE_2);
	REG_IF_SYM_EXIST("C_SVM.TRADE_3", &ISvm::TRADE_3);
	REG_IF_SYM_EXIST("C_SVM.VERSTEHE", &ISvm::VERSTEHE);
	REG_IF_SYM_EXIST("C_SVM.FOUNDTREASURE", &ISvm::FOUNDTREASURE);
	REG_IF_SYM_EXIST("C_SVM.CANTUNDERSTANDTHIS", &ISvm::CANTUNDERSTANDTHIS);
	REG_IF_SYM_EXIST("C_SVM.CANTREADTHIS", &ISvm::CANTREADTHIS);
	REG_IF_SYM_EXIST("C_SVM.STONEPLATE_1", &ISvm::STONEPLATE_1);
	REG_IF_SYM_EXIST("C_SVM.STONEPLATE_2", &ISvm::STONEPLATE_2);
	REG_IF_SYM_EXIST("C_SVM.STONEPLATE_3", &ISvm::STONEPLATE_3);
	REG_IF_SYM_EXIST("C_SVM.COUGH", &ISvm::COUGH);
	REG_IF_SYM_EXIST("C_SVM.HUI", &ISvm::HUI);
	REG_IF_SYM_EXIST("C_SVM.ADDON_THISLITTLEBASTARD", &ISvm::ADDON_THISLITTLEBASTARD);
	REG_IF_SYM_EXIST("C_SVM.ADDON_OPENADANOSTEMPLE", &ISvm::ADDON_OPENADANOSTEMPLE);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_DESCRIPTION", &ISvm::ATTENTAT_ADDON_DESCRIPTION);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_DESCRIPTION2", &ISvm::ATTENTAT_ADDON_DESCRIPTION2);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_PRO", &ISvm::ATTENTAT_ADDON_PRO);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_CONTRA", &ISvm::ATTENTAT_ADDON_CONTRA);
	REG_IF_SYM_EXIST("C_SVM.MINE_ADDON_DESCRIPTION", &ISvm::MINE_ADDON_DESCRIPTION);
	REG_IF_SYM_EXIST("C_SVM.ADDON_SUMMONANCIENTGHOST", &ISvm::ADDON_SUMMONANCIENTGHOST);
	REG_IF_SYM_EXIST("C_SVM.ADDON_ANCIENTGHOST_NOTNEAR", &ISvm::ADDON_ANCIENTGHOST_NOTNEAR);
	REG_IF_SYM_EXIST("C_SVM.ADDON_GOLD_DESCRIPTION", &ISvm::ADDON_GOLD_DESCRIPTION);

	REG_IF_SYM_EXIST("C_SVM.WATCHYOURAIM", &ISvm::WATCHYOURAIM);
	REG_IF_SYM_EXIST("C_SVM.watchyouraimangry", &ISvm::watchyouraimangry);
	REG_IF_SYM_EXIST("C_SVM.letsforgetourlittlefight", &ISvm::letsforgetourlittlefight);
	REG_IF_SYM_EXIST("C_SVM.strange", &ISvm::strange);
	REG_IF_SYM_EXIST("C_SVM.diemortalenemy", &ISvm::diemortalenemy);
	REG_IF_SYM_EXIST("C_SVM.nowwait", &ISvm::nowwait);
	REG_IF_SYM_EXIST("C_SVM.nowwaitintruder", &ISvm::nowwaitintruder);
	REG_IF_SYM_EXIST("C_SVM.youstillnothaveenough", &ISvm::youstillnothaveenough);
	REG_IF_SYM_EXIST("C_SVM.youattackedmycharge", &ISvm::youattackedmycharge);
	REG_IF_SYM_EXIST("C_SVM.iwillteachyourespectforforeignproperty", &ISvm::iwillteachyourespectforforeignproperty);
	REG_IF_SYM_EXIST("C_SVM.youkilledoneofus", &ISvm::youkilledoneofus);
	REG_IF_SYM_EXIST("C_SVM.berzerk", &ISvm::berzerk);
	REG_IF_SYM_EXIST("C_SVM.youllbesorryforthis", &ISvm::youllbesorryforthis);
	REG_IF_SYM_EXIST("C_SVM.yesyes", &ISvm::yesyes);
	REG_IF_SYM_EXIST("C_SVM.shitwhatamonster", &ISvm::shitwhatamonster);
	REG_IF_SYM_EXIST("C_SVM.wewillmeetagain", &ISvm::wewillmeetagain);
	REG_IF_SYM_EXIST("C_SVM.nevertrythatagain", &ISvm::nevertrythatagain);
	REG_IF_SYM_EXIST("C_SVM.itookyourore", &ISvm::itookyourore);
	REG_IF_SYM_EXIST("C_SVM.shitnoore", &ISvm::shitnoore);
	REG_IF_SYM_EXIST("C_SVM.youviolatedforbiddenterritory", &ISvm::youviolatedforbiddenterritory);
	REG_IF_SYM_EXIST("C_SVM.youwannafoolme", &ISvm::youwannafoolme);
	REG_IF_SYM_EXIST("C_SVM.whatdidyouinthere", &ISvm::whatdidyouinthere);
	REG_IF_SYM_EXIST("C_SVM.intruderalert", &ISvm::intruderalert);
	REG_IF_SYM_EXIST("C_SVM.behindyou", &ISvm::behindyou);
	REG_IF_SYM_EXIST("C_SVM.heyheyhey", &ISvm::heyheyhey);
	REG_IF_SYM_EXIST("C_SVM.cheerfight", &ISvm::cheerfight);
	REG_IF_SYM_EXIST("C_SVM.cheerfriend", &ISvm::cheerfriend);
	REG_IF_SYM_EXIST("C_SVM.ooh", &ISvm::ooh);
	REG_IF_SYM_EXIST("C_SVM.yeahwelldone", &ISvm::yeahwelldone);
	REG_IF_SYM_EXIST("C_SVM.hedefeatedhim", &ISvm::hedefeatedhim);
	REG_IF_SYM_EXIST("C_SVM.hedeservedit", &ISvm::hedeservedit);
	REG_IF_SYM_EXIST("C_SVM.hekilledhim", &ISvm::hekilledhim);
	REG_IF_SYM_EXIST("C_SVM.itwasagoodfight", &ISvm::itwasagoodfight);
	REG_IF_SYM_EXIST("C_SVM.friendlygreetings", &ISvm::friendlygreetings);
	REG_IF_SYM_EXIST("C_SVM.algreetings", &ISvm::algreetings);
	REG_IF_SYM_EXIST("C_SVM.magegreetings", &ISvm::magegreetings);
	REG_IF_SYM_EXIST("C_SVM.sectgreetings", &ISvm::sectgreetings);
	REG_IF_SYM_EXIST("C_SVM.thereheis", &ISvm::thereheis);
	REG_IF_SYM_EXIST("C_SVM.nolearnovermax", &ISvm::nolearnovermax);
	REG_IF_SYM_EXIST("C_SVM.nolearnyoualreadyknow", &ISvm::nolearnyoualreadyknow);
	REG_IF_SYM_EXIST("C_SVM.heyyou", &ISvm::heyyou);
	REG_IF_SYM_EXIST("C_SVM.whatdoyouwant", &ISvm::whatdoyouwant);
	REG_IF_SYM_EXIST("C_SVM.isaidwhatdoyouwant", &ISvm::isaidwhatdoyouwant);
	REG_IF_SYM_EXIST("C_SVM.makeway", &ISvm::makeway);
	REG_IF_SYM_EXIST("C_SVM.outofmyway", &ISvm::outofmyway);
	REG_IF_SYM_EXIST("C_SVM.youdeaforwhat", &ISvm::youdeaforwhat);
	REG_IF_SYM_EXIST("C_SVM.lookaway", &ISvm::lookaway);
	REG_IF_SYM_EXIST("C_SVM.okaykeepit", &ISvm::okaykeepit);
	REG_IF_SYM_EXIST("C_SVM.whatsthat", &ISvm::whatsthat);
	REG_IF_SYM_EXIST("C_SVM.thatsmyweapon", &ISvm::thatsmyweapon);
	REG_IF_SYM_EXIST("C_SVM.giveittome", &ISvm::giveittome);
	REG_IF_SYM_EXIST("C_SVM.youcankeepthecrap", &ISvm::youcankeepthecrap);
	REG_IF_SYM_EXIST("C_SVM.theykilledmyfriend", &ISvm::theykilledmyfriend);
	REG_IF_SYM_EXIST("C_SVM.suckergotsome", &ISvm::suckergotsome);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedebr", &ISvm::suckerdefeatedebr);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedgur", &ISvm::suckerdefeatedgur);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedmage", &ISvm::suckerdefeatedmage);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatednov_guard", &ISvm::suckerdefeatednov_guard);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedvlk_guard", &ISvm::suckerdefeatedvlk_guard);
	REG_IF_SYM_EXIST("C_SVM.youdefeatedmycomrade", &ISvm::youdefeatedmycomrade);
	REG_IF_SYM_EXIST("C_SVM.youdefeatednov_guard", &ISvm::youdefeatednov_guard);
	REG_IF_SYM_EXIST("C_SVM.youdefeatedvlk_guard", &ISvm::youdefeatedvlk_guard);
	REG_IF_SYM_EXIST("C_SVM.youstolefromme", &ISvm::youstolefromme);
	REG_IF_SYM_EXIST("C_SVM.youstolefromus", &ISvm::youstolefromus);
	REG_IF_SYM_EXIST("C_SVM.youstolefromebr", &ISvm::youstolefromebr);
	REG_IF_SYM_EXIST("C_SVM.youstolefromgur", &ISvm::youstolefromgur);
	REG_IF_SYM_EXIST("C_SVM.stolefrommage", &ISvm::stolefrommage);
	REG_IF_SYM_EXIST("C_SVM.youkilledmyfriend", &ISvm::youkilledmyfriend);
	REG_IF_SYM_EXIST("C_SVM.youkilledebr", &ISvm::youkilledebr);
	REG_IF_SYM_EXIST("C_SVM.youkilledgur", &ISvm::youkilledgur);
	REG_IF_SYM_EXIST("C_SVM.youkilledmage", &ISvm::youkilledmage);
	REG_IF_SYM_EXIST("C_SVM.youkilledocfolk", &ISvm::youkilledocfolk);
	REG_IF_SYM_EXIST("C_SVM.youkilledncfolk", &ISvm::youkilledncfolk);
	REG_IF_SYM_EXIST("C_SVM.youkilledpsifolk", &ISvm::youkilledpsifolk);
	REG_IF_SYM_EXIST("C_SVM.getthingsright", &ISvm::getthingsright);
	REG_IF_SYM_EXIST("C_SVM.youdefeatedmewell", &ISvm::youdefeatedmewell);
	REG_IF_SYM_EXIST("C_SVM.om", &ISvm::om);
}

#undef REG_IF_SYM_EXIST

void zenkit::IMenu::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_MENU", "IMenu");
	s.register_member("C_MENU.BACKPIC", &IMenu::back_pic);
	s.register_member("C_MENU.BACKWORLD", &IMenu::back_world);
	s.register_member("C_MENU.POSX", &IMenu::pos_x);
	s.register_member("C_MENU.POSY", &IMenu::pos_y);
	s.register_member("C_MENU.DIMX", &IMenu::dim_x);
	s.register_member("C_MENU.DIMY", &IMenu::dim_y);
	s.register_member("C_MENU.ALPHA", &IMenu::alpha);
	s.register_member("C_MENU.MUSICTHEME", &IMenu::music_theme);
	s.register_member("C_MENU.EVENTTIMERMSEC", &IMenu::event_timer_msec);
	s.register_member("C_MENU.ITEMS", &IMenu::items);
	s.register_member("C_MENU.FLAGS", &IMenu::flags);
	s.register_member("C_MENU.DEFAULTOUTGAME", &IMenu::default_outgame);
	s.register_member("C_MENU.DEFAULTINGAME", &IMenu::default_ingame);
}

void zenkit::IMenuItem::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_MENU_ITEM", "IMenuItem");
	s.register_member("C_MENU_ITEM.FONTNAME", &IMenuItem::fontname);
	s.register_member("C_MENU_ITEM.TEXT", &IMenuItem::text);
	s.register_member("C_MENU_ITEM.BACKPIC", &IMenuItem::backpic);
	s.register_member("C_MENU_ITEM.ALPHAMODE", &IMenuItem::alphamode);
	s.register_member("C_MENU_ITEM.ALPHA", &IMenuItem::alpha);
	s.register_member("C_MENU_ITEM.TYPE", &IMenuItem::type);
	s.register_member("C_MENU_ITEM.ONSELACTION", &IMenuItem::on_sel_action);
	s.register_member("C_MENU_ITEM.ONSELACTION_S", &IMenuItem::on_sel_action_s);
	s.register_member("C_MENU_ITEM.ONCHGSETOPTION", &IMenuItem::on_chg_set_option);
	s.register_member("C_MENU_ITEM.ONCHGSETOPTIONSECTION", &IMenuItem::on_chg_set_option_section);
	s.register_member("C_MENU_ITEM.ONEVENTACTION", &IMenuItem::on_event_action);
	s.register_member("C_MENU_ITEM.POSX", &IMenuItem::pos_x);
	s.register_member("C_MENU_ITEM.POSY", &IMenuItem::pos_y);
	s.register_member("C_MENU_ITEM.DIMX", &IMenuItem::dim_x);
	s.register_member("C_MENU_ITEM.DIMY", &IMenuItem::dim_y);
	s.register_member("C_MENU_ITEM.SIZESTARTSCALE", &IMenuItem::size_start_scale);
	s.register_member("C_MENU_ITEM.FLAGS", &IMenuItem::flags);
	s.register_member("C_MENU_ITEM.OPENDELAYTIME", &IMenuItem::open_delay_time);
	s.register_member("C_MENU_ITEM.OPENDURATION", &IMenuItem::open_duration);
	s.register_member("C_MENU_ITEM.USERFLOAT", &IMenuItem::user_float);
	s.register_member("C_MENU_ITEM.USERSTRING", &IMenuItem::user_string);
	s.register_member("C_MENU_ITEM.FRAMESIZEX", &IMenuItem::frame_sizex);
	s.register_member("C_MENU_ITEM.FRAMESIZEY", &IMenuItem::frame_sizey);

	// switch
	if (s.find_symbol_by_name("C_MENU_ITEM.FRAMEPOSX") != nullptr) {
		s.register_member("C_MENU_ITEM.FRAMEPOSX", &IMenuItem::frame_posx);
		s.register_member("C_MENU_ITEM.FRAMEPOSY", &IMenuItem::frame_posy);
	}

	// Gothic 2 only
	if (s.find_symbol_by_name("C_MENU_ITEM.HIDEIFOPTIONSECTIONSET") != nullptr) {
		s.register_member("C_MENU_ITEM.HIDEIFOPTIONSECTIONSET", &IMenuItem::hide_if_option_section_set);
		s.register_member("C_MENU_ITEM.HIDEIFOPTIONSET", &IMenuItem::hide_if_option_set);
		s.register_member("C_MENU_ITEM.HIDEONVALUE", &IMenuItem::hide_on_value);
	}
}

void zenkit::ICamera::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("CCAMSYS", "ICamera");
	s.register_member("CCAMSYS.BESTRANGE", &ICamera::best_range);
	s.register_member("CCAMSYS.MINRANGE", &ICamera::min_range);
	s.register_member("CCAMSYS.MAXRANGE", &ICamera::max_range);
	s.register_member("CCAMSYS.BESTELEVATION", &ICamera::best_elevation);
	s.register_member("CCAMSYS.MINELEVATION", &ICamera::min_elevation);
	s.register_member("CCAMSYS.MAXELEVATION", &ICamera::max_elevation);
	s.register_member("CCAMSYS.BESTAZIMUTH", &ICamera::best_azimuth);
	s.register_member("CCAMSYS.MINAZIMUTH", &ICamera::min_azimuth);
	s.register_member("CCAMSYS.MAXAZIMUTH", &ICamera::max_azimuth);
	s.register_member("CCAMSYS.BESTROTZ", &ICamera::best_rot_z);
	s.register_member("CCAMSYS.MINROTZ", &ICamera::min_rot_z);
	s.register_member("CCAMSYS.MAXROTZ", &ICamera::max_rot_z);
	s.register_member("CCAMSYS.ROTOFFSETX", &ICamera::rot_offset_x);
	s.register_member("CCAMSYS.ROTOFFSETY", &ICamera::rot_offset_y);
	s.register_member("CCAMSYS.ROTOFFSETZ", &ICamera::rot_offset_z);
	s.register_member("CCAMSYS.TARGETOFFSETX", &ICamera::target_offset_x);
	s.register_member("CCAMSYS.TARGETOFFSETY", &ICamera::target_offset_y);
	s.register_member("CCAMSYS.TARGETOFFSETZ", &ICamera::target_offset_z);
	s.register_member("CCAMSYS.VELOTRANS", &ICamera::velo_trans);
	s.register_member("CCAMSYS.VELOROT", &ICamera::velo_rot);
	s.register_member("CCAMSYS.TRANSLATE", &ICamera::translate);
	s.register_member("CCAMSYS.ROTATE", &ICamera::rotate);
	s.register_member("CCAMSYS.COLLISION", &ICamera::collision);
}

void zenkit::IMusicSystem::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_MUSICSYS_CFG", "IMusicSystem");
	s.register_member("C_MUSICSYS_CFG.VOLUME", &IMusicSystem::volume);
	s.register_member("C_MUSICSYS_CFG.BITRESOLUTION", &IMusicSystem::bit_resolution);
	s.register_member("C_MUSICSYS_CFG.GLOBALREVERBENABLED", &IMusicSystem::global_reverb_enabled);
	s.register_member("C_MUSICSYS_CFG.SAMPLERATE", &IMusicSystem::sample_rate);
	s.register_member("C_MUSICSYS_CFG.NUMCHANNELS", &IMusicSystem::num_channels);
	s.register_member("C_MUSICSYS_CFG.REVERBBUFFERSIZE", &IMusicSystem::reverb_buffer_size);
}

void zenkit::IMusicTheme::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_MUSICTHEME", "IMusicTheme");
	s.register_member("C_MUSICTHEME.FILE", &IMusicTheme::file);
	s.register_member("C_MUSICTHEME.VOL", &IMusicTheme::vol);
	s.register_member("C_MUSICTHEME.LOOP", &IMusicTheme::loop);
	s.register_member("C_MUSICTHEME.REVERBMIX", &IMusicTheme::reverbmix);
	s.register_member("C_MUSICTHEME.REVERBTIME", &IMusicTheme::reverbtime);
	s.register_member("C_MUSICTHEME.TRANSTYPE", &IMusicTheme::transtype);
	s.register_member("C_MUSICTHEME.TRANSSUBTYPE", &IMusicTheme::transsubtype);
}

void zenkit::IMusicJingle::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_MUSICJINGLE", "IMusicJingle");
	s.register_member("C_MUSICJINGLE.NAME", &IMusicJingle::name);
	s.register_member("C_MUSICJINGLE.LOOP", &IMusicJingle::loop);
	s.register_member("C_MUSICJINGLE.VOL", &IMusicJingle::vol);
	s.register_member("C_MUSICJINGLE.TRANSSUBTYPE", &IMusicJingle::transsubtype);
}

void zenkit::IParticleEffect::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("C_PARTICLEFX", "IParticleEffect");
	s.register_member("C_PARTICLEFX.PPSVALUE", &IParticleEffect::pps_value);
	s.register_member("C_PARTICLEFX.PPSSCALEKEYS_S", &IParticleEffect::pps_scale_keys_s);
	s.register_member("C_PARTICLEFX.PPSISLOOPING", &IParticleEffect::pps_is_looping);
	s.register_member("C_PARTICLEFX.PPSISSMOOTH", &IParticleEffect::pps_is_smooth);
	s.register_member("C_PARTICLEFX.PPSFPS", &IParticleEffect::pps_fps);
	s.register_member("C_PARTICLEFX.PPSCREATEEM_S", &IParticleEffect::pps_create_em_s);
	s.register_member("C_PARTICLEFX.PPSCREATEEMDELAY", &IParticleEffect::pps_create_em_delay);
	s.register_member("C_PARTICLEFX.SHPTYPE_S", &IParticleEffect::shp_type_s);
	s.register_member("C_PARTICLEFX.SHPFOR_S", &IParticleEffect::shp_for_s);
	s.register_member("C_PARTICLEFX.SHPOFFSETVEC_S", &IParticleEffect::shp_offset_vec_s);
	s.register_member("C_PARTICLEFX.SHPDISTRIBTYPE_S", &IParticleEffect::shp_distrib_type_s);
	s.register_member("C_PARTICLEFX.SHPDISTRIBWALKSPEED", &IParticleEffect::shp_distrib_walk_speed);
	s.register_member("C_PARTICLEFX.SHPISVOLUME", &IParticleEffect::shp_is_volume);
	s.register_member("C_PARTICLEFX.SHPDIM_S", &IParticleEffect::shp_dim_s);
	s.register_member("C_PARTICLEFX.SHPMESH_S", &IParticleEffect::shp_mesh_s);
	s.register_member("C_PARTICLEFX.SHPMESHRENDER_B", &IParticleEffect::shp_mesh_render_b);
	s.register_member("C_PARTICLEFX.SHPSCALEKEYS_S", &IParticleEffect::shp_scale_keys_s);
	s.register_member("C_PARTICLEFX.SHPSCALEISLOOPING", &IParticleEffect::shp_scale_is_looping);
	s.register_member("C_PARTICLEFX.SHPSCALEISSMOOTH", &IParticleEffect::shp_scale_is_smooth);
	s.register_member("C_PARTICLEFX.SHPSCALEFPS", &IParticleEffect::shp_scale_fps);
	s.register_member("C_PARTICLEFX.DIRMODE_S", &IParticleEffect::dir_mode_s);
	s.register_member("C_PARTICLEFX.DIRFOR_S", &IParticleEffect::dir_for_s);
	s.register_member("C_PARTICLEFX.DIRMODETARGETFOR_S", &IParticleEffect::dir_mode_target_for_s);
	s.register_member("C_PARTICLEFX.DIRMODETARGETPOS_S", &IParticleEffect::dir_mode_target_pos_s);
	s.register_member("C_PARTICLEFX.DIRANGLEHEAD", &IParticleEffect::dir_angle_head);
	s.register_member("C_PARTICLEFX.DIRANGLEHEADVAR", &IParticleEffect::dir_angle_head_var);
	s.register_member("C_PARTICLEFX.DIRANGLEELEV", &IParticleEffect::dir_angle_elev);
	s.register_member("C_PARTICLEFX.DIRANGLEELEVVAR", &IParticleEffect::dir_angle_elev_var);
	s.register_member("C_PARTICLEFX.VELAVG", &IParticleEffect::vel_avg);
	s.register_member("C_PARTICLEFX.VELVAR", &IParticleEffect::vel_var);
	s.register_member("C_PARTICLEFX.LSPPARTAVG", &IParticleEffect::lsp_part_avg);
	s.register_member("C_PARTICLEFX.LSPPARTVAR", &IParticleEffect::lsp_part_var);
	s.register_member("C_PARTICLEFX.FLYGRAVITY_S", &IParticleEffect::fly_gravity_s);
	s.register_member("C_PARTICLEFX.FLYCOLLDET_B", &IParticleEffect::fly_colldet_b);
	s.register_member("C_PARTICLEFX.VISNAME_S", &IParticleEffect::vis_name_s);
	s.register_member("C_PARTICLEFX.VISORIENTATION_S", &IParticleEffect::vis_orientation_s);
	s.register_member("C_PARTICLEFX.VISTEXISQUADPOLY", &IParticleEffect::vis_tex_is_quadpoly);
	s.register_member("C_PARTICLEFX.VISTEXANIFPS", &IParticleEffect::vis_tex_ani_fps);
	s.register_member("C_PARTICLEFX.VISTEXANIISLOOPING", &IParticleEffect::vis_tex_ani_is_looping);
	s.register_member("C_PARTICLEFX.VISTEXCOLORSTART_S", &IParticleEffect::vis_tex_color_start_s);
	s.register_member("C_PARTICLEFX.VISTEXCOLOREND_S", &IParticleEffect::vis_tex_color_end_s);
	s.register_member("C_PARTICLEFX.VISSIZESTART_S", &IParticleEffect::vis_size_start_s);
	s.register_member("C_PARTICLEFX.VISSIZEENDSCALE", &IParticleEffect::vis_size_end_scale);
	s.register_member("C_PARTICLEFX.VISALPHAFUNC_S", &IParticleEffect::vis_alpha_func_s);
	s.register_member("C_PARTICLEFX.VISALPHASTART", &IParticleEffect::vis_alpha_start);
	s.register_member("C_PARTICLEFX.VISALPHAEND", &IParticleEffect::vis_alpha_end);
	s.register_member("C_PARTICLEFX.TRLFADESPEED", &IParticleEffect::trl_fade_speed);
	s.register_member("C_PARTICLEFX.TRLTEXTURE_S", &IParticleEffect::trl_texture_s);
	s.register_member("C_PARTICLEFX.TRLWIDTH", &IParticleEffect::trl_width);
	s.register_member("C_PARTICLEFX.MRKFADESPEED", &IParticleEffect::mrk_fades_peed);
	s.register_member("C_PARTICLEFX.MRKTEXTURE_S", &IParticleEffect::mrkt_exture_s);
	s.register_member("C_PARTICLEFX.MRKSIZE", &IParticleEffect::mrk_size);

	// Gothic 2 only
	if (s.find_symbol_by_name("C_PARTICLEFX.FLOCKMODE") != nullptr) {
		s.register_member("C_PARTICLEFX.FLOCKMODE", &IParticleEffect::flock_mode);
		s.register_member("C_PARTICLEFX.FLOCKSTRENGTH", &IParticleEffect::flock_strength);
		s.register_member("C_PARTICLEFX.USEEMITTERSFOR", &IParticleEffect::use_emitters_for);
		s.register_member("C_PARTICLEFX.TIMESTARTEND_S", &IParticleEffect::time_start_end_s);
		s.register_member("C_PARTICLEFX.M_BISAMBIENTPFX", &IParticleEffect::m_bis_ambient_pfx);
	}
}

void zenkit::IEffectBase::register_(zenkit::DaedalusScript& s) {
	ZKLOG_CLASS("CFX_BASE", "IEffectBase");
	s.register_member("CFX_BASE.VISNAME_S", &IEffectBase::vis_name_s);
	s.register_member("CFX_BASE.VISSIZE_S", &IEffectBase::vis_size_s);
	s.register_member("CFX_BASE.VISALPHA", &IEffectBase::vis_alpha);
	s.register_member("CFX_BASE.VISALPHABLENDFUNC_S", &IEffectBase::vis_alpha_blend_func_s);
	s.register_member("CFX_BASE.VISTEXANIFPS", &IEffectBase::vis_tex_ani_fps);
	s.register_member("CFX_BASE.VISTEXANIISLOOPING", &IEffectBase::vis_tex_ani_is_looping);
	s.register_member("CFX_BASE.EMTRJMODE_S", &IEffectBase::em_trj_mode_s);
	s.register_member("CFX_BASE.EMTRJORIGINNODE", &IEffectBase::em_trj_origin_node);
	s.register_member("CFX_BASE.EMTRJTARGETNODE", &IEffectBase::em_trj_target_node);
	s.register_member("CFX_BASE.EMTRJTARGETRANGE", &IEffectBase::em_trj_target_range);
	s.register_member("CFX_BASE.EMTRJTARGETAZI", &IEffectBase::em_trj_target_azi);
	s.register_member("CFX_BASE.EMTRJTARGETELEV", &IEffectBase::em_trj_target_elev);
	s.register_member("CFX_BASE.EMTRJNUMKEYS", &IEffectBase::em_trj_num_keys);
	s.register_member("CFX_BASE.EMTRJNUMKEYSVAR", &IEffectBase::em_trj_num_keys_var);
	s.register_member("CFX_BASE.EMTRJANGLEELEVVAR", &IEffectBase::em_trj_angle_elev_var);
	s.register_member("CFX_BASE.EMTRJANGLEHEADVAR", &IEffectBase::em_trj_angle_head_var);
	s.register_member("CFX_BASE.EMTRJKEYDISTVAR", &IEffectBase::em_trj_key_dist_var);
	s.register_member("CFX_BASE.EMTRJLOOPMODE_S", &IEffectBase::em_trj_loop_mode_s);
	s.register_member("CFX_BASE.EMTRJEASEFUNC_S", &IEffectBase::em_trj_ease_func_s);
	s.register_member("CFX_BASE.EMTRJEASEVEL", &IEffectBase::em_trj_ease_vel);
	s.register_member("CFX_BASE.EMTRJDYNUPDATEDELAY", &IEffectBase::em_trj_dyn_update_delay);
	s.register_member("CFX_BASE.EMTRJDYNUPDATETARGETONLY", &IEffectBase::em_trj_dyn_update_target_only);
	s.register_member("CFX_BASE.EMFXCREATE_S", &IEffectBase::em_fx_create_s);
	s.register_member("CFX_BASE.EMFXINVESTORIGIN_S", &IEffectBase::em_fx_invest_origin_s);
	s.register_member("CFX_BASE.EMFXINVESTTARGET_S", &IEffectBase::em_fx_invest_target_s);
	s.register_member("CFX_BASE.EMFXTRIGGERDELAY", &IEffectBase::em_fx_trigger_delay);
	s.register_member("CFX_BASE.EMFXCREATEDOWNTRJ", &IEffectBase::em_fx_create_down_trj);
	s.register_member("CFX_BASE.EMACTIONCOLLDYN_S", &IEffectBase::em_action_coll_dyn_s);
	s.register_member("CFX_BASE.EMACTIONCOLLSTAT_S", &IEffectBase::em_action_coll_stat_s);
	s.register_member("CFX_BASE.EMFXCOLLSTAT_S", &IEffectBase::em_fx_coll_stat_s);
	s.register_member("CFX_BASE.EMFXCOLLDYN_S", &IEffectBase::em_fx_coll_dyn_s);
	s.register_member("CFX_BASE.EMFXCOLLSTATALIGN_S", &IEffectBase::em_fx_coll_stat_align_s);
	s.register_member("CFX_BASE.EMFXCOLLDYNALIGN_S", &IEffectBase::em_fx_coll_dyn_align_s);
	s.register_member("CFX_BASE.EMFXLIFESPAN", &IEffectBase::em_fx_lifespan);
	s.register_member("CFX_BASE.EMCHECKCOLLISION", &IEffectBase::em_check_collision);
	s.register_member("CFX_BASE.EMADJUSTSHPTOORIGIN", &IEffectBase::em_adjust_shp_to_origin);
	s.register_member("CFX_BASE.EMINVESTNEXTKEYDURATION", &IEffectBase::em_invest_next_key_duration);
	s.register_member("CFX_BASE.EMFLYGRAVITY", &IEffectBase::em_fly_gravity);
	s.register_member("CFX_BASE.EMSELFROTVEL_S", &IEffectBase::em_self_rot_vel_s);
	s.register_member("CFX_BASE.USERSTRING", &IEffectBase::user_string);
	s.register_member("CFX_BASE.LIGHTPRESETNAME", &IEffectBase::light_preset_name);
	s.register_member("CFX_BASE.SFXID", &IEffectBase::sfx_id);
	s.register_member("CFX_BASE.SFXISAMBIENT", &IEffectBase::sfx_is_ambient);
	s.register_member("CFX_BASE.SENDASSESSMAGIC", &IEffectBase::send_assess_magic);
	s.register_member("CFX_BASE.SECSPERDAMAGE", &IEffectBase::secs_per_damage);

	// Gothic 2 only
	if (s.find_symbol_by_name("CFX_BASE.EMFXCOLLDYNPERC_S") != nullptr)
		s.register_member("CFX_BASE.EMFXCOLLDYNPERC_S", &IEffectBase::em_fx_coll_dyn_perc_s);
}
