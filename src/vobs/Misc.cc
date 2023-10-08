// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Misc.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

namespace zenkit::vobs {
	void Animate::parse(Animate& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Animate::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->start_on = r.read_bool(); // startOn

		if (r.is_save_game()) {
			// In save-games, animated VObs contain extra variables
			this->s_is_running = r.read_bool(); // isRunning
		}
	}

	void Item::parse(Item& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Item::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->instance = r.read_string(); // itemInstance

		if (r.is_save_game()) {
			// In save-games, items contain extra variables
			this->s_amount = r.read_int(); // amount
			this->s_flags = r.read_int();  // flags
		}
	}

	void LensFlare::parse(LensFlare& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void LensFlare::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->fx = r.read_string(); // lensflareFX
	}

	void ParticleEffectController::parse(ParticleEffectController& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void ParticleEffectController::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->pfx_name = r.read_string();        // pfxName
		this->kill_when_done = r.read_bool();    // killVobWhenDone
		this->initially_running = r.read_bool(); // pfxStartOn
	}

	void MessageFilter::parse(MessageFilter& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void MessageFilter::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string();                                       // triggerTarget
		this->on_trigger = static_cast<MessageFilterAction>(r.read_enum());   // onTrigger
		this->on_untrigger = static_cast<MessageFilterAction>(r.read_enum()); // onUntrigger
	}

	void CodeMaster::parse(CodeMaster& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void CodeMaster::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
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

		if (this->saved && version == GameVersion::GOTHIC_2) {
			// In Gothic II save-games, code masters contain extra variables
			this->s_num_triggered_slaves = r.read_byte(); // numSlavesTriggered

			for (auto i = 0; i < slave_count; ++i) {
				// TODO: Figure out how to parse these correctly.
				r.skip_object(false); // [slaveTriggered1 % 0 0]
			}
		}
	}

	void MoverController::parse(MoverController& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void MoverController::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->target = r.read_string(); // triggerTarget

		if (version == GameVersion::GOTHIC_1) {
			this->message = static_cast<MoverMessageType>(r.read_enum()); // moverMessage
		} else {
			this->message = static_cast<MoverMessageType>(r.read_byte()); // moverMessage
		}

		this->key = r.read_int(); // gotoFixedKey
	}

	void TouchDamage::parse(TouchDamage& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void TouchDamage::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
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

	void Earthquake::parse(Earthquake& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Earthquake::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		this->radius = r.read_float();   // radius
		this->duration = r.read_float(); // timeSec
		this->amplitude = r.read_vec3(); // amplitudeCM
	}

	void Npc::parse(vobs::Npc& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Npc::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
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

		ArchiveObject hdr;
		for (auto i = 0u; i < talent_count; ++i) {
			if (!r.read_object_begin(hdr)) // [% oCNpcTalent 0 0]
				throw zenkit::ParserError {"vobs::Npc"};

			// empty object
			if (hdr.class_name == "%") {
				r.skip_object(true);
				continue;
			}

			this->talents[i].talent = r.read_int(); // talent
			this->talents[i].value = r.read_int();  // value
			this->talents[i].skill = r.read_int();  // skill

			if (!r.read_object_end()) {
				ZKLOGW("VOb.Npc", "oCNpcTalent object not fully parsed");
				r.skip_object(true);
			}
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
			for (int& hc : this->hcs) {
				hc = r.read_int(); // hc1
			}
		}

		for (int& mission : this->missions) {
			mission = r.read_int(); // mission0
		}

		this->start_ai_state = r.read_string(); // startAIState

		std::size_t var_count = version == GameVersion::GOTHIC_1 ? 50 : 100;
		auto vars = r.read_raw(var_count * 4); // scriptVars
		for (auto i = 0u; i < var_count / 4; ++i) {
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
			       "open an issue at https://github.com/GothicKit/phoenix providing your save-game as a ZIP file.");
			throw zenkit::ParserError {"vobs::Npc"};
		}

		r.skip_object(false); // [carryVob % 0 0]
		r.skip_object(false); // [enemy % 0 0]

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
			if (!r.read_object_begin(hdr)) throw zenkit::ParserError {"vobs::Npc"};

			this->items[i] = std::make_unique<Item>();
			this->items[i]->load(r, version);
			this->items[i]->id = hdr.index;

			if (!r.read_object_end()) {
				ZKLOGW("VOb.Npc", "oCItem:zCVob object not fully parsed");
				r.skip_object(true);
			}

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
				// [vob § 0 0]
				if (!r.read_object_begin(hdr)) {
					throw zenkit::ParserError {"VOb.Npc"};
				}

				if (hdr.class_name == "\xA7") {
					// This item is a reference.
					// TODO: Warn if not found.
					for (auto const& item : this->items) {
						if (item->id == hdr.index) {
							this->slots[i].item = item.get();
							break;
						}
					}
				} else {
					// This item is embedded.
					this->items.push_back(std::make_unique<Item>());

					auto& item = this->items.back();
					item->load(r, version);
					item->id = hdr.index;

					this->slots[i].item = item.get();
				}

				if (!r.read_object_end()) {
					ZKLOGW("VOb.Npc", "Inventory slot not fully parsed.");
					r.skip_object(true);
				}

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

	void ScreenEffect::load(ReadArchive& r, GameVersion version) {
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
} // namespace zenkit::vobs
