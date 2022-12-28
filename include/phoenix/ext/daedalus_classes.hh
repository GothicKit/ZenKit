// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/script.hh>

#define var
#define string std::string
#define func int32_t

// clang-format off
#define FLAG(E)                                                                                                        \
	inline E operator|(E a, E b) {                                                                                     \
		return static_cast<E>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));                          \
	}                                                                                                                  \
	inline bool operator&(E a, E b) { return (static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b)) != 0; }
// clang-format on

namespace phoenix {
	struct c_gil_values : public instance {
		static constexpr std::uint32_t count = 66;

		var int32_t water_depth_knee[count];
		var int32_t water_depth_chest[count];
		var int32_t jumpup_height[count];
		var int32_t swim_time[count];
		var int32_t dive_time[count];
		var int32_t step_height[count];
		var int32_t jumplow_height[count];
		var int32_t jumpmid_height[count];
		var int32_t slide_angle[count];
		var int32_t slide_angle2[count];
		var int32_t disable_autoroll[count];
		var int32_t surface_align[count];
		var int32_t climb_heading_angle[count];
		var int32_t climb_horiz_angle[count];
		var int32_t climb_ground_angle[count];
		var int32_t fight_range_base[count];
		var int32_t fight_range_fist[count];
		var int32_t fight_range_g[count];
		var int32_t fight_range_1hs[count];
		var int32_t fight_range_1ha[count];
		var int32_t fight_range_2hs[count];
		var int32_t fight_range_2ha[count];
		var int32_t falldown_height[count];
		var int32_t falldown_damage[count];
		var int32_t blood_disabled[count];
		var int32_t blood_max_distance[count];
		var int32_t blood_amount[count];
		var int32_t blood_flow[count];
		var string blood_emitter[count];
		var string blood_texture[count];
		var int32_t turn_speed[count];

		static void register_(script& s) {
			s.register_member("C_GILVALUES.WATER_DEPTH_KNEE", &c_gil_values::water_depth_knee);
			s.register_member("C_GILVALUES.WATER_DEPTH_CHEST", &c_gil_values::water_depth_chest);
			s.register_member("C_GILVALUES.JUMPUP_HEIGHT", &c_gil_values::jumpup_height);
			s.register_member("C_GILVALUES.SWIM_TIME", &c_gil_values::swim_time);
			s.register_member("C_GILVALUES.DIVE_TIME", &c_gil_values::dive_time);
			s.register_member("C_GILVALUES.STEP_HEIGHT", &c_gil_values::step_height);
			s.register_member("C_GILVALUES.JUMPLOW_HEIGHT", &c_gil_values::jumplow_height);
			s.register_member("C_GILVALUES.JUMPMID_HEIGHT", &c_gil_values::jumpmid_height);
			s.register_member("C_GILVALUES.SLIDE_ANGLE", &c_gil_values::slide_angle);
			s.register_member("C_GILVALUES.SLIDE_ANGLE2", &c_gil_values::slide_angle2);
			s.register_member("C_GILVALUES.DISABLE_AUTOROLL", &c_gil_values::disable_autoroll);
			s.register_member("C_GILVALUES.SURFACE_ALIGN", &c_gil_values::surface_align);
			s.register_member("C_GILVALUES.CLIMB_HEADING_ANGLE", &c_gil_values::climb_heading_angle);
			s.register_member("C_GILVALUES.CLIMB_HORIZ_ANGLE", &c_gil_values::climb_horiz_angle);
			s.register_member("C_GILVALUES.CLIMB_GROUND_ANGLE", &c_gil_values::climb_ground_angle);
			s.register_member("C_GILVALUES.FIGHT_RANGE_BASE", &c_gil_values::fight_range_base);
			s.register_member("C_GILVALUES.FIGHT_RANGE_FIST", &c_gil_values::fight_range_fist);
			s.register_member("C_GILVALUES.FIGHT_RANGE_1HS", &c_gil_values::fight_range_1hs);
			s.register_member("C_GILVALUES.FIGHT_RANGE_1HA", &c_gil_values::fight_range_1ha);
			s.register_member("C_GILVALUES.FIGHT_RANGE_2HS", &c_gil_values::fight_range_2hs);
			s.register_member("C_GILVALUES.FIGHT_RANGE_2HA", &c_gil_values::fight_range_2ha);
			s.register_member("C_GILVALUES.FALLDOWN_HEIGHT", &c_gil_values::falldown_height);
			s.register_member("C_GILVALUES.FALLDOWN_DAMAGE", &c_gil_values::falldown_damage);
			s.register_member("C_GILVALUES.BLOOD_DISABLED", &c_gil_values::blood_disabled);
			s.register_member("C_GILVALUES.BLOOD_MAX_DISTANCE", &c_gil_values::blood_max_distance);
			s.register_member("C_GILVALUES.BLOOD_AMOUNT", &c_gil_values::blood_amount);
			s.register_member("C_GILVALUES.BLOOD_FLOW", &c_gil_values::blood_flow);
			s.register_member("C_GILVALUES.BLOOD_EMITTER", &c_gil_values::blood_emitter);
			s.register_member("C_GILVALUES.BLOOD_TEXTURE", &c_gil_values::blood_texture);
			s.register_member("C_GILVALUES.TURN_SPEED", &c_gil_values::turn_speed);

			// Gothic 2 only
			if (s.find_symbol_by_name("C_GILVALUES.FIGHT_RANGE_G") != nullptr) {
				s.register_member("C_GILVALUES.FIGHT_RANGE_G", &c_gil_values::fight_range_g);
			}
		}
	};

	namespace damage_type {
		static constexpr std::uint32_t barrier = 0U;
		static constexpr std::uint32_t blunt = 1U;
		static constexpr std::uint32_t edge = 2U;
		static constexpr std::uint32_t fire = 3U;
		static constexpr std::uint32_t fly = 4U;
		static constexpr std::uint32_t magic = 5U;
		static constexpr std::uint32_t point = 6U;
		static constexpr std::uint32_t fall = 7U;
		static constexpr std::uint32_t count = 8U;
	} // namespace damage_type

	namespace npc_attribute {
		static constexpr std::uint32_t hitpoints = 0U;
		static constexpr std::uint32_t hitpoints_max = 1U;
		static constexpr std::uint32_t mana = 2U;
		static constexpr std::uint32_t mana_max = 3U;
		static constexpr std::uint32_t strength = 4U;
		static constexpr std::uint32_t dexterity = 5U;
		static constexpr std::uint32_t regenerate_hp = 6U;
		static constexpr std::uint32_t regenerate_mana = 7U;
		static constexpr std::uint32_t count = 8U;
	} // namespace npc_attribute

	enum class npc_type : std::uint32_t {
		ambient = 0U,
		main = 1U,
		friend_ = 2U,
		oc_ambient = 3U,
		oc_main = 4U,
		bl_ambient = 5U,
		tal_ambient = 6U,
		bl_main = 7U,
	};

	enum class npc_flag : std::uint32_t {
		none = 0U,
		friends = 1U << 0U,
		immortal = 1U << 1U,
		ghost = 1U << 2U,
		protected_ = 1U << 3U,
	};
	FLAG(npc_flag)

	struct c_npc : public instance {
		static constexpr std::uint32_t hitchance_count =
		    5; // -> TODO: one of "unknown", "one-handed", "two-handed", "bow", "crossbow"
		static constexpr std::uint32_t name_count = 5;
		static constexpr std::uint32_t mission_count = 5;
		static constexpr std::uint32_t aivar_count = 100;

		var int32_t id;
		var string name[name_count];
		var string slot;
		var string effect;
		var npc_type type;
		var npc_flag flags;
		var int32_t attribute[npc_attribute::count];
		var int32_t hitchance[hitchance_count];
		var int32_t protection[damage_type::count];
		var int32_t damage[damage_type::count];
		var int32_t damage_type;
		var int32_t guild;
		var int32_t level;
		var func mission[mission_count];
		var int32_t fight_tactic;
		var int32_t weapon;
		var int32_t voice;
		var int32_t voice_pitch;
		var int32_t body_mass;
		var func daily_routine;
		var func start_aistate;
		var string spawnpoint;
		var int32_t spawn_delay;
		var int32_t senses;
		var int32_t senses_range;
		var int32_t aivar[aivar_count];
		var string wp;
		var int32_t exp;
		var int32_t exp_next;
		var int32_t lp;
		var int32_t bodystate_interruptable_override;
		var int32_t no_focus;

		static void register_(script& s) {
			s.register_member("C_NPC.ID", &c_npc::id);
			s.register_member("C_NPC.NAME", &c_npc::name);
			s.register_member("C_NPC.SLOT", &c_npc::slot);
			s.register_member("C_NPC.NPCTYPE", &c_npc::type);
			s.register_member("C_NPC.FLAGS", &c_npc::flags);
			s.register_member("C_NPC.ATTRIBUTE", &c_npc::attribute);
			s.register_member("C_NPC.PROTECTION", &c_npc::protection);
			s.register_member("C_NPC.DAMAGE", &c_npc::damage);
			s.register_member("C_NPC.DAMAGETYPE", &c_npc::damage_type);
			s.register_member("C_NPC.GUILD", &c_npc::guild);
			s.register_member("C_NPC.LEVEL", &c_npc::level);
			s.register_member("C_NPC.MISSION", &c_npc::mission);
			s.register_member("C_NPC.FIGHT_TACTIC", &c_npc::fight_tactic);
			s.register_member("C_NPC.WEAPON", &c_npc::weapon);
			s.register_member("C_NPC.VOICE", &c_npc::voice);
			s.register_member("C_NPC.VOICEPITCH", &c_npc::voice_pitch);
			s.register_member("C_NPC.BODYMASS", &c_npc::body_mass);
			s.register_member("C_NPC.DAILY_ROUTINE", &c_npc::daily_routine);
			s.register_member("C_NPC.START_AISTATE", &c_npc::start_aistate);
			s.register_member("C_NPC.SPAWNPOINT", &c_npc::spawnpoint);
			s.register_member("C_NPC.SPAWNDELAY", &c_npc::spawn_delay);
			s.register_member("C_NPC.SENSES", &c_npc::senses);
			s.register_member("C_NPC.SENSES_RANGE", &c_npc::senses_range);
			s.register_member("C_NPC.AIVAR", &c_npc::aivar);
			s.register_member("C_NPC.WP", &c_npc::wp);
			s.register_member("C_NPC.EXP", &c_npc::exp);
			s.register_member("C_NPC.EXP_NEXT", &c_npc::exp_next);
			s.register_member("C_NPC.LP", &c_npc::lp);

			// Gothic 2 only
			if (s.find_symbol_by_name("C_NPC.EFFECT") != nullptr) {
				s.register_member("C_NPC.EFFECT", &c_npc::effect);
				s.register_member("C_NPC.HITCHANCE", &c_npc::hitchance);
				s.register_member("C_NPC.BODYSTATEINTERRUPTABLEOVERRIDE", &c_npc::bodystate_interruptable_override);
				s.register_member("C_NPC.NOFOCUS", &c_npc::no_focus);
			}
		}
	};

	struct c_mission : public instance {
		var string name;
		var string description;
		var int32_t duration;
		var int32_t important;
		var func offer_conditions;
		var func offer;
		var func success_conditions;
		var func success;
		var func failure_conditions;
		var func failure;
		var func obsolete_conditions;
		var func obsolete;
		var func running;

		static void register_(script& s) {
			s.register_member("C_MISSION.NAME", &c_mission::name);
			s.register_member("C_MISSION.DESCRIPTION", &c_mission::description);
			s.register_member("C_MISSION.DURATION", &c_mission::duration);
			s.register_member("C_MISSION.IMPORTANT", &c_mission::important);
			s.register_member("C_MISSION.OFFERCONDITIONS", &c_mission::offer_conditions);
			s.register_member("C_MISSION.OFFER", &c_mission::offer);
			s.register_member("C_MISSION.SUCCESSCONDITIONS", &c_mission::success_conditions);
			s.register_member("C_MISSION.SUCCESS", &c_mission::success);
			s.register_member("C_MISSION.FAILURECONDITIONS", &c_mission::failure_conditions);
			s.register_member("C_MISSION.FAILURE", &c_mission::failure);
			s.register_member("C_MISSION.OBSOLETECONDITIONS", &c_mission::obsolete_conditions);
			s.register_member("C_MISSION.OBSOLETE", &c_mission::obsolete);
			s.register_member("C_MISSION.RUNNING", &c_mission::running);
		}
	};

	enum class item_flags : std::uint32_t {
		dagger = 1U << 13U,
		sword = 1U << 14U,
		axe = 1U << 15U,
		two_handed_sword = 1U << 16U,
		two_handed_axe = 1U << 17U,
		bow = 1U << 19U,
		crossbow = 1U << 20U,
		amulet = 1U << 22U,
		ring = 1U << 11U,
		belt = 1U << 24U,
		mission = 1U << 12U,
	};
	FLAG(item_flags)

	struct c_item : public instance {
		static constexpr std::uint32_t condition_count = 3;
		static constexpr std::uint32_t state_count = 4;

