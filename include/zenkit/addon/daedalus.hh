// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/DaedalusScript.hh"
#include "zenkit/Library.hh"

#include <cstdint>

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

namespace zenkit {
	struct IGuildValues : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	namespace DamageType {
		static constexpr std::uint32_t BARRIER = 0U;
		static constexpr std::uint32_t BLUNT = 1U;
		static constexpr std::uint32_t EDGE = 2U;
		static constexpr std::uint32_t FIRE = 3U;
		static constexpr std::uint32_t FLY = 4U;
		static constexpr std::uint32_t MAGIC = 5U;
		static constexpr std::uint32_t POINT = 6U;
		static constexpr std::uint32_t FALL = 7U;

		static constexpr std::uint32_t _NUM = 8U;

		static constexpr std::uint32_t barrier ZKREM("renamed to DamageType::BARRIER") = BARRIER;
		static constexpr std::uint32_t blunt ZKREM("renamed to DamageType::BLUNT") = BLUNT;
		static constexpr std::uint32_t edge ZKREM("renamed to DamageType::EDGE") = EDGE;
		static constexpr std::uint32_t fire ZKREM("renamed to DamageType::FIRE") = FIRE;
		static constexpr std::uint32_t fly ZKREM("renamed to DamageType::FLY") = FLY;
		static constexpr std::uint32_t magic ZKREM("renamed to DamageType::MAGIC") = MAGIC;
		static constexpr std::uint32_t point ZKREM("renamed to DamageType::POINT") = POINT;
		static constexpr std::uint32_t fall ZKREM("renamed to DamageType::FALL") = FALL;
		static constexpr std::uint32_t count ZKREM("renamed to DamageType::_NUM") = _NUM;
	} // namespace DamageType

	namespace NpcAttribute {
		static constexpr std::uint32_t HITPOINTS = 0U;
		static constexpr std::uint32_t HITPOINTS_MAX = 1U;
		static constexpr std::uint32_t MANA = 2U;
		static constexpr std::uint32_t MANA_MAX = 3U;
		static constexpr std::uint32_t STRENGTH = 4U;
		static constexpr std::uint32_t DEXTERITY = 5U;
		static constexpr std::uint32_t REGENERATE_HP = 6U;
		static constexpr std::uint32_t REGENERATE_MANA = 7U;

		static constexpr std::uint32_t _NUM = 8U;

		static constexpr std::uint32_t hitpoints ZKREM("renamed to NpcAttribute::HITPOINTS") = HITPOINTS;
		static constexpr std::uint32_t hitpoints_max ZKREM("renamed to NpcAttribute::HITPOINTS_MAX") = HITPOINTS_MAX;
		static constexpr std::uint32_t mana ZKREM("renamed to NpcAttribute::MANA") = MANA;
		static constexpr std::uint32_t mana_max ZKREM("renamed to NpcAttribute::MANA_MAX") = MANA_MAX;
		static constexpr std::uint32_t strength ZKREM("renamed to NpcAttribute::STRENGTH") = STRENGTH;
		static constexpr std::uint32_t dexterity ZKREM("renamed to NpcAttribute::DEXTERITY") = DEXTERITY;
		static constexpr std::uint32_t regenerate_hp ZKREM("renamed to NpcAttribute::REGENERATE_HP") = REGENERATE_HP;
		static constexpr std::uint32_t
		    regenerate_mana ZKREM("renamed to NpcAttribute::REGENERATE_MANA") = REGENERATE_MANA;
		static constexpr std::uint32_t count ZKREM("renamed to NpcAttribute::_NUM") = _NUM;
	} // namespace NpcAttribute

	enum class NpcType : std::uint32_t {
		G1_AMBIENT = 0U,
		G1_MAIN = 1U,
		G1_GUARD = 2U,
		G1_FRIEND = 3U,
		G1_MINE_AMBIENT = 4U,
		G1_MINE_GUARD = 5U,
		G1_OW_AMBIENT = 6U,
		G1_OW_GUARD = 7U,
		G1_ROGUE = 8U,

		G2_AMBIENT = 0U,
		G2_MAIN = 1U,
		G2_FRIEND = 2U,
		G2_OC_AMBIENT = 3U,
		G2_OC_MAIN = 4U,
		G2_BL_AMBIENT = 5U,
		G2_TAL_AMBIENT = 6U,
		G2_BL_MAIN = 7U,

