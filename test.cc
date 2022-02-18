// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <iostream>
#include <phoenix/daedalus/interpreter.hh>
#include <phoenix/daedalus/script.hh>
#include <phoenix/vdfs.hh>
#include <phoenix/world.hh>

#include <chrono>

using namespace std::literals;

#define func int

#define G2 0

struct C_NPC : public phoenix::instance {
	int ID {};
	std::string NAME[5] {};
	std::string SLOT {};
	int NPCTYPE {};
	int FLAGS {};
	int ATTRIBUTE[8] {};
	int PROTECTION[8] {};
	int DAMAGE[8] {};
	int DAMAGETYPE {};
	int GUILD {};
	int LEVEL {};
	int MISSION[5] {};
	int FIGHT_TACTIC {};
	int WEAPON {};
	int VOICE {};
	int VOICEPITCH {};
	int BODYMASS {};
	func DAILY_ROUTINE {};
	func START_AISTATE {};
	std::string SPAWNPOINT {};
	int SPAWNDELAY {};
	int SENSES {};
	int SENSES_RANGE {};

#if G2
	int AIVAR[100] {};
#else
	int AIVAR[50] {};
#endif

	std::string WP {};
	int EXP {};
	int EXP_NEXT {};
	int LP {};


#if G2
	int BODYSTATEINTERRUPTABLEOVERRIDE {};
	int NOFOCUS {};
	std::string EFFECT;
	int HITCHANCE[5];
#endif
};

struct C_ITEM : public phoenix::instance {
	int ID;
	std::string NAME;
	std::string NAMEID;
	int HP;
	int HP_MAX;
	int MAINFLAG;
	int FLAGS;
	int WEIGHT;
	int VALUE;
	int DAMAGETYPE;
	int DAMAGETOTAL;
	int DAMAGE[8];
	int WEAR;
	int PROTECTION[8];
	int NUTRITION;
	int COND_ATR[3];
	int COND_VALUE[3];
	int CHANGE_ATR[3];
	int CHANGE_VALUE[3];
	func MAGIC;
	func ON_EQUIP;
	func ON_UNEQUIP;
	func ON_STATE[4];
	func OWNER;
	int OWNERGUILD;
	int DISGUISEGUILD;
	std::string VISUAL;
	std::string VISUAL_CHANGE;
	int VISUAL_SKIN;
	std::string SCEMENAME;
	int MATERIAL;
	int MUNITION;
	int SPELL;
	int RANGE;
	int MAG_CIRCLE;
	std::string DESCRIPTION;
	std::string TEXT[6];
	int COUNT[6];

#if G2
	std::string EFFECT;
	int INV_ZBIAS;
	int INV_ROTX;
	int INV_ROTY;
	int INV_ROTZ;
	int INV_ANIMATE;
#endif
};


struct C_INFO : public phoenix::instance {
	int NPC;
	int NR;
	int IMPORTANT;
	func CONDITION;
	func INFORMATION;
	std::string DESCRIPTION;
	int TRADE;
	int PERMANENT;
};