		var int32_t id;
		var string name;
		var string name_id;
		var int32_t hp;
		var int32_t hp_max;
		var int32_t main_flag;
		var item_flags flags;
		var int32_t weight;
		var int32_t value;
		var int32_t damage_type;
		var int32_t damage_total;
		var int32_t damage[damage_type::count];
		var int32_t wear;
		var int32_t protection[damage_type::count];
		var int32_t nutrition;
		var int32_t cond_atr[condition_count];
		var int32_t cond_value[condition_count];
		var int32_t change_atr[condition_count];
		var int32_t change_value[condition_count];
		var func magic;
		var func on_equip;
		var func on_unequip;
		var func on_state[state_count];
		var func owner;
		var int32_t owner_guild;
		var int32_t disguise_guild;
		var string visual;
		var string visual_change;
		var string effect;
		var int32_t visual_skin;
		var string scheme_name;
		var int32_t material;
		var int32_t munition;
		var int32_t spell;
		var int32_t range;
		var int32_t mag_circle;
		var string description;
		var string text[6];
		var int32_t count[6];
		var int32_t inv_zbias;
		var int32_t inv_rot_x;
		var int32_t inv_rot_y;
		var int32_t inv_rot_z;
		var int32_t inv_animate;

		static void register_(script& s) {
			s.register_member("C_ITEM.ID", &c_item::id);
			s.register_member("C_ITEM.NAME", &c_item::name);
			s.register_member("C_ITEM.NAMEID", &c_item::name_id);
			s.register_member("C_ITEM.HP", &c_item::hp);
			s.register_member("C_ITEM.HP_MAX", &c_item::hp_max);
			s.register_member("C_ITEM.MAINFLAG", &c_item::main_flag);
			s.register_member("C_ITEM.FLAGS", &c_item::flags);
			s.register_member("C_ITEM.WEIGHT", &c_item::weight);
			s.register_member("C_ITEM.VALUE", &c_item::value);
			s.register_member("C_ITEM.DAMAGETYPE", &c_item::damage_type);
			s.register_member("C_ITEM.DAMAGETOTAL", &c_item::damage_total);
			s.register_member("C_ITEM.DAMAGE", &c_item::damage);
			s.register_member("C_ITEM.WEAR", &c_item::wear);
			s.register_member("C_ITEM.PROTECTION", &c_item::protection);
			s.register_member("C_ITEM.NUTRITION", &c_item::nutrition);
			s.register_member("C_ITEM.COND_ATR", &c_item::cond_atr);
			s.register_member("C_ITEM.COND_VALUE", &c_item::cond_value);
			s.register_member("C_ITEM.CHANGE_ATR", &c_item::change_atr);
			s.register_member("C_ITEM.CHANGE_VALUE", &c_item::change_value);
			s.register_member("C_ITEM.MAGIC", &c_item::magic);
			s.register_member("C_ITEM.ON_EQUIP", &c_item::on_equip);
			s.register_member("C_ITEM.ON_UNEQUIP", &c_item::on_unequip);
			s.register_member("C_ITEM.ON_STATE", &c_item::on_state);
			s.register_member("C_ITEM.OWNER", &c_item::owner);
			s.register_member("C_ITEM.OWNERGUILD", &c_item::owner_guild);
			s.register_member("C_ITEM.DISGUISEGUILD", &c_item::disguise_guild);
			s.register_member("C_ITEM.VISUAL", &c_item::visual);
			s.register_member("C_ITEM.VISUAL_CHANGE", &c_item::visual_change);
			s.register_member("C_ITEM.VISUAL_SKIN", &c_item::visual_skin);
			s.register_member("C_ITEM.SCEMENAME", &c_item::scheme_name);
			s.register_member("C_ITEM.MATERIAL", &c_item::material);
			s.register_member("C_ITEM.MUNITION", &c_item::munition);
			s.register_member("C_ITEM.SPELL", &c_item::spell);
			s.register_member("C_ITEM.RANGE", &c_item::range);
			s.register_member("C_ITEM.MAG_CIRCLE", &c_item::mag_circle);
			s.register_member("C_ITEM.DESCRIPTION", &c_item::description);
			s.register_member("C_ITEM.TEXT", &c_item::text);
			s.register_member("C_ITEM.COUNT", &c_item::count);

			// Gothic 2 only
			if (s.find_symbol_by_name("C_ITEM.EFFECT") != nullptr) {
				s.register_member("C_ITEM.EFFECT", &c_item::effect);
				s.register_member("C_ITEM.INV_ZBIAS", &c_item::inv_zbias);
				s.register_member("C_ITEM.INV_ROTX", &c_item::inv_rot_x);
				s.register_member("C_ITEM.INV_ROTY", &c_item::inv_rot_y);
				s.register_member("C_ITEM.INV_ROTZ", &c_item::inv_rot_z);
				s.register_member("C_ITEM.INV_ANIMATE", &c_item::inv_animate);
			}
		}
	};

	struct c_focus : public instance {
		var float npc_longrange;
		var float npc_range1;
		var float npc_range2;
		var float npc_azi;
		var float npc_elevdo;
		var float npc_elevup;
		var int32_t npc_prio;
		var float item_range1;
		var float item_range2;
		var float item_azi;
		var float item_elevdo;
		var float item_elevup;
		var int32_t item_prio;
		var float mob_range1;
		var float mob_range2;
		var float mob_azi;
		var float mob_elevdo;
		var float mob_elevup;
		var int32_t mob_prio;

		static void register_(script& s) {
			s.register_member("C_FOCUS.NPC_LONGRANGE", &c_focus::npc_longrange);
			s.register_member("C_FOCUS.NPC_RANGE1", &c_focus::npc_range1);
			s.register_member("C_FOCUS.NPC_RANGE2", &c_focus::npc_range2);
			s.register_member("C_FOCUS.NPC_AZI", &c_focus::npc_azi);
			s.register_member("C_FOCUS.NPC_ELEVDO", &c_focus::npc_elevdo);
			s.register_member("C_FOCUS.NPC_ELEVUP", &c_focus::npc_elevup);
			s.register_member("C_FOCUS.NPC_PRIO", &c_focus::npc_prio);
			s.register_member("C_FOCUS.ITEM_RANGE1", &c_focus::item_range1);
			s.register_member("C_FOCUS.ITEM_RANGE2", &c_focus::item_range2);
			s.register_member("C_FOCUS.ITEM_AZI", &c_focus::item_azi);
			s.register_member("C_FOCUS.ITEM_ELEVDO", &c_focus::item_elevdo);
			s.register_member("C_FOCUS.ITEM_ELEVUP", &c_focus::item_elevup);
			s.register_member("C_FOCUS.ITEM_PRIO", &c_focus::item_prio);
			s.register_member("C_FOCUS.MOB_RANGE1", &c_focus::mob_range1);
			s.register_member("C_FOCUS.MOB_RANGE2", &c_focus::mob_range2);
			s.register_member("C_FOCUS.MOB_AZI", &c_focus::mob_azi);
			s.register_member("C_FOCUS.MOB_ELEVDO", &c_focus::mob_elevdo);
			s.register_member("C_FOCUS.MOB_ELEVUP", &c_focus::mob_elevup);
			s.register_member("C_FOCUS.MOB_PRIO", &c_focus::mob_prio);
		}
	};

	struct c_info_choice {
		var string text;
		var int32_t function = 0;
	};

	struct c_info : public instance {
		var int32_t npc;
		var int32_t nr;
		var int32_t important;
		var func condition;
		var func information;
		var string description;
		var int32_t trade;
		var int32_t permanent;

		std::vector<c_info_choice> choices {};

		void add_choice(const c_info_choice& ch) {
			choices.insert(choices.begin(), ch);
		}

		void remove_choice(std::size_t index) {
			choices.erase(choices.begin() + static_cast<decltype(choices)::difference_type>(index));
		}

		static void register_(script& s) {
			s.register_member("C_INFO.NPC", &c_info::npc);
			s.register_member("C_INFO.NR", &c_info::nr);
			s.register_member("C_INFO.IMPORTANT", &c_info::important);
			s.register_member("C_INFO.CONDITION", &c_info::condition);
			s.register_member("C_INFO.INFORMATION", &c_info::information);
			s.register_member("C_INFO.DESCRIPTION", &c_info::description);
			s.register_member("C_INFO.TRADE", &c_info::trade);
			s.register_member("C_INFO.PERMANENT", &c_info::permanent);
		}
	};

	struct c_item_react : public instance {
		var int32_t npc;
		var int32_t trade_item;
		var int32_t trade_amount;
		var int32_t requested_cat;
		var int32_t requested_item;
		var int32_t requested_amount;
		var func reaction;

		static void register_(script& s) {
			s.register_member("C_ITEMREACT.NPC", &c_item_react::npc);
			s.register_member("C_ITEMREACT.TRADE_ITEM", &c_item_react::trade_item);
			s.register_member("C_ITEMREACT.TRADE_AMOUNT", &c_item_react::trade_amount);
			s.register_member("C_ITEMREACT.REQUESTED_CAT", &c_item_react::requested_cat);
			s.register_member("C_ITEMREACT.REQUESTED_ITEM", &c_item_react::requested_item);
			s.register_member("C_ITEMREACT.REQUESTED_AMOUNT", &c_item_react::requested_amount);
			s.register_member("C_ITEMREACT.REACTION", &c_item_react::reaction);
		}
	};

	struct c_spell : public instance {
		var float time_per_mana;
		var int32_t damage_per_level;
		var int32_t damage_type;
		var int32_t spell_type;
		var int32_t can_turn_during_invest;
		var int32_t can_change_target_during_invest;
		var int32_t is_multi_effect;
		var int32_t target_collect_algo;
		var int32_t target_collect_type;
		var int32_t target_collect_range;
		var int32_t target_collect_azi;
		var int32_t target_collect_elev;

		static void register_(script& s) {
			s.register_member("C_SPELL.TIME_PER_MANA", &c_spell::time_per_mana);
			s.register_member("C_SPELL.DAMAGE_PER_LEVEL", &c_spell::damage_per_level);
			s.register_member("C_SPELL.DAMAGETYPE", &c_spell::damage_type);
			s.register_member("C_SPELL.SPELLTYPE", &c_spell::spell_type);
			s.register_member("C_SPELL.CANTURNDURINGINVEST", &c_spell::can_turn_during_invest);
			s.register_member("C_SPELL.CANCHANGETARGETDURINGINVEST", &c_spell::can_change_target_during_invest);
			s.register_member("C_SPELL.ISMULTIEFFECT", &c_spell::is_multi_effect);
			s.register_member("C_SPELL.TARGETCOLLECTALGO", &c_spell::target_collect_algo);
			s.register_member("C_SPELL.TARGETCOLLECTTYPE", &c_spell::target_collect_type);
			s.register_member("C_SPELL.TARGETCOLLECTRANGE", &c_spell::target_collect_range);
			s.register_member("C_SPELL.TARGETCOLLECTAZI", &c_spell::target_collect_azi);
			s.register_member("C_SPELL.TARGETCOLLECTELEV", &c_spell::target_collect_elev);
		}
	};