		AMBIENT = G2_AMBIENT,
		MAIN = G2_MAIN,

		g1_ambient ZKREM("renamed to NpcType::G1_AMBIENT") = G1_AMBIENT,
		g1_main ZKREM("renamed to NpcType::G1_MAIN") = G1_MAIN,
		g1_guard ZKREM("renamed to NpcType::G1_GUARD") = G1_GUARD,
		g1_friend ZKREM("renamed to NpcType::G1_FRIEND") = G1_FRIEND,
		g1_mine_ambient ZKREM("renamed to NpcType::G1_MINE_AMBIENT") = G1_MINE_AMBIENT,
		g1_mine_guard ZKREM("renamed to NpcType::G1_MINE_GUARD") = G1_MINE_GUARD,
		g1_ow_ambient ZKREM("renamed to NpcType::G1_OW_AMBIENT") = G1_OW_AMBIENT,
		g1_ow_guard ZKREM("renamed to NpcType::G1_OW_GUARD") = G1_OW_GUARD,
		g1_rogue ZKREM("renamed to NpcType::G1_ROGUE") = G1_ROGUE,

		g2_ambient ZKREM("renamed to NpcType::G2_AMBIENT") = G2_AMBIENT,
		g2_main ZKREM("renamed to NpcType::G2_MAIN") = G2_MAIN,
		g2_friend ZKREM("renamed to NpcType::G2_FRIEND") = G2_FRIEND,
		g2_oc_ambient ZKREM("renamed to NpcType::G2_OC_AMBIENT") = G2_OC_AMBIENT,
		g2_oc_main ZKREM("renamed to NpcType::G2_OC_MAIN") = G2_OC_MAIN,
		g2_bl_ambient ZKREM("renamed to NpcType::G2_BL_AMBIENT") = G2_BL_AMBIENT,
		g2_tal_ambient ZKREM("renamed to NpcType::G2_TAL_AMBIENT") = G2_TAL_AMBIENT,
		g2_bl_main ZKREM("renamed to NpcType::G2_BL_MAIN") = G2_BL_MAIN,

		ambient ZKREM("renamed to NpcType::AMBIENT") = AMBIENT,
		main ZKREM("renamed to NpcType::MAIN") = MAIN,

		friend_ ZKREM("use NpcType::G2_FRIEND") = G2_FRIEND,
		oc_ambient ZKREM("use NpcType::G2_OC_AMBIENT") = G2_OC_AMBIENT,
		oc_main ZKREM("use NpcType::G2_OC_MAIN") = G2_OC_MAIN,
		bl_ambient ZKREM("use NpcType::G2_BL_AMBIENT") = G2_BL_AMBIENT,
		tal_ambient ZKREM("use NpcType::G2_TAL_AMBIENT") = G2_TAL_AMBIENT,
		bl_main ZKREM("use NpcType::G2_BL_MAIN") = G2_BL_MAIN,
	};

	static_assert(NpcType::G1_AMBIENT == NpcType::G2_AMBIENT);
	static_assert(NpcType::G1_MAIN == NpcType::G2_MAIN);

	enum class NpcFlag : std::uint32_t {
		NONE = 0U,
		FRIENDS = 1U << 0U,
		IMMORTAL = 1U << 1U,
		GHOST = 1U << 2U,
		PROTECTED = 1U << 3U,

		none ZKREM("renamed to zenkit::NONE") = NONE,
		friends ZKREM("renamed to zenkit::FRIENDS") = FRIENDS,
		immortal ZKREM("renamed to zenkit::IMMORTAL") = IMMORTAL,
		ghost ZKREM("renamed to zenkit::GHOST") = GHOST,
		protected_ ZKREM("renamed to zenkit::PROTECTED") = PROTECTED,
	};
	FLAG(NpcFlag)

	struct INpc : public DaedalusInstance {
		static constexpr std::uint32_t hitchance_count =
		    5; // -> TODO: one of "unknown", "one-handed", "two-handed", "bow", "crossbow"
		static constexpr std::uint32_t name_count = 5;
		static constexpr std::uint32_t mission_count = 5;
		static constexpr std::uint32_t aivar_count = 100;

