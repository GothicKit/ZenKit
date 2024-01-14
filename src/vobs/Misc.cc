// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Misc.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

namespace zenkit {
	void VAnimate::parse(VAnimate& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VAnimate::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->start_on = r.read_bool(); // startOn
		this->s_is_running = this->start_on;

		if (r.is_save_game()) {
			// In save-games, animated VObs contain extra variables
			this->s_is_running = r.read_bool(); // isRunning
		}
	}

	void VAnimate::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_bool("startOn", this->start_on);

		if (w.is_save_game()) {
			w.write_bool("isRunning", this->s_is_running);
		}
	}

	void VItem::parse(VItem& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VItem::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->instance = r.read_string(); // itemInstance

		if (r.is_save_game()) {
			// In save-games, items contain extra variables
			this->s_amount = r.read_int(); // amount
			this->s_flags = r.read_int();  // flags
		}
	}

	void VItem::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("itemInstance", this->instance);

		if (w.is_save_game()) {
			w.write_int("amount", this->s_amount);
			w.write_int("flags", this->s_flags);
		}
	}

	uint16_t VItem::get_version_identifier(GameVersion) const {
		return 0;
	}

	void VLensFlare::parse(VLensFlare& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VLensFlare::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->fx = r.read_string(); // lensflareFX
	}

	void VLensFlare::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("lensflareFX", this->fx);
	}

	uint16_t VLensFlare::get_version_identifier(GameVersion game) const {
		return game == GameVersion::GOTHIC_1 ? 64704 : 193;
	}

	void VParticleEffectController::parse(VParticleEffectController& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VParticleEffectController::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->pfx_name = r.read_string();        // pfxName
		this->kill_when_done = r.read_bool();    // killVobWhenDone
		this->initially_running = r.read_bool(); // pfxStartOn
	}

	void VParticleEffectController::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("pfxName", this->pfx_name);
		w.write_bool("killVobWhenDone", this->kill_when_done);
		w.write_bool("pfxStartOn", this->initially_running);
	}

	void VMessageFilter::parse(VMessageFilter& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VMessageFilter::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();                                       // triggerTarget
		this->on_trigger = static_cast<MessageFilterAction>(r.read_enum());   // onTrigger
		this->on_untrigger = static_cast<MessageFilterAction>(r.read_enum()); // onUntrigger
	}

	void VMessageFilter::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("triggerTarget", this->target);
		w.write_enum("onTrigger", static_cast<std::uint32_t>(this->on_trigger));
		w.write_enum("onUntrigger", static_cast<std::uint32_t>(this->on_untrigger));
	}

	uint16_t VMessageFilter::get_version_identifier(GameVersion) const {
		return 0;
	}

	void VCodeMaster::parse(VCodeMaster& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VCodeMaster::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();               // triggerTarget
		this->ordered = r.read_bool();                // orderRelevant
		this->first_false_is_failure = r.read_bool(); // firstFalseIsFailure
		this->failure_target = r.read_string();       // triggerTargetFailure
		this->untriggered_cancels = r.read_bool();    // untriggerCancels

		auto slave_count = r.read_byte(); // numSlaves
		for (int32_t i = 0; i < slave_count; ++i) {
			this->slaves.emplace_back(r.read_string()); // slaveVobName[i]
		}

		if (r.is_save_game() && version == GameVersion::GOTHIC_2) {
			// In Gothic II save-games, code masters contain extra variables
			this->s_num_triggered_slaves = r.read_byte(); // numSlavesTriggered

			for (auto i = 0; i < slave_count; ++i) {
				// FIXME: Figure out how to parse these correctly.
				r.skip_object(false); // [slaveTriggered1 % 0 0]
			}
		}
	}

	void VCodeMaster::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("triggerTarget", this->target);
		w.write_bool("orderRelevant", this->ordered);
		w.write_bool("firstFalseIsFailure", this->first_false_is_failure);
		w.write_string("triggerTargetFailure", this->failure_target);
		w.write_bool("untriggerCancels", this->untriggered_cancels);

		w.write_byte("numSlaves", this->slaves.size());
		for (auto i = 0u; i < (this->slaves.size() & 0xffu); ++i) {
			w.write_string("slaveVobName" + std::to_string(i), this->slaves[i]);
		}

		if (w.is_save_game() && version == GameVersion::GOTHIC_2) {
			// FIXME: This is not implemented because its behavior is unknown.
		}
	}

	uint16_t VCodeMaster::get_version_identifier(GameVersion) const {
		return 0;
	}

	void VMoverController::parse(VMoverController& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VMoverController::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();                               // triggerTarget
		this->message = static_cast<MoverMessageType>(r.read_enum()); // moverMessage
		this->key = r.read_int();                                     // gotoFixedKey
	}

	void VMoverController::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("triggerTarget", this->target);
		w.write_enum("moverMessage", static_cast<std::uint32_t>(this->message));
		w.write_int("gotoFixedKey", this->key);
	}

	uint16_t VMoverController::get_version_identifier(GameVersion) const {
		return 0;
	}

	void VTouchDamage::parse(VTouchDamage& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VTouchDamage::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->damage = r.read_float();                                    // damage
		this->barrier = r.read_bool();                                    // Barrier
		this->blunt = r.read_bool();                                      // Blunt
		this->edge = r.read_bool();                                       // Edge
		this->fire = r.read_bool();                                       // Fire
		this->fly = r.read_bool();                                        // Fly
		this->magic = r.read_bool();                                      // Magic
		this->point = r.read_bool();                                      // Point
		this->fall = r.read_bool();                                       // Fall
		this->repeat_delay_sec = r.read_float();                          // damageRepeatDelaySec
		this->volume_scale = r.read_float();                              // damageVolDownScale
		this->collision = static_cast<TouchCollisionType>(r.read_enum()); // damageCollType
	}

	void VTouchDamage::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_float("damage", this->damage);
		w.write_bool("Barrier", this->barrier);
		w.write_bool("Blunt", this->blunt);
		w.write_bool("Edge", this->edge);
		w.write_bool("Fire", this->fire);
		w.write_bool("Fly", this->fly);
		w.write_bool("Magic", this->magic);
		w.write_bool("Point", this->point);
		w.write_bool("Fall", this->fall);
		w.write_float("damageRepeatDelaySec", this->repeat_delay_sec);
		w.write_float("damageVolDownScale", this->volume_scale);
		w.write_enum("damageCollType", static_cast<std::uint32_t>(this->collision));
	}

	uint16_t VTouchDamage::get_version_identifier(GameVersion) const {
		return 36865;
	}

	void VEarthquake::parse(VEarthquake& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VEarthquake::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->radius = r.read_float();   // radius
		this->duration = r.read_float(); // timeSec
		this->amplitude = r.read_vec3(); // amplitudeCM
	}

	void VEarthquake::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_float("radius", this->radius);
		w.write_float("timeSec", this->duration);
		w.write_vec3("amplitudeCM", this->amplitude);
	}

	uint16_t VEarthquake::get_version_identifier(GameVersion) const {
		return 52224;
	}

	void VNpc::Talent::load(ReadArchive& r, GameVersion) {
		this->talent = r.read_int(); // talent
		this->value = r.read_int();  // value
		this->skill = r.read_int();  // skill
	}

	void VNpc::Talent::save(WriteArchive& w, GameVersion) const {
		w.write_int("talent", this->talent);
		w.write_int("value", this->value);
		w.write_int("skill", this->skill);
	}

	void VNpc::parse(VNpc& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VNpc::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);

		this->npc_instance = r.read_string(); // npcInstance
		this->model_scale = r.read_vec3();    // modelScale
		this->model_fatness = r.read_float(); // modelFatness

		auto overlay_count = r.read_int(); // numOverlays
		for (auto i = 0; i < overlay_count; ++i) {
			this->overlays.push_back(r.read_string()); // overlay
		}

		this->flags = r.read_int();         // flags
		this->guild = r.read_int();         // guild
		this->guild_true = r.read_int();    // guildTrue
		this->level = r.read_int();         // level
		this->xp = r.read_int();            // xp
		this->xp_next_level = r.read_int(); // xpnl
		this->lp = r.read_int();            // lp

		auto talent_count = static_cast<std::size_t>(r.read_int()); // numTalents
		this->talents.resize(talent_count);

		for (auto i = 0u; i < talent_count; ++i) {
			this->talents[i] = r.read_object<Talent>(version);
		}

		this->fight_tactic = r.read_int(); // fightTactic
		this->fight_mode = r.read_int();   // fightMode
		this->wounded = r.read_bool();     // wounded
		this->mad = r.read_bool();         // mad
		this->mad_time = r.read_int();     // madTime
		this->player = r.read_bool();      // player

		for (int& attribute : this->attributes) {
			attribute = r.read_int(); // atr0
		}

		if (version == GameVersion::GOTHIC_2) {
			// TODO: what are these (hc<n>)?
			for (int& hc : this->hit_chance) {
				hc = r.read_int(); // hc1
			}
		}

		for (int& mission : this->missions) {
			mission = r.read_int(); // mission0
		}

		this->start_ai_state = r.read_string(); // startAIState

		std::size_t var_count = version == GameVersion::GOTHIC_1 ? 50 : 100;
		auto vars = r.read_raw(var_count * 4); // scriptVars
		for (auto i = 0u; i < var_count; ++i) {
			this->aivar[i] = vars->read_int();
		}

		this->script_waypoint = r.read_string(); // scriptWp
		this->attitude = r.read_int();           // attitude
		this->attitude_temp = r.read_int();      // tmpAttitude
		this->name_nr = r.read_int();            // nameNr

		// unknown.
		[[maybe_unused]] auto spells = r.read_raw(4); // spells

		// TODO: News (I don't have a sample.).
		auto news_count = r.read_int(); // NumOfEntries
		if (news_count != 0) {
			ZKLOGE("VOb.Npc",
			       "!!! IMPORTANT !!! This save-game contains news entries and cannot be loaded currently. Please "
			       "open an issue at https://github.com/GothicKit/ZenKit providing your save-game as a ZIP file.");
			throw ParserError {"VNpc"};
		}

		carry_vob = std::dynamic_pointer_cast<VirtualObject>(r.read_object(version)); // [carryVob % 0 0]
		enemy = std::dynamic_pointer_cast<VirtualObject>(r.read_object(version));     // [enemy % 0 0]

		this->move_lock = r.read_bool(); // moveLock

		if (version == GameVersion::GOTHIC_1) {
			for (auto& i : this->packed) {
				i = r.read_string(); // packed
			}
		} else {
			auto pack = r.read_string(); // packed
			auto it = pack.begin();
			auto idx = 0;

			do {
				auto next = std::find(it, pack.end(), ';');
				this->packed[idx++].assign(it, next);
				it = next;
			} while (it != pack.end() && idx < 9);
		}

		auto item_count = static_cast<size_t>(r.read_int()); // itemCount
		this->items.resize(item_count);

		for (auto i = 0u; i < item_count; ++i) {
			this->items[i] = r.read_object<VItem>(version); // item0

			if ((this->items[i]->s_flags & 0x200) != 0) {
				(void) r.read_int(); // shortKey<n> // TODO
			}
		}

		auto inv_slot_count = static_cast<uint32_t>(r.read_int()); // numInvSlots
		this->slots.resize(inv_slot_count);
		for (auto i = 0u; i < inv_slot_count; ++i) {
			this->slots[i].used = r.read_bool();   // used
			this->slots[i].name = r.read_string(); // name

			if (this->slots[i].used) {
				this->slots[i].item = r.read_object<VItem>(version);
				this->slots[i].in_inventory = r.read_bool(); // inInv
			}
		}

		this->current_state_valid = r.read_bool();      // curState.valid
		this->current_state_name = r.read_string();     // curState.name
		this->current_state_index = r.read_int();       // curState.prgIndex
		this->current_state_is_routine = r.read_bool(); // curState.isRtnState
		this->next_state_valid = r.read_bool();         // nextState.valid
		this->next_state_name = r.read_string();        // nextState.name
		this->next_state_index = r.read_int();          // nextState.prgIndex
		this->next_state_is_routine = r.read_bool();    // nextState.isRtnState
		this->last_ai_state = r.read_int();             // lastAIState
		this->has_routine = r.read_bool();              // hasRoutine
		this->routine_changed = r.read_bool();          // rtnChanged
		this->routine_overlay = r.read_bool();          // rtnOverlay
		this->routine_overlay_count = r.read_int();     // rtnOverlayCount
		this->walkmode_routine = r.read_int();          // walkmode_routine
		this->weaponmode_routine = r.read_bool();       // weaponmode_routine
		this->start_new_routine = r.read_bool();        // startNewRoutine
		this->ai_state_driven = r.read_int();           // aiStateDriven
		this->ai_state_pos = r.read_vec3();             // aiStatePos
		this->current_routine = r.read_string();        // curRoutine
		this->respawn = r.read_bool();                  // respawn
		this->respawn_time = r.read_int();              // respawnTime

		auto prot = r.read_raw(sizeof(int32_t) * 8); // protection
		for (int& i : this->protection) {
			i = prot->read_int();
		}

		if (version == GameVersion::GOTHIC_2) {
			this->bs_interruptable_override = r.read_int(); // bsInterruptableOverride
			this->npc_type = r.read_int();                  // npcType
			this->spell_mana = r.read_int();                // spellMana
		}
	}

	void VNpc::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);

		w.write_string("npcInstance", this->npc_instance);
		w.write_vec3("modelScale", this->model_scale);
		w.write_float("modelFatness", this->model_fatness);

		w.write_int("numOverlays", this->overlays.size());
		for (auto& overlay : this->overlays) {
			w.write_string("overlay", overlay);
		}

		w.write_int("flags", this->flags);
		w.write_int("guild", this->guild);
		w.write_int("guildTrue", this->guild_true);
		w.write_int("level", this->level);
		w.write_int("xp", this->xp);
		w.write_int("xpnl", this->xp_next_level);
		w.write_int("lp", this->lp);

		w.write_int("numTalents", this->talents.size());
		for (auto& talent : this->talents) {
			w.write_object(talent, version);
		}

		w.write_int("fightTactic", this->fight_tactic);
		w.write_int("fightMode", this->fight_mode);
		w.write_bool("wounded", this->wounded);
		w.write_bool("mad", this->mad);
		w.write_int("madTime", this->mad_time);
		w.write_bool("player", this->player);

		for (auto i = 0u; i < attribute_count; ++i) {
			w.write_int("atr" + std::to_string(i), this->attributes[i]);
		}

		if (version == GameVersion::GOTHIC_2) {
			for (auto i = 0u; i < hcs_count; ++i) {
				w.write_int("hc" + std::to_string(i), this->hit_chance[i]);
			}
		}

		for (auto i = 0u; i < missions_count; ++i) {
			w.write_int("mission" + std::to_string(i), this->missions[i]);
		}

		w.write_string("startAIState", this->start_ai_state);

		{
			std::vector<std::byte> script_vars;
			auto script_vars_writer = Write::to(&script_vars);

			std::size_t var_count = version == GameVersion::GOTHIC_1 ? 50 : 100;
			for (auto i = 0u; i < var_count; ++i) {
				script_vars_writer->write_int(this->aivar[i]);
			}
			w.write_raw("scriptVars", script_vars);
		}

		w.write_string("scriptWp", this->script_waypoint);
		w.write_int("attitude", this->attitude);
		w.write_int("tmpAttitude", this->attitude_temp);
		w.write_int("nameNr", this->name_nr);

		// TODO: unknown.
		std::vector<std::byte> spells;
		spells.push_back({});
		spells.push_back({});
		spells.push_back({});
		spells.push_back({});
		w.write_raw("spells", spells);

		// TODO: News (I don't have a sample.).
		w.write_int("NumOfEntries", 0);

		w.write_object("carryVob", this->carry_vob, version);
		w.write_object("enemy", this->enemy, version);
		w.write_bool("moveLock", this->move_lock);

		if (version == GameVersion::GOTHIC_1) {
			for (auto const& i : this->packed) {
				w.write_string("packed", i);
			}
		} else {
			std::string pack;

			for (auto const& i : this->packed) {
				pack += i;
				pack += ";";
			}

			pack.resize(pack.size() - 1);
			w.write_string("packed", pack);
		}

		w.write_int("itemCount", this->items.size());
		for (auto i = 0u; i < this->items.size(); ++i) {
			auto& item = this->items[i];
			w.write_object("item" + std::to_string(i), item, version);

			if ((item->s_flags & 0x200) != 0) {
				w.write_int("shortKey" + std::to_string(i), 0); // TODO
			}
		}

		w.write_int("numInvSlots", this->slots.size());
		for (auto& slot : this->slots) {
			w.write_bool("used", slot.used);
			w.write_string("name", slot.name);

			if (slot.used) {
				w.write_object("vob", slot.item, version);
				w.write_bool("inInv", slot.in_inventory);
			}
		}

		w.write_bool("curState.valid", this->current_state_valid);
		w.write_string("curState.name", this->current_state_name);
		w.write_int("curState.prgIndex", this->current_state_index);
		w.write_bool("curState.isRtnState", this->current_state_is_routine);
		w.write_bool("nextState.valid", this->next_state_valid);
		w.write_string("nextState.name", this->next_state_name);
		w.write_int("nextState.prgIndex", this->next_state_index);
		w.write_bool("nextState.isRtnState", this->next_state_is_routine);
		w.write_int("lastAIState", this->last_ai_state);
		w.write_bool("hasRoutine", this->has_routine);
		w.write_bool("rtnChanged", this->routine_changed);
		w.write_bool("rtnOverlay", this->routine_overlay);
		w.write_int("rtnOverlayCount", this->routine_overlay_count);
		w.write_int("walkmode_routine", this->walkmode_routine);
		w.write_bool("weaponmode_routine", this->weaponmode_routine);
		w.write_bool("startNewRoutine", this->start_new_routine);
		w.write_int("aiStateDriven", this->ai_state_driven);
		w.write_vec3("aiStatePos", this->ai_state_pos);
		w.write_string("curRoutine", this->current_routine);
		w.write_bool("respawn", this->respawn);
		w.write_int("respawnTime", this->respawn_time);

		{
			std::vector<std::byte> prot;
			auto prot_writer = Write::to(&prot);

			for (auto p : this->protection) {
				prot_writer->write_int(p);
			}

			w.write_raw("protection", prot);
		}

		if (version == GameVersion::GOTHIC_2) {
			w.write_int("bsInterruptableOverride", this->bs_interruptable_override);
			w.write_int("npcType", this->npc_type);
			w.write_int("spellMana", this->spell_mana);
		}
	}

	uint16_t VNpc::get_version_identifier(GameVersion) const {
		return 52224;
	}

	void VScreenEffect::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);

		if (r.is_save_game()) {
			std::size_t count = version == GameVersion::GOTHIC_1 ? 5 : 12;
			(void) r.read_raw(count * 4); // blend
			(void) r.read_raw(count * 4); // cinema
			(void) r.read_raw(count * 4); // fovMorph
			(void) r.read_vec2();         // fovSaved
			(void) r.read_vec2();         // fovSaved1st
		}
	}

	void VScreenEffect::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);

		if (w.is_save_game()) {
			// FIXME: Not implemented because original not understood.
			std::size_t count = version == GameVersion::GOTHIC_1 ? 5 : 12;
			std::vector<std::byte> bytes {count * 4, std::byte {}};
			w.write_raw("blend", bytes);
			w.write_raw("cinema", bytes);   //
			w.write_raw("fovMorph", bytes); //
			w.write_vec2("fovSaved", {});
			w.write_vec2("fovSaved1st", {});
		}
	}
} // namespace zenkit