	struct c_svm : public instance {
		var string MILGREETINGS;
		var string PALGREETINGS;
		var string WEATHER;
		var string IGETYOUSTILL;
		var string DIEENEMY;
		var string DIEMONSTER;
		var string ADDON_DIEMONSTER;
		var string ADDON_DIEMONSTER2;
		var string DIRTYTHIEF;
		var string HANDSOFF;
		var string SHEEPKILLER;
		var string SHEEPKILLERMONSTER;
		var string YOUMURDERER;
		var string DIESTUPIDBEAST;
		var string YOUDAREHITME;
		var string YOUASKEDFORIT;
		var string THENIBEATYOUOUTOFHERE;
		var string WHATDIDYOUDOINTHERE;
		var string WILLYOUSTOPFIGHTING;
		var string KILLENEMY;
		var string ENEMYKILLED;
		var string MONSTERKILLED;
		var string ADDON_MONSTERKILLED;
		var string ADDON_MONSTERKILLED2;
		var string THIEFDOWN;
		var string RUMFUMMLERDOWN;
		var string SHEEPATTACKERDOWN;
		var string KILLMURDERER;
		var string STUPIDBEASTKILLED;
		var string NEVERHITMEAGAIN;
		var string YOUBETTERSHOULDHAVELISTENED;
		var string GETUPANDBEGONE;
		var string NEVERENTERROOMAGAIN;
		var string THEREISNOFIGHTINGHERE;
		var string SPAREME;
		var string RUNAWAY;
		var string ALARM;
		var string GUARDS;
		var string HELP;
		var string GOODMONSTERKILL;
		var string GOODKILL;
		var string NOTNOW;
		var string RUNCOWARD;
		var string GETOUTOFHERE;
		var string WHYAREYOUINHERE;
		var string YESGOOUTOFHERE;
		var string WHATSTHISSUPPOSEDTOBE;
		var string YOUDISTURBEDMYSLUMBER;
		var string ITOOKYOURGOLD;
		var string SHITNOGOLD;
		var string ITAKEYOURWEAPON;
		var string WHATAREYOUDOING;
		var string LOOKINGFORTROUBLEAGAIN;
		var string STOPMAGIC;
		var string ISAIDSTOPMAGIC;
		var string WEAPONDOWN;
		var string ISAIDWEAPONDOWN;
		var string WISEMOVE;
		var string NEXTTIMEYOUREINFORIT;
		var string OHMYHEAD;
		var string THERESAFIGHT;
		var string OHMYGODITSAFIGHT;
		var string GOODVICTORY;
		var string NOTBAD;
		var string OHMYGODHESDOWN;
		var string CHEERFRIEND01;
		var string CHEERFRIEND02;
		var string CHEERFRIEND03;
		var string OOH01;
		var string OOH02;
		var string OOH03;
		var string WHATWASTHAT;
		var string GETOUTOFMYBED;
		var string AWAKE;
		var string ABS_COMMANDER;
		var string ABS_MONASTERY;
		var string ABS_FARM;
		var string ABS_GOOD;
		var string SHEEPKILLER_CRIME;
		var string ATTACK_CRIME;
		var string THEFT_CRIME;
		var string MURDER_CRIME;
		var string PAL_CITY_CRIME;
		var string MIL_CITY_CRIME;
		var string CITY_CRIME;
		var string MONA_CRIME;
		var string FARM_CRIME;
		var string OC_CRIME;
		var string TOUGHGUY_ATTACKLOST;
		var string TOUGHGUY_ATTACKWON;
		var string TOUGHGUY_PLAYERATTACK;
		var string GOLD_1000;
		var string GOLD_950;
		var string GOLD_900;
		var string GOLD_850;
		var string GOLD_800;
		var string GOLD_750;
		var string GOLD_700;
		var string GOLD_650;
		var string GOLD_600;
		var string GOLD_550;
		var string GOLD_500;
		var string GOLD_450;
		var string GOLD_400;
		var string GOLD_350;
		var string GOLD_300;
		var string GOLD_250;
		var string GOLD_200;
		var string GOLD_150;
		var string GOLD_100;
		var string GOLD_90;
		var string GOLD_80;
		var string GOLD_70;
		var string GOLD_60;
		var string GOLD_50;
		var string GOLD_40;
		var string GOLD_30;
		var string GOLD_20;
		var string GOLD_10;
		var string SMALLTALK01;
		var string SMALLTALK02;
		var string SMALLTALK03;
		var string SMALLTALK04;
		var string SMALLTALK05;
		var string SMALLTALK06;
		var string SMALLTALK07;
		var string SMALLTALK08;
		var string SMALLTALK09;
		var string SMALLTALK10;
		var string SMALLTALK11;
		var string SMALLTALK12;
		var string SMALLTALK13;
		var string SMALLTALK14;
		var string SMALLTALK15;
		var string SMALLTALK16;
		var string SMALLTALK17;
		var string SMALLTALK18;
		var string SMALLTALK19;
		var string SMALLTALK20;
		var string SMALLTALK21;
		var string SMALLTALK22;
		var string SMALLTALK23;
		var string SMALLTALK24;
		var string SMALLTALK25;
		var string SMALLTALK26;
		var string SMALLTALK27;
		var string SMALLTALK28;
		var string SMALLTALK29;
		var string SMALLTALK30;
		var string NOLEARNNOPOINTS;
		var string NOLEARNOVERPERSONALMAX;
		var string NOLEARNYOUREBETTER;
		var string YOULEARNEDSOMETHING;
		var string UNTERSTADT;
		var string OBERSTADT;
		var string TEMPEL;
		var string MARKT;
		var string GALGEN;
		var string KASERNE;
		var string HAFEN;
		var string WHERETO;
		var string OBERSTADT_2_UNTERSTADT;
		var string UNTERSTADT_2_OBERSTADT;
		var string UNTERSTADT_2_TEMPEL;
		var string UNTERSTADT_2_HAFEN;
		var string TEMPEL_2_UNTERSTADT;
		var string TEMPEL_2_MARKT;
		var string TEMPEL_2_GALGEN;
		var string MARKT_2_TEMPEL;
		var string MARKT_2_KASERNE;
		var string MARKT_2_GALGEN;
		var string GALGEN_2_TEMPEL;
		var string GALGEN_2_MARKT;
		var string GALGEN_2_KASERNE;
		var string KASERNE_2_MARKT;
		var string KASERNE_2_GALGEN;
		var string HAFEN_2_UNTERSTADT;
		var string DEAD;
		var string AARGH_1;
		var string AARGH_2;
		var string AARGH_3;
		var string ADDON_WRONGARMOR;
		var string ADDON_WRONGARMOR_SLD;
		var string ADDON_WRONGARMOR_MIL;
		var string ADDON_WRONGARMOR_KDF;
		var string ADDON_NOARMOR_BDT;
		var string ADDON_DIEBANDIT;
		var string ADDON_DIRTYPIRATE;
		var string SC_HEYTURNAROUND;
		var string SC_HEYTURNAROUND02;
		var string SC_HEYTURNAROUND03;
		var string SC_HEYTURNAROUND04;
		var string SC_HEYWAITASECOND;
		var string DOESNTWORK;
		var string PICKBROKE;
		var string NEEDKEY;
		var string NOMOREPICKS;
		var string NOPICKLOCKTALENT;
		var string NOSWEEPING;
		var string PICKLOCKORKEYMISSING;
		var string KEYMISSING;
		var string PICKLOCKMISSING;
		var string NEVEROPEN;
		var string MISSINGITEM;
		var string DONTKNOW;
		var string NOTHINGTOGET;
		var string NOTHINGTOGET02;
		var string NOTHINGTOGET03;
		var string HEALSHRINE;
		var string HEALLASTSHRINE;
		var string IRDORATHTHEREYOUARE;
		var string SCOPENSIRDORATHBOOK;
		var string SCOPENSLASTDOOR;
		var string TRADE_1;
		var string TRADE_2;
		var string TRADE_3;
		var string VERSTEHE;
		var string FOUNDTREASURE;
		var string CANTUNDERSTANDTHIS;
		var string CANTREADTHIS;
		var string STONEPLATE_1;
		var string STONEPLATE_2;
		var string STONEPLATE_3;
		var string COUGH;
		var string HUI;
		var string ADDON_THISLITTLEBASTARD;
		var string ADDON_OPENADANOSTEMPLE;
		var string ATTENTAT_ADDON_DESCRIPTION;
		var string ATTENTAT_ADDON_DESCRIPTION2;
		var string ATTENTAT_ADDON_PRO;
		var string ATTENTAT_ADDON_CONTRA;
		var string MINE_ADDON_DESCRIPTION;
		var string ADDON_SUMMONANCIENTGHOST;
		var string ADDON_ANCIENTGHOST_NOTNEAR;
		var string ADDON_GOLD_DESCRIPTION;

		var string WATCHYOURAIM;
		var string watchyouraimangry;
		var string letsforgetourlittlefight;
		var string strange;
		var string diemortalenemy;
		var string nowwait;
		var string nowwaitintruder;
		var string youstillnothaveenough;
		var string youattackedmycharge;
		var string iwillteachyourespectforforeignproperty;
		var string youkilledoneofus;
		var string berzerk;
		var string youllbesorryforthis;
		var string yesyes;
		var string shitwhatamonster;
		var string wewillmeetagain;
		var string nevertrythatagain;
		var string itookyourore;
		var string shitnoore;
		var string youviolatedforbiddenterritory;
		var string youwannafoolme;
		var string whatdidyouinthere;
		var string intruderalert;
		var string behindyou;
		var string heyheyhey;
		var string cheerfight;
		var string cheerfriend;
		var string ooh;
		var string yeahwelldone;
		var string hedefeatedhim;
		var string hedeservedit;
		var string hekilledhim;
		var string itwasagoodfight;
		var string friendlygreetings;
		var string algreetings;
		var string magegreetings;
		var string sectgreetings;
		var string thereheis;
		var string nolearnovermax;
		var string nolearnyoualreadyknow;
		var string heyyou;
		var string whatdoyouwant;
		var string isaidwhatdoyouwant;
		var string makeway;
		var string outofmyway;
		var string youdeaforwhat;
		var string lookaway;
		var string okaykeepit;
		var string whatsthat;
		var string thatsmyweapon;
		var string giveittome;
		var string youcankeepthecrap;
		var string theykilledmyfriend;
		var string suckergotsome;
		var string suckerdefeatedebr;
		var string suckerdefeatedgur;
		var string suckerdefeatedmage;
		var string suckerdefeatednov_guard;
		var string suckerdefeatedvlk_guard;
		var string youdefeatedmycomrade;
		var string youdefeatednov_guard;
		var string youdefeatedvlk_guard;
		var string youstolefromme;
		var string youstolefromus;
		var string youstolefromebr;
		var string youstolefromgur;
		var string stolefrommage;
		var string youkilledmyfriend;
		var string youkilledebr;
		var string youkilledgur;
		var string youkilledmage;
		var string youkilledocfolk;
		var string youkilledncfolk;
		var string youkilledpsifolk;
		var string getthingsright;
		var string youdefeatedmewell;
		var string om;

#define REG_IF_SYM_EXIST(sym, ref)                                                                                     \
	do {                                                                                                               \
		if (s.find_symbol_by_name(sym)) {                                                                              \
			s.register_member(sym, ref);                                                                               \
		}                                                                                                              \
	} while (false)