		var int32_t id;
		var string name[name_count];
		var string slot;
		var string effect;
		var NpcType type;
		var NpcFlag flags;
		var int32_t attribute[NpcAttribute::_NUM];
		var int32_t hitchance[hitchance_count];
		var int32_t protection[DamageType::_NUM];
		var int32_t damage[DamageType::_NUM];
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IMission : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	enum class ItemFlag : std::uint32_t {
		DAGGER = 1U << 13U,
		SWORD = 1U << 14U,
		AXE = 1U << 15U,
		TWO_HANDED_SWORD = 1U << 16U,
		TWO_HANDED_AXE = 1U << 17U,
		BOW = 1U << 19U,
		CROSSBOW = 1U << 20U,
		AMULET = 1U << 22U,
		RING = 1U << 11U,
		BELT = 1U << 24U,
		MISSION = 1U << 12U,

		dagger ZKREM("renamed to zenkit::DAGGER") = DAGGER,
		sword ZKREM("renamed to zenkit::SWORD") = SWORD,
		axe ZKREM("renamed to zenkit::AXE") = AXE,
		two_handed_sword ZKREM("renamed to zenkit::TWO_HANDED_SWORD") = TWO_HANDED_SWORD,
		two_handed_axe ZKREM("renamed to zenkit::TWO_HANDED_AXE") = TWO_HANDED_AXE,
		bow ZKREM("renamed to zenkit::BOW") = BOW,
		crossbow ZKREM("renamed to zenkit::CROSSBOW") = CROSSBOW,
		amulet ZKREM("renamed to zenkit::AMULET") = AMULET,
		ring ZKREM("renamed to zenkit::RING") = RING,
		belt ZKREM("renamed to zenkit::BELT") = BELT,
		mission ZKREM("renamed to zenkit::MISSION") = MISSION,
	};
	FLAG(ItemFlag)

	struct IItem : public DaedalusInstance {
		static constexpr std::uint32_t condition_count = 3;
		static constexpr std::uint32_t state_count = 4;
		static constexpr std::uint32_t text_count = 6;

		var int32_t id;
		var string name;
		var string name_id;
		var int32_t hp;
		var int32_t hp_max;
		var int32_t main_flag;
		var ItemFlag flags;
		var int32_t weight;
		var int32_t value;
		var int32_t damage_type;
		var int32_t damage_total;
		var int32_t damage[DamageType::_NUM];
		var int32_t wear;
		var int32_t protection[DamageType::_NUM];
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
		var string text[text_count];
		var int32_t count[text_count];
		var int32_t inv_zbias;
		var int32_t inv_rot_x;
		var int32_t inv_rot_y;
		var int32_t inv_rot_z;
		var int32_t inv_animate;

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IFocus : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IInfoChoice {
		var string text;
		var int32_t function = 0;
	};

	struct IInfo : public DaedalusInstance {
		var int32_t npc;
		var int32_t nr;
		var int32_t important;
		var func condition;
		var func information;
		var string description;
		var int32_t trade;
		var int32_t permanent;

		std::vector<IInfoChoice> choices {};

