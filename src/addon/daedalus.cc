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

void zenkit::IGuildValues::register_(DaedalusScript& s) {
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

void zenkit::IFightAi::register_(DaedalusScript& s) {
	ZKLOG_CLASS("C_FIGHTAI", "IFightAi");
	s.register_member("C_FIGHTAI.MOVE", &IFightAi::move);
}

void zenkit::ISoundEffect::register_(DaedalusScript& s) {
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

void zenkit::ISoundSystem::register_(DaedalusScript& s) {
	ZKLOG_CLASS("C_SNDSYS_CFG", "ISoundSystem");
	s.register_member("C_SNDSYS_CFG.VOLUME", &ISoundSystem::volume);
	s.register_member("C_SNDSYS_CFG.BITRESOLUTION", &ISoundSystem::bit_resolution);
	s.register_member("C_SNDSYS_CFG.SAMPLERATE", &ISoundSystem::sample_rate);
	s.register_member("C_SNDSYS_CFG.USESTEREO", &ISoundSystem::use_stereo);
	s.register_member("C_SNDSYS_CFG.NUMSFXCHANNELS", &ISoundSystem::num_sfx_channels);
	s.register_member("C_SNDSYS_CFG.USED3DPROVIDERNAME", &ISoundSystem::used_3d_provider_name);
}

void zenkit::IParticleEffectEmitKey::register_(DaedalusScript& s) {
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

void zenkit::INpc::register_(DaedalusScript& s) {
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

void zenkit::IMission::register_(DaedalusScript& s) {
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

void zenkit::IItem::register_(DaedalusScript& s) {
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

void zenkit::IFocus::register_(DaedalusScript& s) {
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

void zenkit::IInfo::register_(DaedalusScript& s) {
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

void zenkit::IInfo::add_choice(IInfoChoice const& ch) {
	choices.insert(choices.begin(), ch);
}

void zenkit::IInfo::remove_choice(std::size_t index) {
	choices.erase(choices.begin() + static_cast<decltype(choices)::difference_type>(index));
}

void zenkit::IItemReact::register_(DaedalusScript& s) {
	ZKLOG_CLASS("C_ITEMREACT", "IItemReact");
	s.register_member("C_ITEMREACT.NPC", &IItemReact::npc);
	s.register_member("C_ITEMREACT.TRADE_ITEM", &IItemReact::trade_item);
	s.register_member("C_ITEMREACT.TRADE_AMOUNT", &IItemReact::trade_amount);
	s.register_member("C_ITEMREACT.REQUESTED_CAT", &IItemReact::requested_cat);
	s.register_member("C_ITEMREACT.REQUESTED_ITEM", &IItemReact::requested_item);
	s.register_member("C_ITEMREACT.REQUESTED_AMOUNT", &IItemReact::requested_amount);
	s.register_member("C_ITEMREACT.REACTION", &IItemReact::reaction);
}

void zenkit::ISpell::register_(DaedalusScript& s) {
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

void zenkit::ISvm::register_(DaedalusScript& s) {
	ZKLOG_CLASS("C_SVM", "ISvm");
	REG_IF_SYM_EXIST("C_SVM.MILGREETINGS", &ISvm::mil_greetings);
	REG_IF_SYM_EXIST("C_SVM.PALGREETINGS", &ISvm::pal_greetings);
	REG_IF_SYM_EXIST("C_SVM.WEATHER", &ISvm::weather);
	REG_IF_SYM_EXIST("C_SVM.IGETYOUSTILL", &ISvm::iget_you_still);
	REG_IF_SYM_EXIST("C_SVM.DIEENEMY", &ISvm::die_enemy);
	REG_IF_SYM_EXIST("C_SVM.DIEMONSTER", &ISvm::die_monster);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIEMONSTER", &ISvm::addon_die_monster);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIEMONSTER2", &ISvm::addon_die_monster_2);
	REG_IF_SYM_EXIST("C_SVM.DIRTYTHIEF", &ISvm::dirty_thief);
	REG_IF_SYM_EXIST("C_SVM.HANDSOFF", &ISvm::hands_off);
	REG_IF_SYM_EXIST("C_SVM.SHEEPKILLER", &ISvm::sheep_killer);
	REG_IF_SYM_EXIST("C_SVM.SHEEPKILLERMONSTER", &ISvm::sheep_killer_monster);
	REG_IF_SYM_EXIST("C_SVM.YOUMURDERER", &ISvm::you_murderer);
	REG_IF_SYM_EXIST("C_SVM.DIESTUPIDBEAST", &ISvm::die_stupid_beast);
	REG_IF_SYM_EXIST("C_SVM.YOUDAREHITME", &ISvm::you_dare_hit_me);
	REG_IF_SYM_EXIST("C_SVM.YOUASKEDFORIT", &ISvm::you_asked_for_it);
	REG_IF_SYM_EXIST("C_SVM.THENIBEATYOUOUTOFHERE", &ISvm::then_ibeat_you_out_of_here);
	REG_IF_SYM_EXIST("C_SVM.WHATDIDYOUDOINTHERE", &ISvm::what_did_you_do_in_there);
	REG_IF_SYM_EXIST("C_SVM.WILLYOUSTOPFIGHTING", &ISvm::will_you_stop_fighting);
	REG_IF_SYM_EXIST("C_SVM.KILLENEMY", &ISvm::kill_enemy);
	REG_IF_SYM_EXIST("C_SVM.ENEMYKILLED", &ISvm::enemy_killed);
	REG_IF_SYM_EXIST("C_SVM.MONSTERKILLED", &ISvm::monster_killed);
	REG_IF_SYM_EXIST("C_SVM.ADDON_MONSTERKILLED", &ISvm::addon_monster_killed);
	REG_IF_SYM_EXIST("C_SVM.ADDON_MONSTERKILLED2", &ISvm::addon_monster_killed_2);
	REG_IF_SYM_EXIST("C_SVM.THIEFDOWN", &ISvm::thief_down);
	REG_IF_SYM_EXIST("C_SVM.RUMFUMMLERDOWN", &ISvm::rumfummler_down);
	REG_IF_SYM_EXIST("C_SVM.SHEEPATTACKERDOWN", &ISvm::sheep_attacker_down);
	REG_IF_SYM_EXIST("C_SVM.KILLMURDERER", &ISvm::kill_murderer);
	REG_IF_SYM_EXIST("C_SVM.STUPIDBEASTKILLED", &ISvm::stupid_beast_killed);
	REG_IF_SYM_EXIST("C_SVM.NEVERHITMEAGAIN", &ISvm::never_hit_me_again);
	REG_IF_SYM_EXIST("C_SVM.YOUBETTERSHOULDHAVELISTENED", &ISvm::you_better_should_have_listened);
	REG_IF_SYM_EXIST("C_SVM.GETUPANDBEGONE", &ISvm::get_up_and_begone);
	REG_IF_SYM_EXIST("C_SVM.NEVERENTERROOMAGAIN", &ISvm::never_enter_room_again);
	REG_IF_SYM_EXIST("C_SVM.THEREISNOFIGHTINGHERE", &ISvm::there_is_no_fighting_here);
	REG_IF_SYM_EXIST("C_SVM.SPAREME", &ISvm::spare_me);
	REG_IF_SYM_EXIST("C_SVM.RUNAWAY", &ISvm::run_away);
	REG_IF_SYM_EXIST("C_SVM.ALARM", &ISvm::alarm);
	REG_IF_SYM_EXIST("C_SVM.GUARDS", &ISvm::guards);
	REG_IF_SYM_EXIST("C_SVM.HELP", &ISvm::help);
	REG_IF_SYM_EXIST("C_SVM.GOODMONSTERKILL", &ISvm::good_monster_kill);
	REG_IF_SYM_EXIST("C_SVM.GOODKILL", &ISvm::good_kill);
	REG_IF_SYM_EXIST("C_SVM.NOTNOW", &ISvm::not_now);
	REG_IF_SYM_EXIST("C_SVM.RUNCOWARD", &ISvm::run_coward);
	REG_IF_SYM_EXIST("C_SVM.GETOUTOFHERE", &ISvm::get_out_of_here);
	REG_IF_SYM_EXIST("C_SVM.WHYAREYOUINHERE", &ISvm::why_are_you_in_here);
	REG_IF_SYM_EXIST("C_SVM.YESGOOUTOFHERE", &ISvm::yes_go_out_of_here);
	REG_IF_SYM_EXIST("C_SVM.WHATSTHISSUPPOSEDTOBE", &ISvm::whats_this_supposed_to_be);
	REG_IF_SYM_EXIST("C_SVM.YOUDISTURBEDMYSLUMBER", &ISvm::you_disturbed_my_slumber);
	REG_IF_SYM_EXIST("C_SVM.ITOOKYOURGOLD", &ISvm::itook_your_gold);
	REG_IF_SYM_EXIST("C_SVM.SHITNOGOLD", &ISvm::shit_no_gold);
	REG_IF_SYM_EXIST("C_SVM.ITAKEYOURWEAPON", &ISvm::itake_your_weapon);
	REG_IF_SYM_EXIST("C_SVM.WHATAREYOUDOING", &ISvm::what_are_you_doing);
	REG_IF_SYM_EXIST("C_SVM.LOOKINGFORTROUBLEAGAIN", &ISvm::looking_for_trouble_again);
	REG_IF_SYM_EXIST("C_SVM.STOPMAGIC", &ISvm::stop_magic);
	REG_IF_SYM_EXIST("C_SVM.ISAIDSTOPMAGIC", &ISvm::isaid_stop_magic);
	REG_IF_SYM_EXIST("C_SVM.WEAPONDOWN", &ISvm::weapon_down);
	REG_IF_SYM_EXIST("C_SVM.ISAIDWEAPONDOWN", &ISvm::isaid_weapon_down);
	REG_IF_SYM_EXIST("C_SVM.WISEMOVE", &ISvm::wise_move);
	REG_IF_SYM_EXIST("C_SVM.NEXTTIMEYOUREINFORIT", &ISvm::next_time_youre_in_for_it);
	REG_IF_SYM_EXIST("C_SVM.OHMYHEAD", &ISvm::oh_my_head);
	REG_IF_SYM_EXIST("C_SVM.THERESAFIGHT", &ISvm::theres_afight);
	REG_IF_SYM_EXIST("C_SVM.OHMYGODITSAFIGHT", &ISvm::oh_my_god_its_afight);
	REG_IF_SYM_EXIST("C_SVM.GOODVICTORY", &ISvm::good_victory);
	REG_IF_SYM_EXIST("C_SVM.NOTBAD", &ISvm::not_bad);
	REG_IF_SYM_EXIST("C_SVM.OHMYGODHESDOWN", &ISvm::oh_my_god_hes_down);
	REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND01", &ISvm::cheer_friend_01);
	REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND02", &ISvm::cheer_friend_02);
	REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND03", &ISvm::cheer_friend_03);
	REG_IF_SYM_EXIST("C_SVM.OOH01", &ISvm::ooh_01);
	REG_IF_SYM_EXIST("C_SVM.OOH02", &ISvm::ooh_02);
	REG_IF_SYM_EXIST("C_SVM.OOH03", &ISvm::ooh_03);
	REG_IF_SYM_EXIST("C_SVM.WHATWASTHAT", &ISvm::what_was_that);
	REG_IF_SYM_EXIST("C_SVM.GETOUTOFMYBED", &ISvm::get_out_of_my_bed);
	REG_IF_SYM_EXIST("C_SVM.AWAKE", &ISvm::awake);
	REG_IF_SYM_EXIST("C_SVM.ABS_COMMANDER", &ISvm::abs_commander);
	REG_IF_SYM_EXIST("C_SVM.ABS_MONASTERY", &ISvm::abs_monastery);
	REG_IF_SYM_EXIST("C_SVM.ABS_FARM", &ISvm::abs_farm);
	REG_IF_SYM_EXIST("C_SVM.ABS_GOOD", &ISvm::abs_good);
	REG_IF_SYM_EXIST("C_SVM.SHEEPKILLER_CRIME", &ISvm::sheep_killer_crime);
	REG_IF_SYM_EXIST("C_SVM.ATTACK_CRIME", &ISvm::attack_crime);
	REG_IF_SYM_EXIST("C_SVM.THEFT_CRIME", &ISvm::theft_crime);
	REG_IF_SYM_EXIST("C_SVM.MURDER_CRIME", &ISvm::murder_crime);
	REG_IF_SYM_EXIST("C_SVM.PAL_CITY_CRIME", &ISvm::pal_city_crime);
	REG_IF_SYM_EXIST("C_SVM.MIL_CITY_CRIME", &ISvm::mil_city_crime);
	REG_IF_SYM_EXIST("C_SVM.CITY_CRIME", &ISvm::city_crime);
	REG_IF_SYM_EXIST("C_SVM.MONA_CRIME", &ISvm::mona_crime);
	REG_IF_SYM_EXIST("C_SVM.FARM_CRIME", &ISvm::farm_crime);
	REG_IF_SYM_EXIST("C_SVM.OC_CRIME", &ISvm::oc_crime);
	REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_ATTACKLOST", &ISvm::toughguy_attack_lost);
	REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_ATTACKWON", &ISvm::toughguy_attack_won);
	REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_PLAYERATTACK", &ISvm::toughguy_player_attack);
	REG_IF_SYM_EXIST("C_SVM.GOLD_1000", &ISvm::gold_1000);
	REG_IF_SYM_EXIST("C_SVM.GOLD_950", &ISvm::gold_950);
	REG_IF_SYM_EXIST("C_SVM.GOLD_900", &ISvm::gold_900);
	REG_IF_SYM_EXIST("C_SVM.GOLD_850", &ISvm::gold_850);
	REG_IF_SYM_EXIST("C_SVM.GOLD_800", &ISvm::gold_800);
	REG_IF_SYM_EXIST("C_SVM.GOLD_750", &ISvm::gold_750);
	REG_IF_SYM_EXIST("C_SVM.GOLD_700", &ISvm::gold_700);
	REG_IF_SYM_EXIST("C_SVM.GOLD_650", &ISvm::gold_650);
	REG_IF_SYM_EXIST("C_SVM.GOLD_600", &ISvm::gold_600);
	REG_IF_SYM_EXIST("C_SVM.GOLD_550", &ISvm::gold_550);
	REG_IF_SYM_EXIST("C_SVM.GOLD_500", &ISvm::gold_500);
	REG_IF_SYM_EXIST("C_SVM.GOLD_450", &ISvm::gold_450);
	REG_IF_SYM_EXIST("C_SVM.GOLD_400", &ISvm::gold_400);
	REG_IF_SYM_EXIST("C_SVM.GOLD_350", &ISvm::gold_350);
	REG_IF_SYM_EXIST("C_SVM.GOLD_300", &ISvm::gold_300);
	REG_IF_SYM_EXIST("C_SVM.GOLD_250", &ISvm::gold_250);
	REG_IF_SYM_EXIST("C_SVM.GOLD_200", &ISvm::gold_200);
	REG_IF_SYM_EXIST("C_SVM.GOLD_150", &ISvm::gold_150);
	REG_IF_SYM_EXIST("C_SVM.GOLD_100", &ISvm::gold_100);
	REG_IF_SYM_EXIST("C_SVM.GOLD_90", &ISvm::gold_90);
	REG_IF_SYM_EXIST("C_SVM.GOLD_80", &ISvm::gold_80);
	REG_IF_SYM_EXIST("C_SVM.GOLD_70", &ISvm::gold_70);
	REG_IF_SYM_EXIST("C_SVM.GOLD_60", &ISvm::gold_60);
	REG_IF_SYM_EXIST("C_SVM.GOLD_50", &ISvm::gold_50);
	REG_IF_SYM_EXIST("C_SVM.GOLD_40", &ISvm::gold_40);
	REG_IF_SYM_EXIST("C_SVM.GOLD_30", &ISvm::gold_30);
	REG_IF_SYM_EXIST("C_SVM.GOLD_20", &ISvm::gold_20);
	REG_IF_SYM_EXIST("C_SVM.GOLD_10", &ISvm::gold_10);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK01", &ISvm::smalltalk_01);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK02", &ISvm::smalltalk_02);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK03", &ISvm::smalltalk_03);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK04", &ISvm::smalltalk_04);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK05", &ISvm::smalltalk_05);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK06", &ISvm::smalltalk_06);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK07", &ISvm::smalltalk_07);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK08", &ISvm::smalltalk_08);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK09", &ISvm::smalltalk_09);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK10", &ISvm::smalltalk_10);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK11", &ISvm::smalltalk_11);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK12", &ISvm::smalltalk_12);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK13", &ISvm::smalltalk_13);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK14", &ISvm::smalltalk_14);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK15", &ISvm::smalltalk_15);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK16", &ISvm::smalltalk_16);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK17", &ISvm::smalltalk_17);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK18", &ISvm::smalltalk_18);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK19", &ISvm::smalltalk_19);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK20", &ISvm::smalltalk_20);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK21", &ISvm::smalltalk_21);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK22", &ISvm::smalltalk_22);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK23", &ISvm::smalltalk_23);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK24", &ISvm::smalltalk_24);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK25", &ISvm::smalltalk_25);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK26", &ISvm::smalltalk_26);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK27", &ISvm::smalltalk_27);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK28", &ISvm::smalltalk_28);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK29", &ISvm::smalltalk_29);
	REG_IF_SYM_EXIST("C_SVM.SMALLTALK30", &ISvm::smalltalk_30);
	REG_IF_SYM_EXIST("C_SVM.NOLEARNNOPOINTS", &ISvm::no_learn_no_points);
	REG_IF_SYM_EXIST("C_SVM.NOLEARNOVERPERSONALMAX", &ISvm::no_learn_over_personal_max);
	REG_IF_SYM_EXIST("C_SVM.NOLEARNYOUREBETTER", &ISvm::no_learn_youre_better);
	REG_IF_SYM_EXIST("C_SVM.YOULEARNEDSOMETHING", &ISvm::you_learned_something);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT", &ISvm::unterstadt);
	REG_IF_SYM_EXIST("C_SVM.OBERSTADT", &ISvm::oberstadt);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL", &ISvm::tempel);
	REG_IF_SYM_EXIST("C_SVM.MARKT", &ISvm::markt);
	REG_IF_SYM_EXIST("C_SVM.GALGEN", &ISvm::galgen);
	REG_IF_SYM_EXIST("C_SVM.KASERNE", &ISvm::kaserne);
	REG_IF_SYM_EXIST("C_SVM.HAFEN", &ISvm::hafen);
	REG_IF_SYM_EXIST("C_SVM.WHERETO", &ISvm::whereto);
	REG_IF_SYM_EXIST("C_SVM.OBERSTADT_2_UNTERSTADT", &ISvm::oberstadt_2_unterstadt);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_OBERSTADT", &ISvm::unterstadt_2_oberstadt);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_TEMPEL", &ISvm::unterstadt_2_tempel);
	REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_HAFEN", &ISvm::unterstadt_2_hafen);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_UNTERSTADT", &ISvm::tempel_2_unterstadt);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_MARKT", &ISvm::tempel_2_markt);
	REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_GALGEN", &ISvm::tempel_2_galgen);
	REG_IF_SYM_EXIST("C_SVM.MARKT_2_TEMPEL", &ISvm::markt_2_tempel);
	REG_IF_SYM_EXIST("C_SVM.MARKT_2_KASERNE", &ISvm::markt_2_kaserne);
	REG_IF_SYM_EXIST("C_SVM.MARKT_2_GALGEN", &ISvm::markt_2_galgen);
	REG_IF_SYM_EXIST("C_SVM.GALGEN_2_TEMPEL", &ISvm::galgen_2_tempel);
	REG_IF_SYM_EXIST("C_SVM.GALGEN_2_MARKT", &ISvm::galgen_2_markt);
	REG_IF_SYM_EXIST("C_SVM.GALGEN_2_KASERNE", &ISvm::galgen_2_kaserne);
	REG_IF_SYM_EXIST("C_SVM.KASERNE_2_MARKT", &ISvm::kaserne_2_markt);
	REG_IF_SYM_EXIST("C_SVM.KASERNE_2_GALGEN", &ISvm::kaserne_2_galgen);
	REG_IF_SYM_EXIST("C_SVM.HAFEN_2_UNTERSTADT", &ISvm::hafen_2_unterstadt);
	REG_IF_SYM_EXIST("C_SVM.DEAD", &ISvm::dead);
	REG_IF_SYM_EXIST("C_SVM.AARGH_1", &ISvm::aargh_1);
	REG_IF_SYM_EXIST("C_SVM.AARGH_2", &ISvm::aargh_2);
	REG_IF_SYM_EXIST("C_SVM.AARGH_3", &ISvm::aargh_3);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR", &ISvm::addon_wrong_armor);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_SLD", &ISvm::addon_wrong_armor_sld);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_MIL", &ISvm::addon_wrong_armor_mil);
	REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_KDF", &ISvm::addon_wrong_armor_kdf);
	REG_IF_SYM_EXIST("C_SVM.ADDON_NOARMOR_BDT", &ISvm::addon_no_armor_bdt);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIEBANDIT", &ISvm::addon_die_bandit);
	REG_IF_SYM_EXIST("C_SVM.ADDON_DIRTYPIRATE", &ISvm::addon_dirty_pirate);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND", &ISvm::sc_hey_turn_around);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND02", &ISvm::sc_hey_turn_around_02);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND03", &ISvm::sc_hey_turn_around_03);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND04", &ISvm::sc_hey_turn_around_04);
	REG_IF_SYM_EXIST("C_SVM.SC_HEYWAITASECOND", &ISvm::sc_hey_wait_asecond);
	REG_IF_SYM_EXIST("C_SVM.DOESNTWORK", &ISvm::doesnt_mork);
	REG_IF_SYM_EXIST("C_SVM.PICKBROKE", &ISvm::pick_broke);
	REG_IF_SYM_EXIST("C_SVM.NEEDKEY", &ISvm::need_key);
	REG_IF_SYM_EXIST("C_SVM.NOMOREPICKS", &ISvm::no_more_picks);
	REG_IF_SYM_EXIST("C_SVM.NOPICKLOCKTALENT", &ISvm::no_pick_lock_talent);
	REG_IF_SYM_EXIST("C_SVM.NOSWEEPING", &ISvm::no_sweeping);
	REG_IF_SYM_EXIST("C_SVM.PICKLOCKORKEYMISSING", &ISvm::pick_lock_or_key_missing);
	REG_IF_SYM_EXIST("C_SVM.KEYMISSING", &ISvm::key_missing);
	REG_IF_SYM_EXIST("C_SVM.PICKLOCKMISSING", &ISvm::pick_lock_missing);
	REG_IF_SYM_EXIST("C_SVM.NEVEROPEN", &ISvm::never_open);
	REG_IF_SYM_EXIST("C_SVM.MISSINGITEM", &ISvm::missing_item);
	REG_IF_SYM_EXIST("C_SVM.DONTKNOW", &ISvm::dont_know);
	REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET", &ISvm::nothing_to_get);
	REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET02", &ISvm::nothing_to_get_02);
	REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET03", &ISvm::nothing_to_get_03);
	REG_IF_SYM_EXIST("C_SVM.HEALSHRINE", &ISvm::heal_shrine);
	REG_IF_SYM_EXIST("C_SVM.HEALLASTSHRINE", &ISvm::heal_last_shrine);
	REG_IF_SYM_EXIST("C_SVM.IRDORATHTHEREYOUARE", &ISvm::irdorath_there_you_are);
	REG_IF_SYM_EXIST("C_SVM.SCOPENSIRDORATHBOOK", &ISvm::sc_opens_irdorath_book);
	REG_IF_SYM_EXIST("C_SVM.SCOPENSLASTDOOR", &ISvm::sc_opens_last_door);
	REG_IF_SYM_EXIST("C_SVM.TRADE_1", &ISvm::trade_1);
	REG_IF_SYM_EXIST("C_SVM.TRADE_2", &ISvm::trade_2);
	REG_IF_SYM_EXIST("C_SVM.TRADE_3", &ISvm::trade_3);
	REG_IF_SYM_EXIST("C_SVM.VERSTEHE", &ISvm::verstehe);
	REG_IF_SYM_EXIST("C_SVM.FOUNDTREASURE", &ISvm::found_treasure);
	REG_IF_SYM_EXIST("C_SVM.CANTUNDERSTANDTHIS", &ISvm::cant_understand_this);
	REG_IF_SYM_EXIST("C_SVM.CANTREADTHIS", &ISvm::cant_read_this);
	REG_IF_SYM_EXIST("C_SVM.STONEPLATE_1", &ISvm::stoneplate_1);
	REG_IF_SYM_EXIST("C_SVM.STONEPLATE_2", &ISvm::stoneplate_2);
	REG_IF_SYM_EXIST("C_SVM.STONEPLATE_3", &ISvm::stoneplate_3);
	REG_IF_SYM_EXIST("C_SVM.COUGH", &ISvm::cough);
	REG_IF_SYM_EXIST("C_SVM.HUI", &ISvm::hui);
	REG_IF_SYM_EXIST("C_SVM.ADDON_THISLITTLEBASTARD", &ISvm::addon_this_little_bastard);
	REG_IF_SYM_EXIST("C_SVM.ADDON_OPENADANOSTEMPLE", &ISvm::addon_open_adanos_temple);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_DESCRIPTION", &ISvm::attentat_addon_description);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_DESCRIPTION2", &ISvm::attentat_addon_description_2);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_PRO", &ISvm::attentat_addon_pro);
	REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_CONTRA", &ISvm::attentat_addon_contra);
	REG_IF_SYM_EXIST("C_SVM.MINE_ADDON_DESCRIPTION", &ISvm::mine_addon_description);
	REG_IF_SYM_EXIST("C_SVM.ADDON_SUMMONANCIENTGHOST", &ISvm::addon_summon_ancient_ghost);
	REG_IF_SYM_EXIST("C_SVM.ADDON_ANCIENTGHOST_NOTNEAR", &ISvm::addon_ancient_ghost_not_near);
	REG_IF_SYM_EXIST("C_SVM.ADDON_GOLD_DESCRIPTION", &ISvm::addon_gold_description);
	REG_IF_SYM_EXIST("C_SVM.WATCHYOURAIM", &ISvm::watch_your_aim);
	REG_IF_SYM_EXIST("C_SVM.watchyouraimangry", &ISvm::watch_your_aim_angry);
	REG_IF_SYM_EXIST("C_SVM.letsforgetourlittlefight", &ISvm::lets_forget_our_little_fight);
	REG_IF_SYM_EXIST("C_SVM.strange", &ISvm::strange);
	REG_IF_SYM_EXIST("C_SVM.diemortalenemy", &ISvm::die_mortal_enemy);
	REG_IF_SYM_EXIST("C_SVM.nowwait", &ISvm::now_wait);
	REG_IF_SYM_EXIST("C_SVM.nowwaitintruder", &ISvm::now_wait_intruder);
	REG_IF_SYM_EXIST("C_SVM.youstillnothaveenough", &ISvm::you_still_not_have_enough);
	REG_IF_SYM_EXIST("C_SVM.youattackedmycharge", &ISvm::you_attacked_my_charge);
	REG_IF_SYM_EXIST("C_SVM.iwillteachyourespectforforeignproperty",
	                 &ISvm::iwill_teach_you_respect_for_foreign_property);
	REG_IF_SYM_EXIST("C_SVM.youkilledoneofus", &ISvm::you_killed_one_of_us);
	REG_IF_SYM_EXIST("C_SVM.berzerk", &ISvm::berzerk);
	REG_IF_SYM_EXIST("C_SVM.youllbesorryforthis", &ISvm::youll_be_sorry_for_this);
	REG_IF_SYM_EXIST("C_SVM.yesyes", &ISvm::yes_yes);
	REG_IF_SYM_EXIST("C_SVM.shitwhatamonster", &ISvm::shit_what_amonster);
	REG_IF_SYM_EXIST("C_SVM.wewillmeetagain", &ISvm::we_will_meet_again);
	REG_IF_SYM_EXIST("C_SVM.nevertrythatagain", &ISvm::never_try_that_again);
	REG_IF_SYM_EXIST("C_SVM.itookyourore", &ISvm::itook_your_ore);
	REG_IF_SYM_EXIST("C_SVM.shitnoore", &ISvm::shit_no_ore);
	REG_IF_SYM_EXIST("C_SVM.youviolatedforbiddenterritory", &ISvm::you_violated_forbidden_territory);
	REG_IF_SYM_EXIST("C_SVM.youwannafoolme", &ISvm::you_wanna_fool_me);
	REG_IF_SYM_EXIST("C_SVM.whatdidyouinthere", &ISvm::what_did_you_in_there);
	REG_IF_SYM_EXIST("C_SVM.intruderalert", &ISvm::intruder_alert);
	REG_IF_SYM_EXIST("C_SVM.behindyou", &ISvm::behind_you);
	REG_IF_SYM_EXIST("C_SVM.heyheyhey", &ISvm::hey_hey_hey);
	REG_IF_SYM_EXIST("C_SVM.cheerfight", &ISvm::cheer_fight);
	REG_IF_SYM_EXIST("C_SVM.cheerfriend", &ISvm::cheer_friend);
	REG_IF_SYM_EXIST("C_SVM.ooh", &ISvm::ooh);
	REG_IF_SYM_EXIST("C_SVM.yeahwelldone", &ISvm::yeah_well_done);
	REG_IF_SYM_EXIST("C_SVM.hedefeatedhim", &ISvm::he_defeatedhim);
	REG_IF_SYM_EXIST("C_SVM.hedeservedit", &ISvm::he_deserv_edit);
	REG_IF_SYM_EXIST("C_SVM.hekilledhim", &ISvm::he_killed_him);
	REG_IF_SYM_EXIST("C_SVM.itwasagoodfight", &ISvm::it_was_agood_fight);
	REG_IF_SYM_EXIST("C_SVM.friendlygreetings", &ISvm::friendly_greetings);
	REG_IF_SYM_EXIST("C_SVM.algreetings", &ISvm::al_greetings);
	REG_IF_SYM_EXIST("C_SVM.magegreetings", &ISvm::mage_greetings);
	REG_IF_SYM_EXIST("C_SVM.sectgreetings", &ISvm::sect_greetings);
	REG_IF_SYM_EXIST("C_SVM.thereheis", &ISvm::there_he_is);
	REG_IF_SYM_EXIST("C_SVM.nolearnovermax", &ISvm::no_learn_over_max);
	REG_IF_SYM_EXIST("C_SVM.nolearnyoualreadyknow", &ISvm::no_learn_you_already_know);
	REG_IF_SYM_EXIST("C_SVM.heyyou", &ISvm::hey_you);
	REG_IF_SYM_EXIST("C_SVM.whatdoyouwant", &ISvm::what_do_you_want);
	REG_IF_SYM_EXIST("C_SVM.isaidwhatdoyouwant", &ISvm::isaid_what_do_you_want);
	REG_IF_SYM_EXIST("C_SVM.makeway", &ISvm::make_way);
	REG_IF_SYM_EXIST("C_SVM.outofmyway", &ISvm::out_of_my_way);
	REG_IF_SYM_EXIST("C_SVM.youdeaforwhat", &ISvm::you_deaf_or_what);
	REG_IF_SYM_EXIST("C_SVM.lookaway", &ISvm::look_away);
	REG_IF_SYM_EXIST("C_SVM.okaykeepit", &ISvm::okay_keep_it);
	REG_IF_SYM_EXIST("C_SVM.whatsthat", &ISvm::whats_that);
	REG_IF_SYM_EXIST("C_SVM.thatsmyweapon", &ISvm::thats_my_weapon);
	REG_IF_SYM_EXIST("C_SVM.giveittome", &ISvm::give_it_tome);
	REG_IF_SYM_EXIST("C_SVM.youcankeepthecrap", &ISvm::you_can_keep_the_crap);
	REG_IF_SYM_EXIST("C_SVM.theykilledmyfriend", &ISvm::they_killed_my_friend);
	REG_IF_SYM_EXIST("C_SVM.suckergotsome", &ISvm::sucker_got_some);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedebr", &ISvm::sucker_defeated_ebr);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedgur", &ISvm::sucker_defeated_gur);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedmage", &ISvm::sucker_defeated_mage);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatednov_guard", &ISvm::sucker_defeated_nov_guard);
	REG_IF_SYM_EXIST("C_SVM.suckerdefeatedvlk_guard", &ISvm::sucker_defeated_vlk_guard);
	REG_IF_SYM_EXIST("C_SVM.youdefeatedmycomrade", &ISvm::you_defeated_my_comrade);
	REG_IF_SYM_EXIST("C_SVM.youdefeatednov_guard", &ISvm::you_defeated_nov_guard);
	REG_IF_SYM_EXIST("C_SVM.youdefeatedvlk_guard", &ISvm::you_defeated_vlk_guard);
	REG_IF_SYM_EXIST("C_SVM.youstolefromme", &ISvm::you_stole_from_me);
	REG_IF_SYM_EXIST("C_SVM.youstolefromus", &ISvm::you_stole_from_us);
	REG_IF_SYM_EXIST("C_SVM.youstolefromebr", &ISvm::you_stole_from_ebr);
	REG_IF_SYM_EXIST("C_SVM.youstolefromgur", &ISvm::you_stole_from_gur);
	REG_IF_SYM_EXIST("C_SVM.stolefrommage", &ISvm::stole_urom_mage);
	REG_IF_SYM_EXIST("C_SVM.youkilledmyfriend", &ISvm::you_killedmyfriend);
	REG_IF_SYM_EXIST("C_SVM.youkilledebr", &ISvm::you_killed_ebr);
	REG_IF_SYM_EXIST("C_SVM.youkilledgur", &ISvm::you_killed_gur);
	REG_IF_SYM_EXIST("C_SVM.youkilledmage", &ISvm::you_killed_mage);
	REG_IF_SYM_EXIST("C_SVM.youkilledocfolk", &ISvm::you_killed_oc_folk);
	REG_IF_SYM_EXIST("C_SVM.youkilledncfolk", &ISvm::you_killed_nc_folk);
	REG_IF_SYM_EXIST("C_SVM.youkilledpsifolk", &ISvm::you_killed_psi_folk);
	REG_IF_SYM_EXIST("C_SVM.getthingsright", &ISvm::get_things_right);
	REG_IF_SYM_EXIST("C_SVM.youdefeatedmewell", &ISvm::you_defeated_me_well);
	REG_IF_SYM_EXIST("C_SVM.om", &ISvm::om);
}