		static void register_(script& s) {
			REG_IF_SYM_EXIST("C_SVM.MILGREETINGS", &c_svm::MILGREETINGS);
			REG_IF_SYM_EXIST("C_SVM.PALGREETINGS", &c_svm::PALGREETINGS);
			REG_IF_SYM_EXIST("C_SVM.WEATHER", &c_svm::WEATHER);
			REG_IF_SYM_EXIST("C_SVM.IGETYOUSTILL", &c_svm::IGETYOUSTILL);
			REG_IF_SYM_EXIST("C_SVM.DIEENEMY", &c_svm::DIEENEMY);
			REG_IF_SYM_EXIST("C_SVM.DIEMONSTER", &c_svm::DIEMONSTER);
			REG_IF_SYM_EXIST("C_SVM.ADDON_DIEMONSTER", &c_svm::ADDON_DIEMONSTER);
			REG_IF_SYM_EXIST("C_SVM.ADDON_DIEMONSTER2", &c_svm::ADDON_DIEMONSTER2);
			REG_IF_SYM_EXIST("C_SVM.DIRTYTHIEF", &c_svm::DIRTYTHIEF);
			REG_IF_SYM_EXIST("C_SVM.HANDSOFF", &c_svm::HANDSOFF);
			REG_IF_SYM_EXIST("C_SVM.SHEEPKILLER", &c_svm::SHEEPKILLER);
			REG_IF_SYM_EXIST("C_SVM.SHEEPKILLERMONSTER", &c_svm::SHEEPKILLERMONSTER);
			REG_IF_SYM_EXIST("C_SVM.YOUMURDERER", &c_svm::YOUMURDERER);
			REG_IF_SYM_EXIST("C_SVM.DIESTUPIDBEAST", &c_svm::DIESTUPIDBEAST);
			REG_IF_SYM_EXIST("C_SVM.YOUDAREHITME", &c_svm::YOUDAREHITME);
			REG_IF_SYM_EXIST("C_SVM.YOUASKEDFORIT", &c_svm::YOUASKEDFORIT);
			REG_IF_SYM_EXIST("C_SVM.THENIBEATYOUOUTOFHERE", &c_svm::THENIBEATYOUOUTOFHERE);
			REG_IF_SYM_EXIST("C_SVM.WHATDIDYOUDOINTHERE", &c_svm::WHATDIDYOUDOINTHERE);
			REG_IF_SYM_EXIST("C_SVM.WILLYOUSTOPFIGHTING", &c_svm::WILLYOUSTOPFIGHTING);
			REG_IF_SYM_EXIST("C_SVM.KILLENEMY", &c_svm::KILLENEMY);
			REG_IF_SYM_EXIST("C_SVM.ENEMYKILLED", &c_svm::ENEMYKILLED);
			REG_IF_SYM_EXIST("C_SVM.MONSTERKILLED", &c_svm::MONSTERKILLED);
			REG_IF_SYM_EXIST("C_SVM.ADDON_MONSTERKILLED", &c_svm::ADDON_MONSTERKILLED);
			REG_IF_SYM_EXIST("C_SVM.ADDON_MONSTERKILLED2", &c_svm::ADDON_MONSTERKILLED2);
			REG_IF_SYM_EXIST("C_SVM.THIEFDOWN", &c_svm::THIEFDOWN);
			REG_IF_SYM_EXIST("C_SVM.RUMFUMMLERDOWN", &c_svm::RUMFUMMLERDOWN);
			REG_IF_SYM_EXIST("C_SVM.SHEEPATTACKERDOWN", &c_svm::SHEEPATTACKERDOWN);
			REG_IF_SYM_EXIST("C_SVM.KILLMURDERER", &c_svm::KILLMURDERER);
			REG_IF_SYM_EXIST("C_SVM.STUPIDBEASTKILLED", &c_svm::STUPIDBEASTKILLED);
			REG_IF_SYM_EXIST("C_SVM.NEVERHITMEAGAIN", &c_svm::NEVERHITMEAGAIN);
			REG_IF_SYM_EXIST("C_SVM.YOUBETTERSHOULDHAVELISTENED", &c_svm::YOUBETTERSHOULDHAVELISTENED);
			REG_IF_SYM_EXIST("C_SVM.GETUPANDBEGONE", &c_svm::GETUPANDBEGONE);
			REG_IF_SYM_EXIST("C_SVM.NEVERENTERROOMAGAIN", &c_svm::NEVERENTERROOMAGAIN);
			REG_IF_SYM_EXIST("C_SVM.THEREISNOFIGHTINGHERE", &c_svm::THEREISNOFIGHTINGHERE);
			REG_IF_SYM_EXIST("C_SVM.SPAREME", &c_svm::SPAREME);
			REG_IF_SYM_EXIST("C_SVM.RUNAWAY", &c_svm::RUNAWAY);
			REG_IF_SYM_EXIST("C_SVM.ALARM", &c_svm::ALARM);
			REG_IF_SYM_EXIST("C_SVM.GUARDS", &c_svm::GUARDS);
			REG_IF_SYM_EXIST("C_SVM.HELP", &c_svm::HELP);
			REG_IF_SYM_EXIST("C_SVM.GOODMONSTERKILL", &c_svm::GOODMONSTERKILL);
			REG_IF_SYM_EXIST("C_SVM.GOODKILL", &c_svm::GOODKILL);
			REG_IF_SYM_EXIST("C_SVM.NOTNOW", &c_svm::NOTNOW);
			REG_IF_SYM_EXIST("C_SVM.RUNCOWARD", &c_svm::RUNCOWARD);
			REG_IF_SYM_EXIST("C_SVM.GETOUTOFHERE", &c_svm::GETOUTOFHERE);
			REG_IF_SYM_EXIST("C_SVM.WHYAREYOUINHERE", &c_svm::WHYAREYOUINHERE);
			REG_IF_SYM_EXIST("C_SVM.YESGOOUTOFHERE", &c_svm::YESGOOUTOFHERE);
			REG_IF_SYM_EXIST("C_SVM.WHATSTHISSUPPOSEDTOBE", &c_svm::WHATSTHISSUPPOSEDTOBE);
			REG_IF_SYM_EXIST("C_SVM.YOUDISTURBEDMYSLUMBER", &c_svm::YOUDISTURBEDMYSLUMBER);
			REG_IF_SYM_EXIST("C_SVM.ITOOKYOURGOLD", &c_svm::ITOOKYOURGOLD);
			REG_IF_SYM_EXIST("C_SVM.SHITNOGOLD", &c_svm::SHITNOGOLD);
			REG_IF_SYM_EXIST("C_SVM.ITAKEYOURWEAPON", &c_svm::ITAKEYOURWEAPON);
			REG_IF_SYM_EXIST("C_SVM.WHATAREYOUDOING", &c_svm::WHATAREYOUDOING);
			REG_IF_SYM_EXIST("C_SVM.LOOKINGFORTROUBLEAGAIN", &c_svm::LOOKINGFORTROUBLEAGAIN);
			REG_IF_SYM_EXIST("C_SVM.STOPMAGIC", &c_svm::STOPMAGIC);
			REG_IF_SYM_EXIST("C_SVM.ISAIDSTOPMAGIC", &c_svm::ISAIDSTOPMAGIC);
			REG_IF_SYM_EXIST("C_SVM.WEAPONDOWN", &c_svm::WEAPONDOWN);
			REG_IF_SYM_EXIST("C_SVM.ISAIDWEAPONDOWN", &c_svm::ISAIDWEAPONDOWN);
			REG_IF_SYM_EXIST("C_SVM.WISEMOVE", &c_svm::WISEMOVE);
			REG_IF_SYM_EXIST("C_SVM.NEXTTIMEYOUREINFORIT", &c_svm::NEXTTIMEYOUREINFORIT);
			REG_IF_SYM_EXIST("C_SVM.OHMYHEAD", &c_svm::OHMYHEAD);
			REG_IF_SYM_EXIST("C_SVM.THERESAFIGHT", &c_svm::THERESAFIGHT);
			REG_IF_SYM_EXIST("C_SVM.OHMYGODITSAFIGHT", &c_svm::OHMYGODITSAFIGHT);
			REG_IF_SYM_EXIST("C_SVM.GOODVICTORY", &c_svm::GOODVICTORY);
			REG_IF_SYM_EXIST("C_SVM.NOTBAD", &c_svm::NOTBAD);
			REG_IF_SYM_EXIST("C_SVM.OHMYGODHESDOWN", &c_svm::OHMYGODHESDOWN);
			REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND01", &c_svm::CHEERFRIEND01);
			REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND02", &c_svm::CHEERFRIEND02);
			REG_IF_SYM_EXIST("C_SVM.CHEERFRIEND03", &c_svm::CHEERFRIEND03);
			REG_IF_SYM_EXIST("C_SVM.OOH01", &c_svm::OOH01);
			REG_IF_SYM_EXIST("C_SVM.OOH02", &c_svm::OOH02);
			REG_IF_SYM_EXIST("C_SVM.OOH03", &c_svm::OOH03);
			REG_IF_SYM_EXIST("C_SVM.WHATWASTHAT", &c_svm::WHATWASTHAT);
			REG_IF_SYM_EXIST("C_SVM.GETOUTOFMYBED", &c_svm::GETOUTOFMYBED);
			REG_IF_SYM_EXIST("C_SVM.AWAKE", &c_svm::AWAKE);
			REG_IF_SYM_EXIST("C_SVM.ABS_COMMANDER", &c_svm::ABS_COMMANDER);
			REG_IF_SYM_EXIST("C_SVM.ABS_MONASTERY", &c_svm::ABS_MONASTERY);
			REG_IF_SYM_EXIST("C_SVM.ABS_FARM", &c_svm::ABS_FARM);
			REG_IF_SYM_EXIST("C_SVM.ABS_GOOD", &c_svm::ABS_GOOD);
			REG_IF_SYM_EXIST("C_SVM.SHEEPKILLER_CRIME", &c_svm::SHEEPKILLER_CRIME);
			REG_IF_SYM_EXIST("C_SVM.ATTACK_CRIME", &c_svm::ATTACK_CRIME);
			REG_IF_SYM_EXIST("C_SVM.THEFT_CRIME", &c_svm::THEFT_CRIME);
			REG_IF_SYM_EXIST("C_SVM.MURDER_CRIME", &c_svm::MURDER_CRIME);
			REG_IF_SYM_EXIST("C_SVM.PAL_CITY_CRIME", &c_svm::PAL_CITY_CRIME);
			REG_IF_SYM_EXIST("C_SVM.MIL_CITY_CRIME", &c_svm::MIL_CITY_CRIME);
			REG_IF_SYM_EXIST("C_SVM.CITY_CRIME", &c_svm::CITY_CRIME);
			REG_IF_SYM_EXIST("C_SVM.MONA_CRIME", &c_svm::MONA_CRIME);
			REG_IF_SYM_EXIST("C_SVM.FARM_CRIME", &c_svm::FARM_CRIME);
			REG_IF_SYM_EXIST("C_SVM.OC_CRIME", &c_svm::OC_CRIME);
			REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_ATTACKLOST", &c_svm::TOUGHGUY_ATTACKLOST);
			REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_ATTACKWON", &c_svm::TOUGHGUY_ATTACKWON);
			REG_IF_SYM_EXIST("C_SVM.TOUGHGUY_PLAYERATTACK", &c_svm::TOUGHGUY_PLAYERATTACK);
			REG_IF_SYM_EXIST("C_SVM.GOLD_1000", &c_svm::GOLD_1000);
			REG_IF_SYM_EXIST("C_SVM.GOLD_950", &c_svm::GOLD_950);
			REG_IF_SYM_EXIST("C_SVM.GOLD_900", &c_svm::GOLD_900);
			REG_IF_SYM_EXIST("C_SVM.GOLD_850", &c_svm::GOLD_850);
			REG_IF_SYM_EXIST("C_SVM.GOLD_800", &c_svm::GOLD_800);
			REG_IF_SYM_EXIST("C_SVM.GOLD_750", &c_svm::GOLD_750);
			REG_IF_SYM_EXIST("C_SVM.GOLD_700", &c_svm::GOLD_700);
			REG_IF_SYM_EXIST("C_SVM.GOLD_650", &c_svm::GOLD_650);
			REG_IF_SYM_EXIST("C_SVM.GOLD_600", &c_svm::GOLD_600);
			REG_IF_SYM_EXIST("C_SVM.GOLD_550", &c_svm::GOLD_550);
			REG_IF_SYM_EXIST("C_SVM.GOLD_500", &c_svm::GOLD_500);
			REG_IF_SYM_EXIST("C_SVM.GOLD_450", &c_svm::GOLD_450);
			REG_IF_SYM_EXIST("C_SVM.GOLD_400", &c_svm::GOLD_400);
			REG_IF_SYM_EXIST("C_SVM.GOLD_350", &c_svm::GOLD_350);
			REG_IF_SYM_EXIST("C_SVM.GOLD_300", &c_svm::GOLD_300);
			REG_IF_SYM_EXIST("C_SVM.GOLD_250", &c_svm::GOLD_250);
			REG_IF_SYM_EXIST("C_SVM.GOLD_200", &c_svm::GOLD_200);
			REG_IF_SYM_EXIST("C_SVM.GOLD_150", &c_svm::GOLD_150);
			REG_IF_SYM_EXIST("C_SVM.GOLD_100", &c_svm::GOLD_100);
			REG_IF_SYM_EXIST("C_SVM.GOLD_90", &c_svm::GOLD_90);
			REG_IF_SYM_EXIST("C_SVM.GOLD_80", &c_svm::GOLD_80);
			REG_IF_SYM_EXIST("C_SVM.GOLD_70", &c_svm::GOLD_70);
			REG_IF_SYM_EXIST("C_SVM.GOLD_60", &c_svm::GOLD_60);
			REG_IF_SYM_EXIST("C_SVM.GOLD_50", &c_svm::GOLD_50);
			REG_IF_SYM_EXIST("C_SVM.GOLD_40", &c_svm::GOLD_40);
			REG_IF_SYM_EXIST("C_SVM.GOLD_30", &c_svm::GOLD_30);
			REG_IF_SYM_EXIST("C_SVM.GOLD_20", &c_svm::GOLD_20);
			REG_IF_SYM_EXIST("C_SVM.GOLD_10", &c_svm::GOLD_10);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK01", &c_svm::SMALLTALK01);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK02", &c_svm::SMALLTALK02);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK03", &c_svm::SMALLTALK03);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK04", &c_svm::SMALLTALK04);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK05", &c_svm::SMALLTALK05);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK06", &c_svm::SMALLTALK06);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK07", &c_svm::SMALLTALK07);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK08", &c_svm::SMALLTALK08);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK09", &c_svm::SMALLTALK09);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK10", &c_svm::SMALLTALK10);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK11", &c_svm::SMALLTALK11);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK12", &c_svm::SMALLTALK12);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK13", &c_svm::SMALLTALK13);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK14", &c_svm::SMALLTALK14);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK15", &c_svm::SMALLTALK15);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK16", &c_svm::SMALLTALK16);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK17", &c_svm::SMALLTALK17);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK18", &c_svm::SMALLTALK18);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK19", &c_svm::SMALLTALK19);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK20", &c_svm::SMALLTALK20);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK21", &c_svm::SMALLTALK21);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK22", &c_svm::SMALLTALK22);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK23", &c_svm::SMALLTALK23);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK24", &c_svm::SMALLTALK24);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK25", &c_svm::SMALLTALK25);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK26", &c_svm::SMALLTALK26);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK27", &c_svm::SMALLTALK27);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK28", &c_svm::SMALLTALK28);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK29", &c_svm::SMALLTALK29);
			REG_IF_SYM_EXIST("C_SVM.SMALLTALK30", &c_svm::SMALLTALK30);
			REG_IF_SYM_EXIST("C_SVM.NOLEARNNOPOINTS", &c_svm::NOLEARNNOPOINTS);
			REG_IF_SYM_EXIST("C_SVM.NOLEARNOVERPERSONALMAX", &c_svm::NOLEARNOVERPERSONALMAX);
			REG_IF_SYM_EXIST("C_SVM.NOLEARNYOUREBETTER", &c_svm::NOLEARNYOUREBETTER);
			REG_IF_SYM_EXIST("C_SVM.YOULEARNEDSOMETHING", &c_svm::YOULEARNEDSOMETHING);
			REG_IF_SYM_EXIST("C_SVM.UNTERSTADT", &c_svm::UNTERSTADT);
			REG_IF_SYM_EXIST("C_SVM.OBERSTADT", &c_svm::OBERSTADT);
			REG_IF_SYM_EXIST("C_SVM.TEMPEL", &c_svm::TEMPEL);
			REG_IF_SYM_EXIST("C_SVM.MARKT", &c_svm::MARKT);
			REG_IF_SYM_EXIST("C_SVM.GALGEN", &c_svm::GALGEN);
			REG_IF_SYM_EXIST("C_SVM.KASERNE", &c_svm::KASERNE);
			REG_IF_SYM_EXIST("C_SVM.HAFEN", &c_svm::HAFEN);
			REG_IF_SYM_EXIST("C_SVM.WHERETO", &c_svm::WHERETO);
			REG_IF_SYM_EXIST("C_SVM.OBERSTADT_2_UNTERSTADT", &c_svm::OBERSTADT_2_UNTERSTADT);
			REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_OBERSTADT", &c_svm::UNTERSTADT_2_OBERSTADT);
			REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_TEMPEL", &c_svm::UNTERSTADT_2_TEMPEL);
			REG_IF_SYM_EXIST("C_SVM.UNTERSTADT_2_HAFEN", &c_svm::UNTERSTADT_2_HAFEN);
			REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_UNTERSTADT", &c_svm::TEMPEL_2_UNTERSTADT);
			REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_MARKT", &c_svm::TEMPEL_2_MARKT);
			REG_IF_SYM_EXIST("C_SVM.TEMPEL_2_GALGEN", &c_svm::TEMPEL_2_GALGEN);
			REG_IF_SYM_EXIST("C_SVM.MARKT_2_TEMPEL", &c_svm::MARKT_2_TEMPEL);
			REG_IF_SYM_EXIST("C_SVM.MARKT_2_KASERNE", &c_svm::MARKT_2_KASERNE);
			REG_IF_SYM_EXIST("C_SVM.MARKT_2_GALGEN", &c_svm::MARKT_2_GALGEN);
			REG_IF_SYM_EXIST("C_SVM.GALGEN_2_TEMPEL", &c_svm::GALGEN_2_TEMPEL);
			REG_IF_SYM_EXIST("C_SVM.GALGEN_2_MARKT", &c_svm::GALGEN_2_MARKT);
			REG_IF_SYM_EXIST("C_SVM.GALGEN_2_KASERNE", &c_svm::GALGEN_2_KASERNE);
			REG_IF_SYM_EXIST("C_SVM.KASERNE_2_MARKT", &c_svm::KASERNE_2_MARKT);
			REG_IF_SYM_EXIST("C_SVM.KASERNE_2_GALGEN", &c_svm::KASERNE_2_GALGEN);
			REG_IF_SYM_EXIST("C_SVM.HAFEN_2_UNTERSTADT", &c_svm::HAFEN_2_UNTERSTADT);
			REG_IF_SYM_EXIST("C_SVM.DEAD", &c_svm::DEAD);
			REG_IF_SYM_EXIST("C_SVM.AARGH_1", &c_svm::AARGH_1);
			REG_IF_SYM_EXIST("C_SVM.AARGH_2", &c_svm::AARGH_2);
			REG_IF_SYM_EXIST("C_SVM.AARGH_3", &c_svm::AARGH_3);
			REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR", &c_svm::ADDON_WRONGARMOR);
			REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_SLD", &c_svm::ADDON_WRONGARMOR_SLD);
			REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_MIL", &c_svm::ADDON_WRONGARMOR_MIL);
			REG_IF_SYM_EXIST("C_SVM.ADDON_WRONGARMOR_KDF", &c_svm::ADDON_WRONGARMOR_KDF);
			REG_IF_SYM_EXIST("C_SVM.ADDON_NOARMOR_BDT", &c_svm::ADDON_NOARMOR_BDT);
			REG_IF_SYM_EXIST("C_SVM.ADDON_DIEBANDIT", &c_svm::ADDON_DIEBANDIT);
			REG_IF_SYM_EXIST("C_SVM.ADDON_DIRTYPIRATE", &c_svm::ADDON_DIRTYPIRATE);
			REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND", &c_svm::SC_HEYTURNAROUND);
			REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND02", &c_svm::SC_HEYTURNAROUND02);
			REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND03", &c_svm::SC_HEYTURNAROUND03);
			REG_IF_SYM_EXIST("C_SVM.SC_HEYTURNAROUND04", &c_svm::SC_HEYTURNAROUND04);
			REG_IF_SYM_EXIST("C_SVM.SC_HEYWAITASECOND", &c_svm::SC_HEYWAITASECOND);
			REG_IF_SYM_EXIST("C_SVM.DOESNTWORK", &c_svm::DOESNTWORK);
			REG_IF_SYM_EXIST("C_SVM.PICKBROKE", &c_svm::PICKBROKE);
			REG_IF_SYM_EXIST("C_SVM.NEEDKEY", &c_svm::NEEDKEY);
			REG_IF_SYM_EXIST("C_SVM.NOMOREPICKS", &c_svm::NOMOREPICKS);
			REG_IF_SYM_EXIST("C_SVM.NOPICKLOCKTALENT", &c_svm::NOPICKLOCKTALENT);
			REG_IF_SYM_EXIST("C_SVM.NOSWEEPING", &c_svm::NOSWEEPING);
			REG_IF_SYM_EXIST("C_SVM.PICKLOCKORKEYMISSING", &c_svm::PICKLOCKORKEYMISSING);
			REG_IF_SYM_EXIST("C_SVM.KEYMISSING", &c_svm::KEYMISSING);
			REG_IF_SYM_EXIST("C_SVM.PICKLOCKMISSING", &c_svm::PICKLOCKMISSING);
			REG_IF_SYM_EXIST("C_SVM.NEVEROPEN", &c_svm::NEVEROPEN);
			REG_IF_SYM_EXIST("C_SVM.MISSINGITEM", &c_svm::MISSINGITEM);
			REG_IF_SYM_EXIST("C_SVM.DONTKNOW", &c_svm::DONTKNOW);
			REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET", &c_svm::NOTHINGTOGET);
			REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET02", &c_svm::NOTHINGTOGET02);
			REG_IF_SYM_EXIST("C_SVM.NOTHINGTOGET03", &c_svm::NOTHINGTOGET03);
			REG_IF_SYM_EXIST("C_SVM.HEALSHRINE", &c_svm::HEALSHRINE);
			REG_IF_SYM_EXIST("C_SVM.HEALLASTSHRINE", &c_svm::HEALLASTSHRINE);
			REG_IF_SYM_EXIST("C_SVM.IRDORATHTHEREYOUARE", &c_svm::IRDORATHTHEREYOUARE);
			REG_IF_SYM_EXIST("C_SVM.SCOPENSIRDORATHBOOK", &c_svm::SCOPENSIRDORATHBOOK);
			REG_IF_SYM_EXIST("C_SVM.SCOPENSLASTDOOR", &c_svm::SCOPENSLASTDOOR);
			REG_IF_SYM_EXIST("C_SVM.TRADE_1", &c_svm::TRADE_1);
			REG_IF_SYM_EXIST("C_SVM.TRADE_2", &c_svm::TRADE_2);
			REG_IF_SYM_EXIST("C_SVM.TRADE_3", &c_svm::TRADE_3);
			REG_IF_SYM_EXIST("C_SVM.VERSTEHE", &c_svm::VERSTEHE);
			REG_IF_SYM_EXIST("C_SVM.FOUNDTREASURE", &c_svm::FOUNDTREASURE);
			REG_IF_SYM_EXIST("C_SVM.CANTUNDERSTANDTHIS", &c_svm::CANTUNDERSTANDTHIS);
			REG_IF_SYM_EXIST("C_SVM.CANTREADTHIS", &c_svm::CANTREADTHIS);
			REG_IF_SYM_EXIST("C_SVM.STONEPLATE_1", &c_svm::STONEPLATE_1);
			REG_IF_SYM_EXIST("C_SVM.STONEPLATE_2", &c_svm::STONEPLATE_2);
			REG_IF_SYM_EXIST("C_SVM.STONEPLATE_3", &c_svm::STONEPLATE_3);
			REG_IF_SYM_EXIST("C_SVM.COUGH", &c_svm::COUGH);
			REG_IF_SYM_EXIST("C_SVM.HUI", &c_svm::HUI);
			REG_IF_SYM_EXIST("C_SVM.ADDON_THISLITTLEBASTARD", &c_svm::ADDON_THISLITTLEBASTARD);
			REG_IF_SYM_EXIST("C_SVM.ADDON_OPENADANOSTEMPLE", &c_svm::ADDON_OPENADANOSTEMPLE);
			REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_DESCRIPTION", &c_svm::ATTENTAT_ADDON_DESCRIPTION);
			REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_DESCRIPTION2", &c_svm::ATTENTAT_ADDON_DESCRIPTION2);
			REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_PRO", &c_svm::ATTENTAT_ADDON_PRO);
			REG_IF_SYM_EXIST("C_SVM.ATTENTAT_ADDON_CONTRA", &c_svm::ATTENTAT_ADDON_CONTRA);
			REG_IF_SYM_EXIST("C_SVM.MINE_ADDON_DESCRIPTION", &c_svm::MINE_ADDON_DESCRIPTION);
			REG_IF_SYM_EXIST("C_SVM.ADDON_SUMMONANCIENTGHOST", &c_svm::ADDON_SUMMONANCIENTGHOST);
			REG_IF_SYM_EXIST("C_SVM.ADDON_ANCIENTGHOST_NOTNEAR", &c_svm::ADDON_ANCIENTGHOST_NOTNEAR);
			REG_IF_SYM_EXIST("C_SVM.ADDON_GOLD_DESCRIPTION", &c_svm::ADDON_GOLD_DESCRIPTION);

			REG_IF_SYM_EXIST("C_SVM.WATCHYOURAIM", &c_svm::WATCHYOURAIM);
			REG_IF_SYM_EXIST("C_SVM.watchyouraimangry", &c_svm::watchyouraimangry);
			REG_IF_SYM_EXIST("C_SVM.letsforgetourlittlefight", &c_svm::letsforgetourlittlefight);
			REG_IF_SYM_EXIST("C_SVM.strange", &c_svm::strange);
			REG_IF_SYM_EXIST("C_SVM.diemortalenemy", &c_svm::diemortalenemy);
			REG_IF_SYM_EXIST("C_SVM.nowwait", &c_svm::nowwait);
			REG_IF_SYM_EXIST("C_SVM.nowwaitintruder", &c_svm::nowwaitintruder);
			REG_IF_SYM_EXIST("C_SVM.youstillnothaveenough", &c_svm::youstillnothaveenough);
			REG_IF_SYM_EXIST("C_SVM.youattackedmycharge", &c_svm::youattackedmycharge);
			REG_IF_SYM_EXIST("C_SVM.iwillteachyourespectforforeignproperty",
			                 &c_svm::iwillteachyourespectforforeignproperty);
			REG_IF_SYM_EXIST("C_SVM.youkilledoneofus", &c_svm::youkilledoneofus);
			REG_IF_SYM_EXIST("C_SVM.berzerk", &c_svm::berzerk);
			REG_IF_SYM_EXIST("C_SVM.youllbesorryforthis", &c_svm::youllbesorryforthis);
			REG_IF_SYM_EXIST("C_SVM.yesyes", &c_svm::yesyes);
			REG_IF_SYM_EXIST("C_SVM.shitwhatamonster", &c_svm::shitwhatamonster);
			REG_IF_SYM_EXIST("C_SVM.wewillmeetagain", &c_svm::wewillmeetagain);
			REG_IF_SYM_EXIST("C_SVM.nevertrythatagain", &c_svm::nevertrythatagain);
			REG_IF_SYM_EXIST("C_SVM.itookyourore", &c_svm::itookyourore);
			REG_IF_SYM_EXIST("C_SVM.shitnoore", &c_svm::shitnoore);
			REG_IF_SYM_EXIST("C_SVM.youviolatedforbiddenterritory", &c_svm::youviolatedforbiddenterritory);
			REG_IF_SYM_EXIST("C_SVM.youwannafoolme", &c_svm::youwannafoolme);
			REG_IF_SYM_EXIST("C_SVM.whatdidyouinthere", &c_svm::whatdidyouinthere);
			REG_IF_SYM_EXIST("C_SVM.intruderalert", &c_svm::intruderalert);
			REG_IF_SYM_EXIST("C_SVM.behindyou", &c_svm::behindyou);
			REG_IF_SYM_EXIST("C_SVM.heyheyhey", &c_svm::heyheyhey);
			REG_IF_SYM_EXIST("C_SVM.cheerfight", &c_svm::cheerfight);
			REG_IF_SYM_EXIST("C_SVM.cheerfriend", &c_svm::cheerfriend);
			REG_IF_SYM_EXIST("C_SVM.ooh", &c_svm::ooh);
			REG_IF_SYM_EXIST("C_SVM.yeahwelldone", &c_svm::yeahwelldone);
			REG_IF_SYM_EXIST("C_SVM.hedefeatedhim", &c_svm::hedefeatedhim);
			REG_IF_SYM_EXIST("C_SVM.hedeservedit", &c_svm::hedeservedit);
			REG_IF_SYM_EXIST("C_SVM.hekilledhim", &c_svm::hekilledhim);
			REG_IF_SYM_EXIST("C_SVM.itwasagoodfight", &c_svm::itwasagoodfight);
			REG_IF_SYM_EXIST("C_SVM.friendlygreetings", &c_svm::friendlygreetings);
			REG_IF_SYM_EXIST("C_SVM.algreetings", &c_svm::algreetings);
			REG_IF_SYM_EXIST("C_SVM.magegreetings", &c_svm::magegreetings);
			REG_IF_SYM_EXIST("C_SVM.sectgreetings", &c_svm::sectgreetings);
			REG_IF_SYM_EXIST("C_SVM.thereheis", &c_svm::thereheis);
			REG_IF_SYM_EXIST("C_SVM.nolearnovermax", &c_svm::nolearnovermax);
			REG_IF_SYM_EXIST("C_SVM.nolearnyoualreadyknow", &c_svm::nolearnyoualreadyknow);
			REG_IF_SYM_EXIST("C_SVM.heyyou", &c_svm::heyyou);
			REG_IF_SYM_EXIST("C_SVM.whatdoyouwant", &c_svm::whatdoyouwant);
			REG_IF_SYM_EXIST("C_SVM.isaidwhatdoyouwant", &c_svm::isaidwhatdoyouwant);
			REG_IF_SYM_EXIST("C_SVM.makeway", &c_svm::makeway);
			REG_IF_SYM_EXIST("C_SVM.outofmyway", &c_svm::outofmyway);
			REG_IF_SYM_EXIST("C_SVM.youdeaforwhat", &c_svm::youdeaforwhat);
			REG_IF_SYM_EXIST("C_SVM.lookaway", &c_svm::lookaway);
			REG_IF_SYM_EXIST("C_SVM.okaykeepit", &c_svm::okaykeepit);
			REG_IF_SYM_EXIST("C_SVM.whatsthat", &c_svm::whatsthat);
			REG_IF_SYM_EXIST("C_SVM.thatsmyweapon", &c_svm::thatsmyweapon);
			REG_IF_SYM_EXIST("C_SVM.giveittome", &c_svm::giveittome);
			REG_IF_SYM_EXIST("C_SVM.youcankeepthecrap", &c_svm::youcankeepthecrap);
			REG_IF_SYM_EXIST("C_SVM.theykilledmyfriend", &c_svm::theykilledmyfriend);
			REG_IF_SYM_EXIST("C_SVM.suckergotsome", &c_svm::suckergotsome);
			REG_IF_SYM_EXIST("C_SVM.suckerdefeatedebr", &c_svm::suckerdefeatedebr);
			REG_IF_SYM_EXIST("C_SVM.suckerdefeatedgur", &c_svm::suckerdefeatedgur);
			REG_IF_SYM_EXIST("C_SVM.suckerdefeatedmage", &c_svm::suckerdefeatedmage);
			REG_IF_SYM_EXIST("C_SVM.suckerdefeatednov_guard", &c_svm::suckerdefeatednov_guard);
			REG_IF_SYM_EXIST("C_SVM.suckerdefeatedvlk_guard", &c_svm::suckerdefeatedvlk_guard);
			REG_IF_SYM_EXIST("C_SVM.youdefeatedmycomrade", &c_svm::youdefeatedmycomrade);
			REG_IF_SYM_EXIST("C_SVM.youdefeatednov_guard", &c_svm::youdefeatednov_guard);
			REG_IF_SYM_EXIST("C_SVM.youdefeatedvlk_guard", &c_svm::youdefeatedvlk_guard);
			REG_IF_SYM_EXIST("C_SVM.youstolefromme", &c_svm::youstolefromme);
			REG_IF_SYM_EXIST("C_SVM.youstolefromus", &c_svm::youstolefromus);
			REG_IF_SYM_EXIST("C_SVM.youstolefromebr", &c_svm::youstolefromebr);
			REG_IF_SYM_EXIST("C_SVM.youstolefromgur", &c_svm::youstolefromgur);
			REG_IF_SYM_EXIST("C_SVM.stolefrommage", &c_svm::stolefrommage);
			REG_IF_SYM_EXIST("C_SVM.youkilledmyfriend", &c_svm::youkilledmyfriend);
			REG_IF_SYM_EXIST("C_SVM.youkilledebr", &c_svm::youkilledebr);
			REG_IF_SYM_EXIST("C_SVM.youkilledgur", &c_svm::youkilledgur);
			REG_IF_SYM_EXIST("C_SVM.youkilledmage", &c_svm::youkilledmage);
			REG_IF_SYM_EXIST("C_SVM.youkilledocfolk", &c_svm::youkilledocfolk);
			REG_IF_SYM_EXIST("C_SVM.youkilledncfolk", &c_svm::youkilledncfolk);
			REG_IF_SYM_EXIST("C_SVM.youkilledpsifolk", &c_svm::youkilledpsifolk);
			REG_IF_SYM_EXIST("C_SVM.getthingsright", &c_svm::getthingsright);
			REG_IF_SYM_EXIST("C_SVM.youdefeatedmewell", &c_svm::youdefeatedmewell);
			REG_IF_SYM_EXIST("C_SVM.om", &c_svm::om);
		}
	};