		ZKAPI void add_choice(IInfoChoice const& ch);
		ZKAPI void remove_choice(std::size_t index);

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IItemReact : public DaedalusInstance {
		var int32_t npc;
		var int32_t trade_item;
		var int32_t trade_amount;
		var int32_t requested_cat;
		var int32_t requested_item;
		var int32_t requested_amount;
		var func reaction;

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct ISpell : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct ISvm : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	enum class MenuFlag : std::uint32_t {
		OVERTOP = 1 << 0,
		EXCLUSIVE = 1 << 1,
		NO_ANIMATION = 1 << 2,
		DONT_SCALE_DIMENSION = 1 << 3,
		DONT_SCALE_POSITION = 1 << 4,
		ALIGN_CENTER = 1 << 5,
		SHOW_INFO = 1 << 6,

		overtop ZKREM("renamed to MenuFlag::OVERTOP") = OVERTOP,
		exclusive ZKREM("renamed to MenuFlag::EXCLUSIVE") = EXCLUSIVE,
		no_animation ZKREM("renamed to MenuFlag::NO_ANIMATION") = NO_ANIMATION,
		dont_scale_dimension ZKREM("renamed to MenuFlag::DONT_SCALE_DIMENSION") = DONT_SCALE_DIMENSION,
		dont_scale_position ZKREM("renamed to MenuFlag::DONT_SCALE_POSITION") = DONT_SCALE_POSITION,
		align_center ZKREM("renamed to MenuFlag::ALIGN_CENTER") = ALIGN_CENTER,
		show_info ZKREM("renamed to MenuFlag::SHOW_INFO") = SHOW_INFO,
	};
	FLAG(MenuFlag)

	struct IMenu : public DaedalusInstance {
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
		var MenuFlag flags;
		var int32_t default_outgame;
		var int32_t default_ingame;

		ZKAPI static void register_(DaedalusScript& s);
	};

	enum class MenuItemFlag : std::uint32_t {
		CHROMAKEYED = 1 << 0,
		TRANSPARENT = 1 << 1,
		SELECTABLE = 1 << 2,
		MOVABLE = 1 << 3,
		CENTERED = 1 << 4,
		DISABLED = 1 << 5,
		FADE = 1 << 6,
		EFFECTS = 1 << 7,
		ONLY_OUTGAME = 1 << 8,
		ONLY_INGAME = 1 << 9,
		PERF_OPTION = 1 << 10,
		MULTILINE = 1 << 11,
		NEEDS_APPLY = 1 << 12,
		NEEDS_RESTART = 1 << 13,
		EXTENDED_MENU = 1 << 14,
		HOR_SELECTABLE = 1 << 15,

		chromakeyed ZKREM("renamed to MenuItemFlag::CHROMAKEYED") = CHROMAKEYED,
		transparent ZKREM("renamed to MenuItemFlag::TRANSPARENT") = TRANSPARENT,
		selectable ZKREM("renamed to MenuItemFlag::SELECTABLE") = SELECTABLE,
		movable ZKREM("renamed to MenuItemFlag::MOVABLE") = MOVABLE,
		centered ZKREM("renamed to MenuItemFlag::CENTERED") = CENTERED,
		disabled ZKREM("renamed to MenuItemFlag::DISABLED") = DISABLED,
		fade ZKREM("renamed to MenuItemFlag::FADE") = FADE,
		effects ZKREM("renamed to MenuItemFlag::EFFECTS") = EFFECTS,
		only_outgame ZKREM("renamed to MenuItemFlag::ONLY_OUTGAME") = ONLY_OUTGAME,
		only_ingame ZKREM("renamed to MenuItemFlag::ONLY_INGAME") = ONLY_INGAME,
		perf_option ZKREM("renamed to MenuItemFlag::PERF_OPTION") = PERF_OPTION,
		multiline ZKREM("renamed to MenuItemFlag::MULTILINE") = MULTILINE,
		needs_apply ZKREM("renamed to MenuItemFlag::NEEDS_APPLY") = NEEDS_APPLY,
		needs_restart ZKREM("renamed to MenuItemFlag::NEEDS_RESTART") = NEEDS_RESTART,
		extended_menu ZKREM("renamed to MenuItemFlag::EXTENDED_MENU") = EXTENDED_MENU,
		hor_selectable ZKREM("renamed to MenuItemFlag::HOR_SELECTABLE") = HOR_SELECTABLE,
	};
	FLAG(MenuItemFlag)

	enum class MenuItemType : std::uint32_t {
		UNKNOWN = 0,
		TEXT = 1,
		SLIDER = 2,
		INPUT = 3,
		CURSOR = 4,
		CHOICEBOX = 5,
		BUTTON = 6,
		LISTBOX = 7,

		unknown ZKREM("renamed to MenuItemType::UNKNOWN") = UNKNOWN,
		text ZKREM("renamed to MenuItemType::TEXT") = TEXT,
		slider ZKREM("renamed to MenuItemType::SLIDER") = SLIDER,
		input ZKREM("renamed to MenuItemType::INPUT") = INPUT,
		cursor ZKREM("renamed to MenuItemType::CURSOR") = CURSOR,
		choicebox ZKREM("renamed to MenuItemType::CHOICEBOX") = CHOICEBOX,
		button ZKREM("renamed to MenuItemType::BUTTON") = BUTTON,
		listbox ZKREM("renamed to MenuItemType::LISTBOX") = LISTBOX,
	};

	enum class MenuItemSelectEvent : uint8_t {
		EXECUTE = 1,
		CHANGED = 2,
		LEAVE = 3,
		TIMER = 4,
		CLOSE = 5,
		INIT = 6,
		SELECT_PREVIOUS = 7,
		SELECT_NEXT = 8,

		execute ZKREM("renamed to MenuItemSelectEvent::EXECUTE") = EXECUTE,
		changed ZKREM("renamed to MenuItemSelectEvent::CHANGED") = CHANGED,
		leave ZKREM("renamed to MenuItemSelectEvent::LEAVE") = LEAVE,
		timer ZKREM("renamed to MenuItemSelectEvent::TIMER") = TIMER,
		close ZKREM("renamed to MenuItemSelectEvent::CLOSE") = CLOSE,
		init ZKREM("renamed to MenuItemSelectEvent::INIT") = INIT,
		select_previous ZKREM("renamed to MenuItemSelectEvent::SELECT_PREVIOUS") = SELECT_PREVIOUS,
		select_next ZKREM("renamed to MenuItemSelectEvent::SELECT_NEXT") = SELECT_NEXT,
	};

	enum class MenuItemSelectAction : uint8_t {
		UNKNOWN = 0,
		BACK = 1,
		START_MENU = 2,
		START_ITEM = 3,
		CLOSE = 4,
		CON_COMMANDS = 5,
		PLAY_SOUND = 6,
		EXECUTE_COMMANDS = 7,

		unknown ZKREM("renamed to MenuItemSelectAction::UNKNOWN") = UNKNOWN,
		back ZKREM("renamed to MenuItemSelectAction::BACK") = BACK,
		start_menu ZKREM("renamed to MenuItemSelectAction::START_MENU") = START_MENU,
		start_item ZKREM("renamed to MenuItemSelectAction::START_ITEM") = START_ITEM,
		close ZKREM("renamed to MenuItemSelectAction::CLOSE") = CLOSE,
		con_commands ZKREM("renamed to MenuItemSelectAction::CON_COMMANDS") = CON_COMMANDS,
		play_sound ZKREM("renamed to MenuItemSelectAction::PLAY_SOUND") = PLAY_SOUND,
		execute_commands ZKREM("renamed to MenuItemSelectAction::EXECUTE_COMMANDS") = EXECUTE_COMMANDS,
	};

	struct IMenuItem : public DaedalusInstance {
		static constexpr std::uint32_t text_count = 10;
		static constexpr std::uint32_t select_action_count = 5;
		static constexpr std::uint32_t event_action_count = 10;
		static constexpr std::uint32_t user_item_count = 4;

		var string fontname;
		var string text[text_count];
		var string backpic;
		var string alphamode;
		var int32_t alpha;
		var MenuItemType type;
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
		var MenuItemFlag flags;
		var float open_delay_time;
		var float open_duration;
		var float user_float[user_item_count];
		var string user_string[user_item_count];
		var int32_t frame_posx;
		var int32_t frame_posy;
		var int32_t frame_sizex;
		var int32_t frame_sizey;
		var string hide_if_option_section_set;
		var string hide_if_option_set;
		var int32_t hide_on_value;

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct ICamera : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	enum class MusicTransitionEffect : std::uint32_t {
		UNKNOWN = 0,
		NONE = 1,
		GROOVE = 2,
		FILL = 3,
		BREAK = 4,
		INTRO = 5,
		END = 6,
		END_AND_INTO = 7,

		unknown ZKREM("renamed to MusicTransitionEffect::UNKNOWN") = UNKNOWN,
		none ZKREM("renamed to MusicTransitionEffect::NONE") = NONE,
		groove ZKREM("renamed to MusicTransitionEffect::GROOVE") = GROOVE,
		fill ZKREM("renamed to MusicTransitionEffect::FILL") = FILL,
		break_ ZKREM("renamed to MusicTransitionEffect::BREAK") = BREAK,
		intro ZKREM("renamed to MusicTransitionEffect::INTRO") = INTRO,
		end ZKREM("renamed to MusicTransitionEffect::END") = END,
		end_and_into ZKREM("renamed to MusicTransitionEffect::END_AND_INTO") = END_AND_INTO,
	};

	enum class MusicTransitionType : std::uint32_t {
		UNKNOWN = 0,
		IMMEDIATE = 1,
		BEAT = 2,
		MEASURE = 3,

		unknown ZKREM("renamed to MusicTransitionType::UNKNOWN") = UNKNOWN,
		immediate ZKREM("renamed to MusicTransitionType::IMMEDIATE") = IMMEDIATE,
		beat ZKREM("renamed to MusicTransitionType::BEAT") = BEAT,
		measure ZKREM("renamed to MusicTransitionType::MEASURE") = MEASURE,
	};

	struct IMusicSystem : public DaedalusInstance {
		var float volume;
		var int32_t bit_resolution;
		var int32_t global_reverb_enabled;
		var int32_t sample_rate;
		var int32_t num_channels;
		var int32_t reverb_buffer_size;

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IMusicTheme : public DaedalusInstance {
		var string file;
		var float vol;
		var int32_t loop;
		var float reverbmix;
		var float reverbtime;
		var MusicTransitionEffect transtype;
		var MusicTransitionType transsubtype;

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IMusicJingle : public DaedalusInstance {
		var string name;
		var int32_t loop;
		var float vol;
		var int32_t transsubtype;

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IParticleEffect : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IEffectBase : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct IParticleEffectEmitKey : public DaedalusInstance {
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

		ZKAPI static void register_(DaedalusScript& s);
	};

	enum class FightAiMove : std::uint32_t {
		NOP = 0,
		RUN = 1,
		RUN_BACK = 2,
		JUMP_BACK = 3,
		TURN = 4,
		STRAFE = 5,
		ATTACK = 6,
		ATTACK_SIDE = 7,
		ATTACK_FRONT = 8,
		ATTACK_TRIPLE = 9,
		ATTACK_WHIRL = 10,
		ATTACK_MASTER = 11,
		TURN_TO_HIT = 15,
		PARRY = 17,
		STAND_UP = 18,
		WAIT = 19,
		WAIT_LONGER = 23,
		WAIT_EXT = 24,

		nop ZKREM("renamed to zenkit::NOP") = NOP,
		run ZKREM("renamed to zenkit::RUN") = RUN,
		run_back ZKREM("renamed to zenkit::RUN_BACK") = RUN_BACK,
		jump_back ZKREM("renamed to zenkit::JUMP_BACK") = JUMP_BACK,
		turn ZKREM("renamed to zenkit::TURN") = TURN,
		strafe ZKREM("renamed to zenkit::STRAFE") = STRAFE,
		attack ZKREM("renamed to zenkit::ATTACK") = ATTACK,
		attack_side ZKREM("renamed to zenkit::ATTACK_SIDE") = ATTACK_SIDE,
		attack_front ZKREM("renamed to zenkit::ATTACK_FRONT") = ATTACK_FRONT,
		attack_triple ZKREM("renamed to zenkit::ATTACK_TRIPLE") = ATTACK_TRIPLE,
		attack_whirl ZKREM("renamed to zenkit::ATTACK_WHIRL") = ATTACK_WHIRL,
		attack_master ZKREM("renamed to zenkit::ATTACK_MASTER") = ATTACK_MASTER,
		turn_to_hit ZKREM("renamed to zenkit::TURN_TO_HIT") = TURN_TO_HIT,
		parry ZKREM("renamed to zenkit::PARRY") = PARRY,
		stand_up ZKREM("renamed to zenkit::STAND_UP") = STAND_UP,
		wait ZKREM("renamed to zenkit::WAIT") = WAIT,
		wait_longer ZKREM("renamed to zenkit::WAIT_LONGER") = WAIT_LONGER,
		wait_ext ZKREM("renamed to zenkit::WAIT_EXT") = WAIT_EXT,
	};

	struct IFightAi : public DaedalusInstance {
		static constexpr std::uint32_t move_count = 6;

		var FightAiMove move[move_count];

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct ISoundEffect : public DaedalusInstance {
		var string file;
		var int32_t pitch_off;
		var int32_t pitch_var;
		var int32_t vol;
		var int32_t loop;
		var int32_t loop_start_offset;
		var int32_t loop_end_offset;
		var float reverb_level;
		var string pfx_name;

		ZKAPI static void register_(DaedalusScript& s);
	};

	struct ISoundSystem : public DaedalusInstance {
		var float volume;
		var int32_t bit_resolution;
		var int32_t sample_rate;
		var int32_t use_stereo;
		var int32_t num_sfx_channels;
		var string used_3d_provider_name;

		ZKAPI static void register_(DaedalusScript& s);
	};

	ZKAPI void register_all_script_classes(DaedalusScript& s);
} // namespace zenkit

#undef var
#undef string
#undef func