int main() {
#if G2
	auto scr = phoenix::script::parse("/mnt/Legacy/gothic/g2notr_en_gog/_work/Data/Scripts/_compiled/GOTHIC.DAT");
	auto world = phoenix::world::parse();
#else
	auto scr = phoenix::script::parse("/mnt/Legacy/gothic/g1_de_gog/_work/DATA/scripts/_compiled/GOTHIC.DAT");

	auto worlds = phoenix::vdf_file::open("/mnt/Legacy/gothic/g1_de_gog/Data/worlds.VDF");
	auto world_reader = worlds.find_entry("WORLD.ZEN")->open();
	auto world = phoenix::world::parse(world_reader, game_version::gothic_1);
#endif
	scr.register_member("C_NPC.ID", &C_NPC::ID);
	scr.register_member("C_NPC.NAME", &C_NPC::NAME);
	scr.register_member("C_NPC.SLOT", &C_NPC::SLOT);
	scr.register_member("C_NPC.NPCTYPE", &C_NPC::NPCTYPE);
	scr.register_member("C_NPC.FLAGS", &C_NPC::FLAGS);
	scr.register_member("C_NPC.ATTRIBUTE", &C_NPC::ATTRIBUTE);
	scr.register_member("C_NPC.PROTECTION", &C_NPC::PROTECTION);
	scr.register_member("C_NPC.DAMAGE", &C_NPC::DAMAGE);
	scr.register_member("C_NPC.DAMAGETYPE", &C_NPC::DAMAGETYPE);
	scr.register_member("C_NPC.GUILD", &C_NPC::GUILD);
	scr.register_member("C_NPC.LEVEL", &C_NPC::LEVEL);
	scr.register_member("C_NPC.MISSION", &C_NPC::MISSION);
	scr.register_member("C_NPC.FIGHT_TACTIC", &C_NPC::FIGHT_TACTIC);
	scr.register_member("C_NPC.WEAPON", &C_NPC::WEAPON);
	scr.register_member("C_NPC.VOICE", &C_NPC::VOICE);
	scr.register_member("C_NPC.VOICEPITCH", &C_NPC::VOICEPITCH);
	scr.register_member("C_NPC.BODYMASS", &C_NPC::BODYMASS);
	scr.register_member("C_NPC.DAILY_ROUTINE", &C_NPC::DAILY_ROUTINE);
	scr.register_member("C_NPC.START_AISTATE", &C_NPC::START_AISTATE);
	scr.register_member("C_NPC.SPAWNPOINT", &C_NPC::SPAWNPOINT);
	scr.register_member("C_NPC.SPAWNDELAY", &C_NPC::SPAWNDELAY);
	scr.register_member("C_NPC.SENSES", &C_NPC::SENSES);
	scr.register_member("C_NPC.SENSES_RANGE", &C_NPC::SENSES_RANGE);
	scr.register_member("C_NPC.AIVAR", &C_NPC::AIVAR);
	scr.register_member("C_NPC.WP", &C_NPC::WP);
	scr.register_member("C_NPC.EXP", &C_NPC::EXP);
	scr.register_member("C_NPC.EXP_NEXT", &C_NPC::EXP_NEXT);
	scr.register_member("C_NPC.LP", &C_NPC::LP);

#if G2
	scr.register_member("C_NPC.BODYSTATEINTERRUPTABLEOVERRIDE", &C_NPC::BODYSTATEINTERRUPTABLEOVERRIDE);
	scr.register_member("C_NPC.NOFOCUS", &C_NPC::NOFOCUS);
	scr.register_member("C_NPC.EFFECT", &C_NPC::EFFECT);
	scr.register_member("C_NPC.HITCHANCE", &C_NPC::HITCHANCE);
#endif

	scr.register_member("C_ITEM.ID", &C_ITEM::ID);
	scr.register_member("C_ITEM.NAME", &C_ITEM::NAME);
	scr.register_member("C_ITEM.NAMEID", &C_ITEM::NAMEID);
	scr.register_member("C_ITEM.HP", &C_ITEM::HP);
	scr.register_member("C_ITEM.HP_MAX", &C_ITEM::HP_MAX);
	scr.register_member("C_ITEM.MAINFLAG", &C_ITEM::MAINFLAG);
	scr.register_member("C_ITEM.FLAGS", &C_ITEM::FLAGS);
	scr.register_member("C_ITEM.WEIGHT", &C_ITEM::WEIGHT);
	scr.register_member("C_ITEM.VALUE", &C_ITEM::VALUE);
	scr.register_member("C_ITEM.DAMAGETYPE", &C_ITEM::DAMAGETYPE);
	scr.register_member("C_ITEM.DAMAGETOTAL", &C_ITEM::DAMAGETOTAL);
	scr.register_member("C_ITEM.DAMAGE", &C_ITEM::DAMAGE);
	scr.register_member("C_ITEM.WEAR", &C_ITEM::WEAR);
	scr.register_member("C_ITEM.PROTECTION", &C_ITEM::PROTECTION);
	scr.register_member("C_ITEM.NUTRITION", &C_ITEM::NUTRITION);
	scr.register_member("C_ITEM.COND_ATR", &C_ITEM::COND_ATR);
	scr.register_member("C_ITEM.COND_VALUE", &C_ITEM::COND_VALUE);
	scr.register_member("C_ITEM.CHANGE_ATR", &C_ITEM::CHANGE_ATR);
	scr.register_member("C_ITEM.CHANGE_VALUE", &C_ITEM::CHANGE_VALUE);
	scr.register_member("C_ITEM.MAGIC", &C_ITEM::MAGIC);
	scr.register_member("C_ITEM.ON_EQUIP", &C_ITEM::ON_EQUIP);
	scr.register_member("C_ITEM.ON_UNEQUIP", &C_ITEM::ON_UNEQUIP);
	scr.register_member("C_ITEM.ON_STATE", &C_ITEM::ON_STATE);
	scr.register_member("C_ITEM.OWNER", &C_ITEM::OWNER);
	scr.register_member("C_ITEM.OWNERGUILD", &C_ITEM::OWNERGUILD);
	scr.register_member("C_ITEM.DISGUISEGUILD", &C_ITEM::DISGUISEGUILD);
	scr.register_member("C_ITEM.VISUAL", &C_ITEM::VISUAL);
	scr.register_member("C_ITEM.VISUAL_CHANGE", &C_ITEM::VISUAL_CHANGE);
	scr.register_member("C_ITEM.VISUAL_SKIN", &C_ITEM::VISUAL_SKIN);
	scr.register_member("C_ITEM.SCEMENAME", &C_ITEM::SCEMENAME);
	scr.register_member("C_ITEM.MATERIAL", &C_ITEM::MATERIAL);
	scr.register_member("C_ITEM.MUNITION", &C_ITEM::MUNITION);
	scr.register_member("C_ITEM.SPELL", &C_ITEM::SPELL);
	scr.register_member("C_ITEM.RANGE", &C_ITEM::RANGE);
	scr.register_member("C_ITEM.MAG_CIRCLE", &C_ITEM::MAG_CIRCLE);
	scr.register_member("C_ITEM.DESCRIPTION", &C_ITEM::DESCRIPTION);
	scr.register_member("C_ITEM.TEXT", &C_ITEM::TEXT);
	scr.register_member("C_ITEM.COUNT", &C_ITEM::COUNT);

#if G2
	scr.register_member("C_ITEM.EFFECT", &C_ITEM::EFFECT);
	scr.register_member("C_ITEM.INV_ZBIAS", &C_ITEM::INV_ZBIAS);
	scr.register_member("C_ITEM.INV_ROTX", &C_ITEM::INV_ROTX);
	scr.register_member("C_ITEM.INV_ROTY", &C_ITEM::INV_ROTY);
	scr.register_member("C_ITEM.INV_ROTZ", &C_ITEM::INV_ROTZ);
	scr.register_member("C_ITEM.INV_ANIMATE", &C_ITEM::INV_ANIMATE);
#endif

	scr.register_member("C_INFO.NPC", &C_INFO::NPC);
	scr.register_member("C_INFO.NR", &C_INFO::NR);
	scr.register_member("C_INFO.IMPORTANT", &C_INFO::IMPORTANT);
	scr.register_member("C_INFO.CONDITION", &C_INFO::CONDITION);
	scr.register_member("C_INFO.INFORMATION", &C_INFO::INFORMATION);
	scr.register_member("C_INFO.DESCRIPTION", &C_INFO::DESCRIPTION);
	scr.register_member("C_INFO.TRADE", &C_INFO::TRADE);
	scr.register_member("C_INFO.PERMANENT", &C_INFO::PERMANENT);

	phoenix::daedalus_interpreter vm {scr};

	std::vector<std::shared_ptr<C_NPC>> npcs;
	std::vector<std::shared_ptr<C_ITEM>> items;

	vm.register_external("NPC_SETTALENTVALUE", [](std::shared_ptr<C_NPC> npc, s32 type, s32 value) {
		// std::cout << "NPC_SETTALENTVALUE(&" << npc->get_symbol()->name() << ", " << type << ", " << value << ")\n";
	});

	vm.register_external("MDL_SETVISUAL", [](std::shared_ptr<C_NPC> npc, std::string_view name) {
		// std::cout << "MDL_SETVISUAL(&" << npc->get_symbol()->name() << ", '" << name << "')\n";
	});

	vm.register_external("MDL_APPLYOVERLAYMDS", [](std::shared_ptr<C_NPC> npc, std::string_view name) {
		// std::cout << "MDL_APPLYOVERLAYMDS(&" << npc->get_symbol()->name() << ", " << name << "')\n";
	});

	vm.register_external("MDL_SETVISUALBODY", [](std::shared_ptr<C_NPC> npc, std::string_view body, s32 bodyTexNr,
												 s32 bodyTexColor, std::string_view head, s32 headTexNr, s32 teethTexNr,
												 s32 armor) {
		// std::cout << "MDL_SETVISUALBODY(&" << npc->get_symbol()->name() << ", " << body << "', " << bodyTexNr << ", " << bodyTexColor << ", '"
		//		  << head << "', " << headTexNr << ", " << teethTexNr << ", " << armor << ")\n";
	});

	vm.register_external("MDL_SETMODELFATNESS", [](std::shared_ptr<C_NPC> npc, f32 fat) {
		// std::cout << "MDL_SETMODELFATNESS(&" << npc->get_symbol()->name() << ", " << fat << ")\n";
	});

	vm.register_external("INTTOSTRING", [](s32 v) {
		// std::cout << "INTTOSTRING(" << v << ")\n";
		return std::to_string(v);
	});

	vm.register_external("CONCATSTRINGS", [](std::string_view lhs, std::string_view rhs) {
		// std::cout << "CONCATSTRINGS('" << lhs << "', '" << rhs << "')\n";
		return std::string {lhs} + std::string {rhs};
	});

	vm.register_external("PRINTDEBUGINSTCH", [](s32 type, std::string_view message) {
		// std::cout << "PRINTDEBUGINSTCH(" << type << ", '" << message << "')\n";
	});

	vm.register_external("MDL_SETMODELSCALE", [](std::shared_ptr<C_NPC> npc, float x, float y, float z) {
		// std::cout << "MDL_SETMODELSCALE(&" << npc->get_symbol()->name() << ", " << x << ", " << y << ", " << z << ")\n";
	});

	vm.register_external("NPC_SETTALENTSKILL", [](std::shared_ptr<C_NPC> npc, s32 talent, s32 value) {
		// std::cout << "NPC_SETTALENTSKILL(&" << npc->get_symbol()->name() << ", " << talent << ", " << value << ")\n";
	});

	vm.register_external("EQUIPITEM", [&vm](std::shared_ptr<C_NPC> npc, s32 item) {
		auto itemsym = vm.loaded_script().find_symbol_by_index(item);
		// std::cout << "EQUIPITEM(&" << npc->get_symbol()->name() << ", &" << itemsym->name() << ")\n";
	});

	vm.register_external("CREATEINVITEMS", [&vm, &items](std::shared_ptr<C_NPC> npc, s32 item, s32 amount) {
		auto* itemsym = vm.loaded_script().find_symbol_by_index(item);

		// std::cout << "CREATEINVITEMS(&" << npc->get_symbol()->name() << ", " << amount << ", &" << itemsym->name() << ")\n";
		if (itemsym->get_instance() == nullptr) {
			items.push_back(vm.init_instance<C_ITEM>(itemsym));
		}
	});

	vm.register_external("CREATEINVITEM", [&vm, &items](std::shared_ptr<C_NPC> npc, s32 item) {
		auto itemsym = vm.loaded_script().find_symbol_by_index(item);

		// std::cout << "CREATEINVITEM(&" << npc->get_symbol()->name() << ", &" << itemsym->name() << ")\n";
		if (itemsym->get_instance() == nullptr) {
			items.push_back(vm.init_instance<C_ITEM>(itemsym));
		}
	});

	vm.register_external("NPC_SETATTITUDE", [](std::shared_ptr<C_NPC> npc, s32 attitude) {
		// std::cout << "NPC_SETATTITUDE(&" << npc->get_symbol()->name() << ", " << attitude << ")\n";
	});

	vm.register_external("NPC_SETTEMPATTITUDE", [](std::shared_ptr<C_NPC> npc, s32 attitude) {
		// std::cout << "NPC_SETTEMPATTITUDE(&" << npc->get_symbol()->name() << ", " << attitude << ")\n";
	});

	vm.register_external("TA_MIN", [](std::shared_ptr<C_NPC> npc, s32 start_h, s32 start_m, s32 stop_h, s32 stop_m, s32 action, std::string_view waypoint) {
		// std::cout << "TA_MIN(&" << npc->get_symbol()->name() << ", " << start_h << ", " << start_m << ", " << stop_h << ", " << stop_m << ", " << action << ", '" << waypoint << "')\n";
	});

	vm.register_external("NPC_PERCENABLE", [](std::shared_ptr<C_NPC> npc, s32 type, s32 callback) {
		// std::cout << "NPC_PERCENABLE(&" << npc->get_symbol()->name() << ", " << type << ", " << callback << ")\n";
	});

	vm.register_external("NPC_SETPERCTIME", [](std::shared_ptr<C_NPC> npc, float seconds) {
		// std::cout << "NPC_SETPERCTIME(&" << npc->get_symbol()->name() << ", " << seconds << ")\n";
	});

	vm.register_external("AI_SETWALKMODE", [](std::shared_ptr<C_NPC> npc, s32 mode) {
		// std::cout << "AI_SETWALKMODE(&" << npc->get_symbol()->name() << ", " << mode << ")\n";
	});

	vm.register_external("NPC_GETBODYSTATE", [](std::shared_ptr<C_NPC> npc) {
		// std::cout << "NPC_GETBODYSTATE(&" << npc->get_symbol()->name() << ")\n";
		return 0;
	});

	vm.register_external("NPC_GETNEARESTWP", [](std::shared_ptr<C_NPC> npc) {
		// std::cout << "NPC_GETNEARESTWP(&" << npc->get_symbol()->name() << ")\n";
		return "NONE"s;
	});

	vm.register_external("HLP_STRCMP", [](std::string_view lhs, std::string_view rhs) {
		// std::cout << "HLP_STRCMP('" << lhs << "', '" << rhs << "')\n";
		return lhs == rhs;
	});

	vm.register_external("AI_GOTOWP", [](std::shared_ptr<C_NPC> npc, std::string_view name) {
		// std::cout << "AI_GOTOWP(&" << npc->get_symbol()->name() << ", '" << name << "')\n";
	});

	vm.register_external("AI_STARTSTATE", [](std::shared_ptr<C_NPC> npc, s32 state, s32 finalize, std::string_view waypoint) {
		// std::cout << "AI_STARTSTATE(&" << npc->get_symbol()->name() << ", " << state << ", " << finalize << ", '" << waypoint << "')\n";
	});

	vm.register_external("HLP_ISVALIDNPC", [](phoenix::symbol* npc) {
		// std::cout << "HLP_ISVALIDNPC(&" << npc->name() << ")\n";
		return npc->get_instance() != nullptr;
	});

	vm.register_external("HLP_ISVALIDITEM", [](phoenix::symbol* item) {
		// std::cout << "HLP_ISVALIDITEM(&" << item->name() << ")\n";
		return item->get_instance() != nullptr;
	});

	vm.register_external("NPC_CANSEENPCFREELOS", [](std::shared_ptr<C_NPC> self, std::shared_ptr<C_NPC> other) {
		if (self == nullptr || other == nullptr) {
			return false;
		}
		// std::cout << "NPC_CANSEENPCFREELOS(&" << self->get_symbol()->name() << ", &" << other->get_symbol()->name() << ")\n";
		return true;
	});

	vm.register_external("DOC_CREATE", []() {
		// std::cout << "DOC_CREATE()\n";
		return 1234567;
	});

	vm.register_external("DOC_SETPAGES", [](s32 id, s32 par1) {
		// std::cout << "DOC_SETPAGES(" << id << ", " << par1 << ")\n";
	});

	vm.register_external("DOC_SETPAGE", [](s32 id, s32 par1, std::string_view par2, s32 par3) {
		// std::cout << "DOC_SETPAGE(" << id << ", " << par1 << ", '" << par2 << "', " << par3 << ")\n";
	});

	vm.register_external("DOC_SETFONT", [](s32 id, s32 par1, std::string_view par2) {
		// std::cout << "DOC_SETFONT(" << id << ", " << par1 << ", '" << par2 << "')\n";
	});

	vm.register_external("DOC_SETMARGINS", [](s32 id, s32 par1, s32 par2, s32 par3, s32 par4, s32 par5, s32 par6) {
		// std::cout << "DOC_SETMARGINS(" << id << ", " << par1 << ", " << par2 << ", " << par3 << ", " << par4 << ", " << par5 << ", " << par6 << ")\n";
	});

	vm.register_external("DOC_PRINTLINE", [](s32 id, s32 y, std::string_view text) {
		// std::cout << "DOC_PRINTLINE(" << id << ", " << y << ", '" << text << "')\n";
	});

	vm.register_external("DOC_PRINTLINES", [](s32 id, s32 y, std::string_view text) {
		// std::cout << "DOC_PRINTLINES(" << id << ", " << y << ", '" << text << "')\n";
	});

	vm.register_external("DOC_SHOW", [](s32 id) {
		// std::cout << "DOC_SHOW(" << id << ")\n";
	});

	vm.register_external("HLP_GETNPC", [&scr](s32 id) {
		// std::cout << "HLP_GETNPC(" << id << ")\n";
		return scr.find_symbol_by_index(id)->get_instance();
	});

	vm.register_external("NPC_EXCHANGEROUTINE", [](std::shared_ptr<C_NPC> npc, std::string_view routine) {
		// std::cout << "NPC_EXCHANGEROUTINE(&" << npc->get_symbol()->name() << ", '" << routine << "')\n";
	});

	vm.register_external("NPC_REMOVEINVITEM", [](std::shared_ptr<C_NPC> npc, s32 item) {
		// std::cout << "NPC_REMOVEINVITEM(&" << npc->get_symbol()->name() << ", '" << item << "')\n";
		return true;
	});

	vm.register_external("WLD_REMOVEITEM", [](std::shared_ptr<C_ITEM> item) {
		// std::cout << "WLD_REMOVEITEM(&" << item->get_symbol()->name() << ")\n";
		return true;
	});

	vm.register_external("WLD_INSERTNPC", [&npcs, &vm, &world](s32 id, std::string_view at) {
		auto* npc = vm.loaded_script().find_symbol_by_index(id);
		if (!npc || npc->get_instance() != nullptr) return;

		// std::cout << "WLD_INSERTNPC(&" << npc->name() << ", '" << at << "')\n";
		npcs.push_back(vm.init_instance<C_NPC>(npc));


		auto wp = world.waynet().waypoint(std::string {at});

		if (wp == nullptr) {
			std::cout << "Insert NPC " << npc->name() << " at [?, ?, ?]\n";
		} else {
			std::cout << "Insert NPC " << npc->name() << " at [" << wp->position.x << ", " << wp->position.y << ", " << wp->position.z << "]\n";
		}
	});

	vm.register_external("WLD_SETMOBROUTINE", [](s32 p0, s32 p1, std::string_view p2, s32 p3) {
		// std::cout << "WLD_SETMOBROUTINE(" << p0 << ", " << p1 << ", '" << p2 << "', " << p3 << ")\n";
	});

	vm.register_external("NPC_SETTOFISTMODE", [](std::shared_ptr<C_NPC> npc) {
		// std::cout << "NPC_SETTOFISTMODE(&" << npc->get_symbol()->name() << ")\n";
	});

	vm.register_external("NPC_CHANGEATTRIBUTE", [](std::shared_ptr<C_NPC> npc, s32 p1, s32 p2) {
		// std::cout << "NPC_CHANGEATTRIBUTE(&" << npc->get_symbol()->name() << ", " << p1 << ", " << p2 << ")\n";
	});

	vm.register_external("WLD_INSERTITEM", [&items, &vm, &world](s32 id, std::string_view at) {
		auto* item = vm.loaded_script().find_symbol_by_index(id);
		if (!item || item->get_instance() != nullptr) return;

		// std::cout << "WLD_INSERTITEM(&" << item->name() << ", '" << at << "')\n";
		items.push_back(vm.init_instance<C_ITEM>(item));

		auto wp = world.waynet().waypoint(std::string {at});
		if (wp == nullptr) {
			std::cout << "Insert item " << item->name() << " at [?, ?, ?]\n";
		} else {
			std::cout << "Insert item " << item->name() << " at [" << wp->position.x << ", " << wp->position.y << ", " << wp->position.z << "]\n";
		}
	});

	vm.register_external("NPC_SETTOFIGHTMODE", [](std::shared_ptr<C_NPC> npc, s32 mode) {
		// std::cout << "NPC_SETTOFIGHTMODE(&" << npc->get_symbol()->name() << ", " << mode << ")\n";
	});

	vm.register_external("WLD_ASSIGNROOMTOGUILD", [](std::string_view room, s32 guild) {
		// std::cout << "WLD_ASSIGNROOMTOGUILD('" << room << "', " << guild << ")\n";
	});

	vm.register_external("WLD_SETOBJECTROUTINE", [](s32 p0, s32 p1, std::string_view p2, s32 p3) {
		// std::cout << "WLD_SETOBJECTROUTINE(" << p0 << ", " << p1 << ", '" << p2 << "', " << p3 << ")\n";
	});

	vm.register_external("WLD_SETGUILDATTITUDE", [](s32 from, s32 attitude, s32 to) {
		// std::cout << "WLD_SETGUILDATTITUDE(" << from << ", " << attitude << ", " << to << ")\n";
	});

	vm.register_external("WLD_EXCHANGEGUILDATTITUDES", [](std::string_view p0) {
		// std::cout << "WLD_EXCHANGEGUILDATTITUDES('" << p0 << "')\n";
	});

	vm.register_external("PLAYVIDEO", [](std::string_view file) {
		// std::cout << "PLAYVIDEO('" << file << "')\n";
		return 1;
	});

	vm.register_external("AI_OUTPUT", [](std::shared_ptr<C_NPC> npc, std::shared_ptr<C_INFO> info, std::string_view message) {
		if (npc == nullptr || info == nullptr) return;
		// std::cout << "AI_OUTPUT(&" << npc->get_symbol()->name() << ", &" << info->get_symbol()->name() << ", '" << message << "')\n";
	});

	// 0xe7482
	// 0xe7496

#if G2
	vm.register_external("HLP_RANDOM", [](s32 max) {
		// std::cout << "HLP_RANDOM(" << max << ")\n";
		return rand() % max;
	});

	vm.register_external("WLD_SETTIME", [](s32 h, s32 m) {
		// std::cout << "WLD_SETTIME(" << h << ", " << m << ")\n";
	});

	vm.register_external("NPC_ISDEAD", [](std::shared_ptr<C_NPC> npc) {
		// std::cout << "NPC_ISDEAD(&" << npc->get_symbol()->name() << ")\n";
		return false;
	});
#endif

	auto now = std::chrono::steady_clock::now();
	vm.call_function("STARTUP_WORLD");

	// std::cout << "Call to STARTUP_NEWWORLD took " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - now).count() << "µs\n";

	// std::cout << "There are " << items.size() << " items\n";
	// std::cout << "There are " << npcs.size() << " NPCs\n";
}