#undef REG_IF_SYM_EXIST

	enum class c_menu_flags : std::uint32_t {
		overtop = 1 << 0,
		exclusive = 1 << 1,
		no_animation = 1 << 2,
		dont_scale_dimension = 1 << 3,
		dont_scale_position = 1 << 4,
		align_center = 1 << 5,
		show_info = 1 << 6,
	};
	FLAG(c_menu_flags)

	struct c_menu : public instance {
		static constexpr std::uint8_t item_count = 150;

		var string back_pic;
		var string back_world;
		var int32_t pos_x;
		var int32_t pos_y;
		var int32_t dim_x;
		var int32_t dim_y;
		var int32_t alpha;
		var string music_theme;
		var int32_t event_timer_msec;
		var string items[item_count];
		var c_menu_flags flags;
		var int32_t default_outgame;
		var int32_t default_ingame;

		static void register_(script& s) {
			s.register_member("C_MENU.BACKPIC", &c_menu::back_pic);
			s.register_member("C_MENU.BACKWORLD", &c_menu::back_world);
			s.register_member("C_MENU.POSX", &c_menu::pos_x);
			s.register_member("C_MENU.POSY", &c_menu::pos_y);
			s.register_member("C_MENU.DIMX", &c_menu::dim_x);
			s.register_member("C_MENU.DIMY", &c_menu::dim_y);
			s.register_member("C_MENU.ALPHA", &c_menu::alpha);
			s.register_member("C_MENU.MUSICTHEME", &c_menu::music_theme);
			s.register_member("C_MENU.EVENTTIMERMSEC", &c_menu::event_timer_msec);
			s.register_member("C_MENU.ITEMS", &c_menu::items);
			s.register_member("C_MENU.FLAGS", &c_menu::flags);
			s.register_member("C_MENU.DEFAULTOUTGAME", &c_menu::default_outgame);
			s.register_member("C_MENU.DEFAULTINGAME", &c_menu::default_ingame);
		}
	};

	enum class c_menu_item_flags : std::uint32_t {
		chromakeyed = 1 << 0,
		transparent = 1 << 1,
		selectable = 1 << 2,
		movable = 1 << 3,
		centered = 1 << 4,
		disabled = 1 << 5,
		fade = 1 << 6,
		effects = 1 << 7,
		only_outgame = 1 << 8,
		only_ingame = 1 << 9,
		perf_option = 1 << 10,
		multiline = 1 << 11,
		needs_apply = 1 << 12,
		needs_restart = 1 << 13,
		extended_menu = 1 << 14,
	};
	FLAG(c_menu_item_flags)

	enum class c_menu_item_type : std::uint32_t {
		unknown = 0,
		text = 1,
		slider = 2,
		input = 3,
		cursor = 4,
		choicebox = 5,
		button = 6,
		listbox = 7,
	};

	enum class c_menu_item_select_event : uint8_t {
		execute = 1,
		changed = 2,
		leave = 3,
		timer = 4,
		close = 5,
		init = 6,
		select_previous = 7,
		select_next = 8,
	};

	enum class c_menu_item_select_action : uint8_t {
		unknown = 0,
		back = 1,
		start_menu = 2,
		start_item = 3,
		close = 4,
		con_commands = 5,
		play_sound = 6,
		execute_commands = 7,
	};

	struct c_menu_item : public instance {
		static constexpr std::uint32_t text_count = 10;
		static constexpr std::uint32_t select_action_count = 5;
		static constexpr std::uint32_t event_action_count = 10;
		static constexpr std::uint32_t user_item_count = 4;

		var string fontname;
		var string text[text_count];
		var string backpic;
		var string alphamode;
		var int32_t alpha;
		var c_menu_item_type type;
		var int32_t on_sel_action[select_action_count];
		var string on_sel_action_s[select_action_count];
		var string on_chg_set_option;
		var string on_chg_set_option_section;
		var func on_event_action[event_action_count];
		var int32_t pos_x;
		var int32_t pos_y;
		var int32_t dim_x;
		var int32_t dim_y;
		var float size_start_scale;
		var c_menu_item_flags flags;
		var float open_delay_time;
		var float open_duration;
		var float user_float[user_item_count];
		var string user_string[user_item_count];
		var int32_t frame_sizex;
		var int32_t frame_sizey;
		var string hide_if_option_section_set;
		var string hide_if_option_set;
		var int32_t hide_on_value;

		static void register_(script& s) {
			s.register_member("C_MENU_ITEM.FONTNAME", &c_menu_item::fontname);
			s.register_member("C_MENU_ITEM.TEXT", &c_menu_item::text);
			s.register_member("C_MENU_ITEM.BACKPIC", &c_menu_item::backpic);
			s.register_member("C_MENU_ITEM.ALPHAMODE", &c_menu_item::alphamode);
			s.register_member("C_MENU_ITEM.ALPHA", &c_menu_item::alpha);
			s.register_member("C_MENU_ITEM.TYPE", &c_menu_item::type);
			s.register_member("C_MENU_ITEM.ONSELACTION", &c_menu_item::on_sel_action);
			s.register_member("C_MENU_ITEM.ONSELACTION_S", &c_menu_item::on_sel_action_s);
			s.register_member("C_MENU_ITEM.ONCHGSETOPTION", &c_menu_item::on_chg_set_option);
			s.register_member("C_MENU_ITEM.ONCHGSETOPTIONSECTION", &c_menu_item::on_chg_set_option_section);
			s.register_member("C_MENU_ITEM.ONEVENTACTION", &c_menu_item::on_event_action);
			s.register_member("C_MENU_ITEM.POSX", &c_menu_item::pos_x);
			s.register_member("C_MENU_ITEM.POSY", &c_menu_item::pos_y);
			s.register_member("C_MENU_ITEM.DIMX", &c_menu_item::dim_x);
			s.register_member("C_MENU_ITEM.DIMY", &c_menu_item::dim_y);
			s.register_member("C_MENU_ITEM.SIZESTARTSCALE", &c_menu_item::size_start_scale);
			s.register_member("C_MENU_ITEM.FLAGS", &c_menu_item::flags);
			s.register_member("C_MENU_ITEM.OPENDELAYTIME", &c_menu_item::open_delay_time);
			s.register_member("C_MENU_ITEM.OPENDURATION", &c_menu_item::open_duration);
			s.register_member("C_MENU_ITEM.USERFLOAT", &c_menu_item::user_float);
			s.register_member("C_MENU_ITEM.USERSTRING", &c_menu_item::user_string);
			s.register_member("C_MENU_ITEM.FRAMESIZEX", &c_menu_item::frame_sizex);
			s.register_member("C_MENU_ITEM.FRAMESIZEY", &c_menu_item::frame_sizey);

			// Gothic 2 only
			if (s.find_symbol_by_name("C_MENU_ITEM.HIDEIFOPTIONSECTIONSET") != nullptr) {
				s.register_member("C_MENU_ITEM.HIDEIFOPTIONSECTIONSET", &c_menu_item::hide_if_option_section_set);
				s.register_member("C_MENU_ITEM.HIDEIFOPTIONSET", &c_menu_item::hide_if_option_set);
				s.register_member("C_MENU_ITEM.HIDEONVALUE", &c_menu_item::hide_on_value);
			}
		}
	};

	struct c_camera : public instance {
		var float best_range;
		var float min_range;
		var float max_range;
		var float best_elevation;
		var float min_elevation;
		var float max_elevation;
		var float best_azimuth;
		var float min_azimuth;
		var float max_azimuth;
		var float best_rot_z;
		var float min_rot_z;
		var float max_rot_z;
		var float rot_offset_x;
		var float rot_offset_y;
		var float rot_offset_z;
		var float target_offset_x;
		var float target_offset_y;
		var float target_offset_z;
		var float velo_trans;
		var float velo_rot;
		var int32_t translate;
		var int32_t rotate;
		var int32_t collision;

		static void register_(script& s) {
			s.register_member("CCAMSYS.BESTRANGE", &c_camera::best_range);
			s.register_member("CCAMSYS.MINRANGE", &c_camera::min_range);
			s.register_member("CCAMSYS.MAXRANGE", &c_camera::max_range);
			s.register_member("CCAMSYS.BESTELEVATION", &c_camera::best_elevation);
			s.register_member("CCAMSYS.MINELEVATION", &c_camera::min_elevation);
			s.register_member("CCAMSYS.MAXELEVATION", &c_camera::max_elevation);
			s.register_member("CCAMSYS.BESTAZIMUTH", &c_camera::best_azimuth);
			s.register_member("CCAMSYS.MINAZIMUTH", &c_camera::min_azimuth);
			s.register_member("CCAMSYS.MAXAZIMUTH", &c_camera::max_azimuth);
			s.register_member("CCAMSYS.BESTROTZ", &c_camera::best_rot_z);
			s.register_member("CCAMSYS.MINROTZ", &c_camera::min_rot_z);
			s.register_member("CCAMSYS.MAXROTZ", &c_camera::max_rot_z);
			s.register_member("CCAMSYS.ROTOFFSETX", &c_camera::rot_offset_x);
			s.register_member("CCAMSYS.ROTOFFSETY", &c_camera::rot_offset_y);
			s.register_member("CCAMSYS.ROTOFFSETZ", &c_camera::rot_offset_z);
			s.register_member("CCAMSYS.TARGETOFFSETX", &c_camera::target_offset_x);
			s.register_member("CCAMSYS.TARGETOFFSETY", &c_camera::target_offset_y);
			s.register_member("CCAMSYS.TARGETOFFSETZ", &c_camera::target_offset_z);
			s.register_member("CCAMSYS.VELOTRANS", &c_camera::velo_trans);
			s.register_member("CCAMSYS.VELOROT", &c_camera::velo_rot);
			s.register_member("CCAMSYS.TRANSLATE", &c_camera::translate);
			s.register_member("CCAMSYS.ROTATE", &c_camera::rotate);
			s.register_member("CCAMSYS.COLLISION", &c_camera::collision);
		}
	};

	enum class music_transition_type : std::uint32_t {
		unknown = 0,
		none = 1,
		groove = 2,
		fill = 3,
		break_ = 4,
		intro = 5,
		end = 6,
		end_and_into = 7
	};

	enum class music_transition_subtype : std::uint32_t { unknown = 0, immediate = 1, beat = 2, measure = 3 };

	struct c_music_system : public instance {
		var float volume;
		var int32_t bit_resolution;
		var int32_t global_reverb_enabled;
		var int32_t sample_rate;
		var int32_t num_channels;
		var int32_t reverb_buffer_size;

		static void register_(script& s) {
			s.register_member("C_MUSICSYS_CFG.VOLUME", &c_music_system::volume);
			s.register_member("C_MUSICSYS_CFG.BITRESOLUTION", &c_music_system::bit_resolution);
			s.register_member("C_MUSICSYS_CFG.GLOBALREVERBENABLED", &c_music_system::global_reverb_enabled);
			s.register_member("C_MUSICSYS_CFG.SAMPLERATE", &c_music_system::sample_rate);
			s.register_member("C_MUSICSYS_CFG.NUMCHANNELS", &c_music_system::num_channels);
			s.register_member("C_MUSICSYS_CFG.REVERBBUFFERSIZE", &c_music_system::reverb_buffer_size);
		}
	};

	struct c_music_theme : public instance {
		var string file;
		var float vol;
		var int32_t loop;
		var float reverbmix;
		var float reverbtime;
		var music_transition_type transtype;
		var music_transition_subtype transsubtype;

		static void register_(script& s) {
			s.register_member("C_MUSICTHEME.FILE", &c_music_theme::file);
			s.register_member("C_MUSICTHEME.VOL", &c_music_theme::vol);
			s.register_member("C_MUSICTHEME.LOOP", &c_music_theme::loop);
			s.register_member("C_MUSICTHEME.REVERBMIX", &c_music_theme::reverbmix);
			s.register_member("C_MUSICTHEME.REVERBTIME", &c_music_theme::reverbtime);
			s.register_member("C_MUSICTHEME.TRANSTYPE", &c_music_theme::transtype);
			s.register_member("C_MUSICTHEME.TRANSSUBTYPE", &c_music_theme::transsubtype);
		}
	};

	struct c_music_jingle : public instance {
		var string name;
		var int32_t loop;
		var float vol;
		var int32_t transsubtype;

		static void register_(script& s) {
			s.register_member("C_MUSICJINGLE.NAME", &c_music_jingle::name);
			s.register_member("C_MUSICJINGLE.LOOP", &c_music_jingle::loop);
			s.register_member("C_MUSICJINGLE.VOL", &c_music_jingle::vol);
			s.register_member("C_MUSICJINGLE.TRANSSUBTYPE", &c_music_jingle::transsubtype);
		}
	};

	struct c_particle_fx : public instance {
		var float pps_value;
		var string pps_scale_keys_s;
		var int32_t pps_is_looping;
		var int32_t pps_is_smooth;
		var float pps_fps;
		var string pps_create_em_s;
		var float pps_create_em_delay;
		var string shp_type_s;
		var string shp_for_s;
		var string shp_offset_vec_s;
		var string shp_distrib_type_s;
		var float shp_distrib_walk_speed;
		var int32_t shp_is_volume;
		var string shp_dim_s;
		var string shp_mesh_s;
		var int32_t shp_mesh_render_b;
		var string shp_scale_keys_s;
		var int32_t shp_scale_is_looping;
		var int32_t shp_scale_is_smooth;
		var float shp_scale_fps;
		var string dir_mode_s;
		var string dir_for_s;
		var string dir_mode_target_for_s;
		var string dir_mode_target_pos_s;
		var float dir_angle_head;
		var float dir_angle_head_var;
		var float dir_angle_elev;
		var float dir_angle_elev_var;
		var float vel_avg;
		var float vel_var;
		var float lsp_part_avg;
		var float lsp_part_var;
		var string fly_gravity_s;
		var int32_t fly_colldet_b;
		var string vis_name_s;
		var string vis_orientation_s;
		var int32_t vis_tex_is_quadpoly;
		var float vis_tex_ani_fps;
		var int32_t vis_tex_ani_is_looping;
		var string vis_tex_color_start_s;
		var string vis_tex_color_end_s;
		var string vis_size_start_s;
		var float vis_size_end_scale;
		var string vis_alpha_func_s;
		var float vis_alpha_start;
		var float vis_alpha_end;
		var float trl_fade_speed;
		var string trl_texture_s;
		var float trl_width;
		var float mrk_fades_peed;
		var string mrkt_exture_s;
		var float mrk_size;
		var string flock_mode;
		var float flock_strength;
		var int32_t use_emitters_for;
		var string time_start_end_s;
		var int32_t m_bis_ambient_pfx;

		static void register_(script& s) {
			s.register_member("C_PARTICLEFX.PPSVALUE", &c_particle_fx::pps_value);
			s.register_member("C_PARTICLEFX.PPSSCALEKEYS_S", &c_particle_fx::pps_scale_keys_s);
			s.register_member("C_PARTICLEFX.PPSISLOOPING", &c_particle_fx::pps_is_looping);
			s.register_member("C_PARTICLEFX.PPSISSMOOTH", &c_particle_fx::pps_is_smooth);
			s.register_member("C_PARTICLEFX.PPSFPS", &c_particle_fx::pps_fps);
			s.register_member("C_PARTICLEFX.PPSCREATEEM_S", &c_particle_fx::pps_create_em_s);
			s.register_member("C_PARTICLEFX.PPSCREATEEMDELAY", &c_particle_fx::pps_create_em_delay);
			s.register_member("C_PARTICLEFX.SHPTYPE_S", &c_particle_fx::shp_type_s);
			s.register_member("C_PARTICLEFX.SHPFOR_S", &c_particle_fx::shp_for_s);
			s.register_member("C_PARTICLEFX.SHPOFFSETVEC_S", &c_particle_fx::shp_offset_vec_s);
			s.register_member("C_PARTICLEFX.SHPDISTRIBTYPE_S", &c_particle_fx::shp_distrib_type_s);
			s.register_member("C_PARTICLEFX.SHPDISTRIBWALKSPEED", &c_particle_fx::shp_distrib_walk_speed);
			s.register_member("C_PARTICLEFX.SHPISVOLUME", &c_particle_fx::shp_is_volume);
			s.register_member("C_PARTICLEFX.SHPDIM_S", &c_particle_fx::shp_dim_s);
			s.register_member("C_PARTICLEFX.SHPMESH_S", &c_particle_fx::shp_mesh_s);
			s.register_member("C_PARTICLEFX.SHPMESHRENDER_B", &c_particle_fx::shp_mesh_render_b);
			s.register_member("C_PARTICLEFX.SHPSCALEKEYS_S", &c_particle_fx::shp_scale_keys_s);
			s.register_member("C_PARTICLEFX.SHPSCALEISLOOPING", &c_particle_fx::shp_scale_is_looping);
			s.register_member("C_PARTICLEFX.SHPSCALEISSMOOTH", &c_particle_fx::shp_scale_is_smooth);
			s.register_member("C_PARTICLEFX.SHPSCALEFPS", &c_particle_fx::shp_scale_fps);
			s.register_member("C_PARTICLEFX.DIRMODE_S", &c_particle_fx::dir_mode_s);
			s.register_member("C_PARTICLEFX.DIRFOR_S", &c_particle_fx::dir_for_s);
			s.register_member("C_PARTICLEFX.DIRMODETARGETFOR_S", &c_particle_fx::dir_mode_target_for_s);
			s.register_member("C_PARTICLEFX.DIRMODETARGETPOS_S", &c_particle_fx::dir_mode_target_pos_s);
			s.register_member("C_PARTICLEFX.DIRANGLEHEAD", &c_particle_fx::dir_angle_head);
			s.register_member("C_PARTICLEFX.DIRANGLEHEADVAR", &c_particle_fx::dir_angle_head_var);
			s.register_member("C_PARTICLEFX.DIRANGLEELEV", &c_particle_fx::dir_angle_elev);
			s.register_member("C_PARTICLEFX.DIRANGLEELEVVAR", &c_particle_fx::dir_angle_elev_var);
			s.register_member("C_PARTICLEFX.VELAVG", &c_particle_fx::vel_avg);
			s.register_member("C_PARTICLEFX.VELVAR", &c_particle_fx::vel_var);
			s.register_member("C_PARTICLEFX.LSPPARTAVG", &c_particle_fx::lsp_part_avg);
			s.register_member("C_PARTICLEFX.LSPPARTVAR", &c_particle_fx::lsp_part_var);
			s.register_member("C_PARTICLEFX.FLYGRAVITY_S", &c_particle_fx::fly_gravity_s);
			s.register_member("C_PARTICLEFX.FLYCOLLDET_B", &c_particle_fx::fly_colldet_b);
			s.register_member("C_PARTICLEFX.VISNAME_S", &c_particle_fx::vis_name_s);
			s.register_member("C_PARTICLEFX.VISORIENTATION_S", &c_particle_fx::vis_orientation_s);
			s.register_member("C_PARTICLEFX.VISTEXISQUADPOLY", &c_particle_fx::vis_tex_is_quadpoly);
			s.register_member("C_PARTICLEFX.VISTEXANIFPS", &c_particle_fx::vis_tex_ani_fps);
			s.register_member("C_PARTICLEFX.VISTEXANIISLOOPING", &c_particle_fx::vis_tex_ani_is_looping);
			s.register_member("C_PARTICLEFX.VISTEXCOLORSTART_S", &c_particle_fx::vis_tex_color_start_s);
			s.register_member("C_PARTICLEFX.VISTEXCOLOREND_S", &c_particle_fx::vis_tex_color_end_s);
			s.register_member("C_PARTICLEFX.VISSIZESTART_S", &c_particle_fx::vis_size_start_s);
			s.register_member("C_PARTICLEFX.VISSIZEENDSCALE", &c_particle_fx::vis_size_end_scale);
			s.register_member("C_PARTICLEFX.VISALPHAFUNC_S", &c_particle_fx::vis_alpha_func_s);
			s.register_member("C_PARTICLEFX.VISALPHASTART", &c_particle_fx::vis_alpha_start);
			s.register_member("C_PARTICLEFX.VISALPHAEND", &c_particle_fx::vis_alpha_end);
			s.register_member("C_PARTICLEFX.TRLFADESPEED", &c_particle_fx::trl_fade_speed);
			s.register_member("C_PARTICLEFX.TRLTEXTURE_S", &c_particle_fx::trl_texture_s);
			s.register_member("C_PARTICLEFX.TRLWIDTH", &c_particle_fx::trl_width);
			s.register_member("C_PARTICLEFX.MRKFADESPEED", &c_particle_fx::mrk_fades_peed);
			s.register_member("C_PARTICLEFX.MRKTEXTURE_S", &c_particle_fx::mrkt_exture_s);
			s.register_member("C_PARTICLEFX.MRKSIZE", &c_particle_fx::mrk_size);

			// Gothic 2 only
			if (s.find_symbol_by_name("C_PARTICLEFX.FLOCKMODE") != nullptr) {
				s.register_member("C_PARTICLEFX.FLOCKMODE", &c_particle_fx::flock_mode);
				s.register_member("C_PARTICLEFX.FLOCKSTRENGTH", &c_particle_fx::flock_strength);
				s.register_member("C_PARTICLEFX.USEEMITTERSFOR", &c_particle_fx::use_emitters_for);
				s.register_member("C_PARTICLEFX.TIMESTARTEND_S", &c_particle_fx::time_start_end_s);
				s.register_member("C_PARTICLEFX.M_BISAMBIENTPFX", &c_particle_fx::m_bis_ambient_pfx);
			}
		}
	};

	struct c_fx_base : public instance {
		static constexpr std::uint8_t user_string_count = 5;

		var string vis_name_s;
		var string vis_size_s;
		var float vis_alpha;
		var string vis_alpha_blend_func_s;
		var float vis_tex_ani_fps;
		var int32_t vis_tex_ani_is_looping;
		var string em_trj_mode_s;
		var string em_trj_origin_node;
		var string em_trj_target_node;
		var float em_trj_target_range;
		var float em_trj_target_azi;
		var float em_trj_target_elev;
		var int32_t em_trj_num_keys;
		var int32_t em_trj_num_keys_var;
		var float em_trj_angle_elev_var;
		var float em_trj_angle_head_var;
		var float em_trj_key_dist_var;
		var string em_trj_loop_mode_s;
		var string em_trj_ease_func_s;
		var float em_trj_ease_vel;
		var float em_trj_dyn_update_delay;
		var int32_t em_trj_dyn_update_target_only;
		var string em_fx_create_s;
		var string em_fx_invest_origin_s;
		var string em_fx_invest_target_s;
		var float em_fx_trigger_delay;
		var int32_t em_fx_create_down_trj;
		var string em_action_coll_dyn_s;
		var string em_action_coll_stat_s;
		var string em_fx_coll_stat_s;
		var string em_fx_coll_dyn_s;
		var string em_fx_coll_stat_align_s;
		var string em_fx_coll_dyn_align_s;
		var float em_fx_lifespan;
		var int32_t em_check_collision;
		var int32_t em_adjust_shp_to_origin;
		var float em_invest_next_key_duration;
		var float em_fly_gravity;
		var string em_self_rot_vel_s;
		var string user_string[user_string_count];
		var string light_preset_name;
		var string sfx_id;
		var int32_t sfx_is_ambient;
		var int32_t send_assess_magic;
		var float secs_per_damage;
		var string em_fx_coll_dyn_perc_s;

		static void register_(script& s) {
			s.register_member("CFX_BASE.VISNAME_S", &c_fx_base::vis_name_s);
			s.register_member("CFX_BASE.VISSIZE_S", &c_fx_base::vis_size_s);
			s.register_member("CFX_BASE.VISALPHA", &c_fx_base::vis_alpha);
			s.register_member("CFX_BASE.VISALPHABLENDFUNC_S", &c_fx_base::vis_alpha_blend_func_s);
			s.register_member("CFX_BASE.VISTEXANIFPS", &c_fx_base::vis_tex_ani_fps);
			s.register_member("CFX_BASE.VISTEXANIISLOOPING", &c_fx_base::vis_tex_ani_is_looping);
			s.register_member("CFX_BASE.EMTRJMODE_S", &c_fx_base::em_trj_mode_s);
			s.register_member("CFX_BASE.EMTRJORIGINNODE", &c_fx_base::em_trj_origin_node);
			s.register_member("CFX_BASE.EMTRJTARGETNODE", &c_fx_base::em_trj_target_node);
			s.register_member("CFX_BASE.EMTRJTARGETRANGE", &c_fx_base::em_trj_target_range);
			s.register_member("CFX_BASE.EMTRJTARGETAZI", &c_fx_base::em_trj_target_azi);
			s.register_member("CFX_BASE.EMTRJTARGETELEV", &c_fx_base::em_trj_target_elev);
			s.register_member("CFX_BASE.EMTRJNUMKEYS", &c_fx_base::em_trj_num_keys);
			s.register_member("CFX_BASE.EMTRJNUMKEYSVAR", &c_fx_base::em_trj_num_keys_var);
			s.register_member("CFX_BASE.EMTRJANGLEELEVVAR", &c_fx_base::em_trj_angle_elev_var);
			s.register_member("CFX_BASE.EMTRJANGLEHEADVAR", &c_fx_base::em_trj_angle_head_var);
			s.register_member("CFX_BASE.EMTRJKEYDISTVAR", &c_fx_base::em_trj_key_dist_var);
			s.register_member("CFX_BASE.EMTRJLOOPMODE_S", &c_fx_base::em_trj_loop_mode_s);
			s.register_member("CFX_BASE.EMTRJEASEFUNC_S", &c_fx_base::em_trj_ease_func_s);
			s.register_member("CFX_BASE.EMTRJEASEVEL", &c_fx_base::em_trj_ease_vel);
			s.register_member("CFX_BASE.EMTRJDYNUPDATEDELAY", &c_fx_base::em_trj_dyn_update_delay);
			s.register_member("CFX_BASE.EMTRJDYNUPDATETARGETONLY", &c_fx_base::em_trj_dyn_update_target_only);
			s.register_member("CFX_BASE.EMFXCREATE_S", &c_fx_base::em_fx_create_s);
			s.register_member("CFX_BASE.EMFXINVESTORIGIN_S", &c_fx_base::em_fx_invest_origin_s);
			s.register_member("CFX_BASE.EMFXINVESTTARGET_S", &c_fx_base::em_fx_invest_target_s);
			s.register_member("CFX_BASE.EMFXTRIGGERDELAY", &c_fx_base::em_fx_trigger_delay);
			s.register_member("CFX_BASE.EMFXCREATEDOWNTRJ", &c_fx_base::em_fx_create_down_trj);
			s.register_member("CFX_BASE.EMACTIONCOLLDYN_S", &c_fx_base::em_action_coll_dyn_s);
			s.register_member("CFX_BASE.EMACTIONCOLLSTAT_S", &c_fx_base::em_action_coll_stat_s);
			s.register_member("CFX_BASE.EMFXCOLLSTAT_S", &c_fx_base::em_fx_coll_stat_s);
			s.register_member("CFX_BASE.EMFXCOLLDYN_S", &c_fx_base::em_fx_coll_dyn_s);
			s.register_member("CFX_BASE.EMFXCOLLSTATALIGN_S", &c_fx_base::em_fx_coll_stat_align_s);
			s.register_member("CFX_BASE.EMFXCOLLDYNALIGN_S", &c_fx_base::em_fx_coll_dyn_align_s);
			s.register_member("CFX_BASE.EMFXLIFESPAN", &c_fx_base::em_fx_lifespan);
			s.register_member("CFX_BASE.EMCHECKCOLLISION", &c_fx_base::em_check_collision);
			s.register_member("CFX_BASE.EMADJUSTSHPTOORIGIN", &c_fx_base::em_adjust_shp_to_origin);
			s.register_member("CFX_BASE.EMINVESTNEXTKEYDURATION", &c_fx_base::em_invest_next_key_duration);
			s.register_member("CFX_BASE.EMFLYGRAVITY", &c_fx_base::em_fly_gravity);
			s.register_member("CFX_BASE.EMSELFROTVEL_S", &c_fx_base::em_self_rot_vel_s);
			s.register_member("CFX_BASE.USERSTRING", &c_fx_base::user_string);
			s.register_member("CFX_BASE.LIGHTPRESETNAME", &c_fx_base::light_preset_name);
			s.register_member("CFX_BASE.SFXID", &c_fx_base::sfx_id);
			s.register_member("CFX_BASE.SFXISAMBIENT", &c_fx_base::sfx_is_ambient);
			s.register_member("CFX_BASE.SENDASSESSMAGIC", &c_fx_base::send_assess_magic);
			s.register_member("CFX_BASE.SECSPERDAMAGE", &c_fx_base::secs_per_damage);

			// Gothic 2 only
			if (s.find_symbol_by_name("CFX_BASE.EMFXCOLLDYNPERC_S") != nullptr)
				s.register_member("CFX_BASE.EMFXCOLLDYNPERC_S", &c_fx_base::em_fx_coll_dyn_perc_s);
		}
	};

	struct c_particle_fx_emit_key : public instance {
		var string vis_name_s;
		var float vis_size_scale;
		var float scale_duration;
		var float pfx_pps_value;
		var int32_t pfx_pps_is_smooth_chg;
		var int32_t pfx_pps_is_looping_chg;
		var float pfx_sc_time;
		var string pfx_fly_gravity_s;
		var string pfx_shp_dim_s;
		var int32_t pfx_shp_is_volume_chg;
		var float pfx_shp_scale_fps;
		var float pfx_shp_distrib_walks_peed;
		var string pfx_shp_offset_vec_s;
		var string pfx_shp_distrib_type_s;
		var string pfx_dir_mode_s;
		var string pfx_dir_for_s;
		var string pfx_dir_mode_target_for_s;
		var string pfx_dir_mode_target_pos_s;
		var float pfx_vel_avg;
		var float pfx_lsp_part_avg;
		var float pfx_vis_alpha_start;
		var string light_preset_name;
		var float light_range;
		var string sfx_id;
		var int32_t sfx_is_ambient;
		var string em_create_fx_id;
		var float em_fly_gravity;
		var string em_self_rot_vel_s;
		var string em_trj_mode_s;
		var float em_trj_ease_vel;
		var int32_t em_check_collision;
		var float em_fx_lifespan;

		static void register_(script& s) {
			s.register_member("C_PARTICLEFXEMITKEY.VISNAME_S", &c_particle_fx_emit_key::vis_name_s);
			s.register_member("C_PARTICLEFXEMITKEY.VISSIZESCALE", &c_particle_fx_emit_key::vis_size_scale);
			s.register_member("C_PARTICLEFXEMITKEY.SCALEDURATION", &c_particle_fx_emit_key::scale_duration);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_PPSVALUE", &c_particle_fx_emit_key::pfx_pps_value);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_PPSISSMOOTHCHG", &c_particle_fx_emit_key::pfx_pps_is_smooth_chg);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_PPSISLOOPINGCHG",
			                  &c_particle_fx_emit_key::pfx_pps_is_looping_chg);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_SCTIME", &c_particle_fx_emit_key::pfx_sc_time);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_FLYGRAVITY_S", &c_particle_fx_emit_key::pfx_fly_gravity_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPDIM_S", &c_particle_fx_emit_key::pfx_shp_dim_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPISVOLUMECHG", &c_particle_fx_emit_key::pfx_shp_is_volume_chg);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPSCALEFPS", &c_particle_fx_emit_key::pfx_shp_scale_fps);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPDISTRIBWALKSPEED",
			                  &c_particle_fx_emit_key::pfx_shp_distrib_walks_peed);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPOFFSETVEC_S", &c_particle_fx_emit_key::pfx_shp_offset_vec_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_SHPDISTRIBTYPE_S",
			                  &c_particle_fx_emit_key::pfx_shp_distrib_type_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRMODE_S", &c_particle_fx_emit_key::pfx_dir_mode_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRFOR_S", &c_particle_fx_emit_key::pfx_dir_for_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRMODETARGETFOR_S",
			                  &c_particle_fx_emit_key::pfx_dir_mode_target_for_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_DIRMODETARGETPOS_S",
			                  &c_particle_fx_emit_key::pfx_dir_mode_target_pos_s);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_VELAVG", &c_particle_fx_emit_key::pfx_vel_avg);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_LSPPARTAVG", &c_particle_fx_emit_key::pfx_lsp_part_avg);
			s.register_member("C_PARTICLEFXEMITKEY.PFX_VISALPHASTART", &c_particle_fx_emit_key::pfx_vis_alpha_start);
			s.register_member("C_PARTICLEFXEMITKEY.LIGHTPRESETNAME", &c_particle_fx_emit_key::light_preset_name);
			s.register_member("C_PARTICLEFXEMITKEY.LIGHTRANGE", &c_particle_fx_emit_key::light_range);
			s.register_member("C_PARTICLEFXEMITKEY.SFXID", &c_particle_fx_emit_key::sfx_id);
			s.register_member("C_PARTICLEFXEMITKEY.SFXISAMBIENT", &c_particle_fx_emit_key::sfx_is_ambient);
			s.register_member("C_PARTICLEFXEMITKEY.EMCREATEFXID", &c_particle_fx_emit_key::em_create_fx_id);
			s.register_member("C_PARTICLEFXEMITKEY.EMFLYGRAVITY", &c_particle_fx_emit_key::em_fly_gravity);
			s.register_member("C_PARTICLEFXEMITKEY.EMSELFROTVEL_S", &c_particle_fx_emit_key::em_self_rot_vel_s);
			s.register_member("C_PARTICLEFXEMITKEY.EMTRJMODE_S", &c_particle_fx_emit_key::em_trj_mode_s);
			s.register_member("C_PARTICLEFXEMITKEY.EMTRJEASEVEL", &c_particle_fx_emit_key::em_trj_ease_vel);
			s.register_member("C_PARTICLEFXEMITKEY.EMCHECKCOLLISION", &c_particle_fx_emit_key::em_check_collision);
			s.register_member("C_PARTICLEFXEMITKEY.EMFXLIFESPAN", &c_particle_fx_emit_key::em_fx_lifespan);
		}
	};

	enum class c_fight_ai_move : std::uint32_t {
		nop = 0,
		run = 1,
		run_back = 2,
		jump_back = 3,
		turn = 4,
		strafe = 5,
		attack = 6,
		attack_side = 7,
		attack_front = 8,
		attack_triple = 9,
		attack_whirl = 10,
		attack_master = 11,
		turn_to_hit = 15,
		parry = 17,
		stand_up = 18,
		wait = 19,
		wait_longer = 23,
		wait_ext = 24,
	};

	struct c_fight_ai : public instance {
		static constexpr std::uint32_t move_count = 6;

		var c_fight_ai_move move[move_count];

		static void register_(script& s) {
			s.register_member("C_FIGHTAI.MOVE", &c_fight_ai::move);
		}
	};

	struct c_sfx : public instance {
		var string file;
		var int32_t pitch_off;
		var int32_t pitch_var;
		var int32_t vol;
		var int32_t loop;
		var int32_t loop_start_offset;
		var int32_t loop_end_offset;
		var float reverb_level;
		var string pfx_name;

		static void register_(script& s) {
			s.register_member("C_SFX.FILE", &c_sfx::file);
			s.register_member("C_SFX.PITCHOFF", &c_sfx::pitch_off);
			s.register_member("C_SFX.PITCHVAR", &c_sfx::pitch_var);
			s.register_member("C_SFX.VOL", &c_sfx::vol);
			s.register_member("C_SFX.LOOP", &c_sfx::loop);
			s.register_member("C_SFX.LOOPSTARTOFFSET", &c_sfx::loop_start_offset);
			s.register_member("C_SFX.LOOPENDOFFSET", &c_sfx::loop_end_offset);
			s.register_member("C_SFX.REVERBLEVEL", &c_sfx::reverb_level);
			s.register_member("C_SFX.PFXNAME", &c_sfx::pfx_name);
		}
	};

	struct c_sound_system : public instance {
		var float volume;
		var int32_t bit_resolution;
		var int32_t sample_rate;
		var int32_t use_stereo;
		var int32_t num_sfx_channels;
		var string used_3d_provider_name;

		static void register_(script& s) {
			s.register_member("C_SNDSYS_CFG.VOLUME", &c_sound_system::volume);
			s.register_member("C_SNDSYS_CFG.BITRESOLUTION", &c_sound_system::bit_resolution);
			s.register_member("C_SNDSYS_CFG.SAMPLERATE", &c_sound_system::sample_rate);
			s.register_member("C_SNDSYS_CFG.USESTEREO", &c_sound_system::use_stereo);
			s.register_member("C_SNDSYS_CFG.NUMSFXCHANNELS", &c_sound_system::num_sfx_channels);
			s.register_member("C_SNDSYS_CFG.USED3DPROVIDERNAME", &c_sound_system::used_3d_provider_name);
		}
	};

	void register_all_script_classes(script& s);
} // namespace phoenix

#undef var
#undef string
#undef func