#undef REG_IF_SYM_EXIST

void zenkit::IMenu::register_(DaedalusScript& s) {
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

void zenkit::IMenuItem::register_(DaedalusScript& s) {
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

void zenkit::ICamera::register_(DaedalusScript& s) {
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

void zenkit::IMusicSystem::register_(DaedalusScript& s) {
	ZKLOG_CLASS("C_MUSICSYS_CFG", "IMusicSystem");
	s.register_member("C_MUSICSYS_CFG.VOLUME", &IMusicSystem::volume);
	s.register_member("C_MUSICSYS_CFG.BITRESOLUTION", &IMusicSystem::bit_resolution);
	s.register_member("C_MUSICSYS_CFG.GLOBALREVERBENABLED", &IMusicSystem::global_reverb_enabled);
	s.register_member("C_MUSICSYS_CFG.SAMPLERATE", &IMusicSystem::sample_rate);
	s.register_member("C_MUSICSYS_CFG.NUMCHANNELS", &IMusicSystem::num_channels);
	s.register_member("C_MUSICSYS_CFG.REVERBBUFFERSIZE", &IMusicSystem::reverb_buffer_size);
}

void zenkit::IMusicTheme::register_(DaedalusScript& s) {
	ZKLOG_CLASS("C_MUSICTHEME", "IMusicTheme");
	s.register_member("C_MUSICTHEME.FILE", &IMusicTheme::file);
	s.register_member("C_MUSICTHEME.VOL", &IMusicTheme::vol);
	s.register_member("C_MUSICTHEME.LOOP", &IMusicTheme::loop);
	s.register_member("C_MUSICTHEME.REVERBMIX", &IMusicTheme::reverbmix);
	s.register_member("C_MUSICTHEME.REVERBTIME", &IMusicTheme::reverbtime);
	s.register_member("C_MUSICTHEME.TRANSTYPE", &IMusicTheme::transtype);
	s.register_member("C_MUSICTHEME.TRANSSUBTYPE", &IMusicTheme::transsubtype);
}

void zenkit::IMusicJingle::register_(DaedalusScript& s) {
	ZKLOG_CLASS("C_MUSICJINGLE", "IMusicJingle");
	s.register_member("C_MUSICJINGLE.NAME", &IMusicJingle::name);
	s.register_member("C_MUSICJINGLE.LOOP", &IMusicJingle::loop);
	s.register_member("C_MUSICJINGLE.VOL", &IMusicJingle::vol);
	s.register_member("C_MUSICJINGLE.TRANSSUBTYPE", &IMusicJingle::transsubtype);
}

void zenkit::IParticleEffect::register_(DaedalusScript& s) {
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

void zenkit::IEffectBase::register_(DaedalusScript& s) {
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
