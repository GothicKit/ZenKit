// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/misc.hh>

namespace phoenix::vobs {
	void animate::parse(animate& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.start_on = ctx.read_bool(); // startOn

		if (ctx.is_save_game()) {
			// In save-games, animated VObs contain extra variables
			obj.s_is_running = ctx.read_bool(); // isRunning
		}
	}

	void item::parse(item& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.instance = ctx.read_string(); // itemInstance

		if (ctx.is_save_game()) {
			// In save-games, items contain extra variables
			obj.s_amount = ctx.read_int(); // amount
			obj.s_flags = ctx.read_int();  // flags
		}
	}

	void lens_flare::parse(lens_flare& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.fx = ctx.read_string(); // lensflareFX
	}

	void pfx_controller::parse(pfx_controller& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.pfx_name = ctx.read_string();        // pfxName
		obj.kill_when_done = ctx.read_bool();    // killVobWhenDone
		obj.initially_running = ctx.read_bool(); // pfxStartOn
	}

	void message_filter::parse(message_filter& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.target = ctx.read_string();                                         // triggerTarget
		obj.on_trigger = static_cast<message_filter_action>(ctx.read_enum());   // onTrigger
		obj.on_untrigger = static_cast<message_filter_action>(ctx.read_enum()); // onUntrigger
	}

	void code_master::parse(code_master& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.target = ctx.read_string();               // triggerTarget
		obj.ordered = ctx.read_bool();                // orderRelevant
		obj.first_false_is_failure = ctx.read_bool(); // firstFalseIsFailure
		obj.failure_target = ctx.read_string();       // triggerTargetFailure
		obj.untriggered_cancels = ctx.read_bool();    // untriggerCancels

		auto slave_count = ctx.read_byte(); // numSlaves
		for (int32_t i = 0; i < slave_count; ++i) {
			obj.slaves.emplace_back(ctx.read_string()); // slaveVobName[i]
		}

		if (obj.saved && version == game_version::gothic_2) {
			// In Gothic II save-games, code masters contain extra variables
			obj.s_num_triggered_slaves = ctx.read_byte(); // numSlavesTriggered

			for (auto i = 0; i < slave_count; ++i) {
				// TODO: Figure out how to parse these correctly.
				ctx.skip_object(false); // [slaveTriggered1 % 0 0]
			}
		}
	}

	void mover_controller::parse(mover_controller& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.target = ctx.read_string(); // triggerTarget

		if (version == game_version::gothic_1) {
			obj.message = static_cast<mover_message_type>(ctx.read_enum()); // moverMessage
		} else {
			obj.message = static_cast<mover_message_type>(ctx.read_byte()); // moverMessage
		}

		obj.key = ctx.read_int(); // gotoFixedKey
	}

	void touch_damage::parse(touch_damage& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.damage = ctx.read_float();                                // damage
		obj.barrier = ctx.read_bool();                                // Barrier
		obj.blunt = ctx.read_bool();                                  // Blunt
		obj.edge = ctx.read_bool();                                   // Edge
		obj.fire = ctx.read_bool();                                   // Fire
		obj.fly = ctx.read_bool();                                    // Fly
		obj.magic = ctx.read_bool();                                  // Magic
		obj.point = ctx.read_bool();                                  // Point
		obj.fall = ctx.read_bool();                                   // Fall
		obj.repeat_delay_sec = ctx.read_float();                      // damageRepeatDelaySec
		obj.volume_scale = ctx.read_float();                          // damageVolDownScale
		obj.collision = static_cast<collision_type>(ctx.read_enum()); // damageCollType
	}

	void earthquake::parse(earthquake& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.radius = ctx.read_float();   // radius
		obj.duration = ctx.read_float(); // timeSec
		obj.amplitude = ctx.read_vec3(); // amplitudeCM
	}

	void vobs::npc::parse(vobs::npc& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);

		obj.npc_instance = ctx.read_string(); // npcInstance
		obj.model_scale = ctx.read_vec3();    // modelScale
		obj.model_fatness = ctx.read_float(); // modelFatness

		auto overlay_count = ctx.read_int(); // numOverlays
		for (auto i = 0; i < overlay_count; ++i) {
			obj.overlays.push_back(ctx.read_string()); // overlay
		}

		obj.flags = ctx.read_int();         // flags
		obj.guild = ctx.read_int();         // guild
		obj.guild_true = ctx.read_int();    // guildTrue
		obj.level = ctx.read_int();         // level
		obj.xp = ctx.read_int();            // xp
		obj.xp_next_level = ctx.read_int(); // xpnl
		obj.lp = ctx.read_int();            // lp

		auto talent_count = ctx.read_int(); // numTalents
		obj.talents.resize(talent_count);

		archive_object hdr;
		for (auto i = 0; i < talent_count; ++i) {
			if (!ctx.read_object_begin(hdr)) // [% oCNpcTalent 0 0]
				throw parser_error {"vobs::npc"};

			// empty object
			if (hdr.class_name == "%") {
				ctx.skip_object(true);
				continue;
			}

			obj.talents[i].talent = ctx.read_int(); // talent
			obj.talents[i].value = ctx.read_int();  // value
			obj.talents[i].skill = ctx.read_int();  // skill

			if (!ctx.read_object_end()) {
				PX_LOGW("vob_tree: oCNpcTalent object not fully parsed");
				ctx.skip_object(true);
			}
		}

		obj.fight_tactic = ctx.read_int(); // fightTactic
		obj.fight_mode = ctx.read_int();   // fightMode
		obj.wounded = ctx.read_bool();     // wounded
		obj.mad = ctx.read_bool();         // mad
		obj.mad_time = ctx.read_int();     // madTime
		obj.player = ctx.read_bool();      // player

		for (auto i = 0; i < 8; ++i) {
			obj.attributes[i] = ctx.read_int(); // atr0
		}

		if (version == game_version::gothic_2) {
			// TODO: what are these (hc<n>)?
			for (auto i = 0; i < 4; ++i) {
				obj.hcs[i] = ctx.read_int(); // hc1
			}
		}

		for (auto i = 0; i < 5; ++i) {
			obj.missions[i] = ctx.read_int(); // mission0
		}

		obj.start_ai_state = ctx.read_string(); // startAIState

		auto vars = ctx.read_raw_bytes((version == game_version::gothic_1 ? 50 : 100) * 4); // scriptVars
		for (auto i = 0u; i < vars.limit() / 4; ++i) {
			obj.aivar[i] = vars.get_int();
		}

		obj.script_waypoint = ctx.read_string(); // scriptWp
		obj.attitude = ctx.read_int();           // attitude
		obj.attitude_temp = ctx.read_int();      // tmpAttitude
		obj.name_nr = ctx.read_int();            // nameNr

		// unknown.
		[[maybe_unused]] auto spells = ctx.read_raw_bytes(4); // spells

		// TODO: News (I don't have a sample.).
		auto news_count = ctx.read_int(); // NumOfEntries
		if (news_count != 0) {
			PX_LOGE("!!! IMPORTANT !!! This save-game contains news entries and cannot be loaded currently. Please "
			        "open an issue at https://github.com/lmichaelis/phoenix providing your save-game as a ZIP file.");
			throw parser_error {"vobs::npc"};
		}

		ctx.skip_object(false); // [carryVob % 0 0]
		ctx.skip_object(false); // [enemy % 0 0]

		obj.move_lock = ctx.read_bool(); // moveLock

		if (version == game_version::gothic_1) {
			for (auto i = 0; i < 9; ++i) {
				obj.packed[i] = ctx.read_string(); // packed
			}
		} else {
			auto packed = ctx.read_string(); // packed
			auto it = packed.begin();
			auto idx = 0;

			do {
				auto next = std::find(it, packed.end(), ';');
				obj.packed[idx++].assign(it, next);
				it = next;
			} while (it != packed.end() && idx < 9);
		}

		auto item_count = ctx.read_int(); // itemCount
		obj.items.resize(item_count);

		for (auto i = 0; i < item_count; ++i) {
			if (!ctx.read_object_begin(hdr))
				throw parser_error {"vobs::npc"};

			obj.items[i] = std::make_unique<item>();
			item::parse(*obj.items[i], ctx, version);
			obj.items[i]->id = hdr.index;

			if (!ctx.read_object_end()) {
				PX_LOGW("vob_tree: oCItem:zCVob object not fully parsed");
				ctx.skip_object(true);
			}
		}

		auto inv_slot_count = ctx.read_int(); // numInvSlots
		obj.slots.resize(inv_slot_count);
		for (auto i = 0; i < inv_slot_count; ++i) {
			obj.slots[i].used = ctx.read_bool();   // used
			obj.slots[i].name = ctx.read_string(); // name

			if (obj.slots[i].used) {
				// [vob § 0 0]
				if (!ctx.read_object_begin(hdr) || !ctx.read_object_end())
					throw parser_error {"vobs::npc"};

				// TODO: Warn if not found.
				for (auto j = 0u; j < obj.items.size(); ++j) {
					if (obj.items[j]->id == hdr.index) {
						obj.slots[i].item_index = j;
						break;
					}
				}

				obj.slots[i].in_inventory = ctx.read_bool(); // inInv
			}
		}

		obj.current_state_valid = ctx.read_bool();      // curState.valid
		obj.current_state_name = ctx.read_string();     // curState.name
		obj.current_state_index = ctx.read_int();       // curState.prgIndex
		obj.current_state_is_routine = ctx.read_bool(); // curState.isRtnState
		obj.next_state_valid = ctx.read_bool();         // nextState.valid
		obj.next_state_name = ctx.read_string();        // nextState.name
		obj.next_state_index = ctx.read_int();          // nextState.prgIndex
		obj.next_state_is_routine = ctx.read_bool();    // nextState.isRtnState
		obj.last_ai_state = ctx.read_int();             // lastAIState
		obj.has_routine = ctx.read_bool();              // hasRoutine
		obj.routine_changed = ctx.read_bool();          // rtnChanged
		obj.routine_overlay = ctx.read_bool();          // rtnOverlay
		obj.routine_overlay_count = ctx.read_int();     // rtnOverlayCount
		obj.walkmode_routine = ctx.read_int();          // walkmode_routine
		obj.weaponmode_routine = ctx.read_bool();       // weaponmode_routine
		obj.start_new_routine = ctx.read_bool();        // startNewRoutine
		obj.ai_state_driven = ctx.read_int();           // aiStateDriven
		obj.ai_state_pos = ctx.read_vec3();             // aiStatePos
		obj.current_routine = ctx.read_string();        // curRoutine
		obj.respawn = ctx.read_bool();                  // respawn
		obj.respawn_time = ctx.read_int();              // respawnTime

		auto protection = ctx.read_raw_bytes(sizeof(int32_t) * 8); // protection
		for (auto i = 0; i < 8; ++i) {
			obj.protection[i] = protection.get_int();
		}

		if (version == game_version::gothic_2) {
			obj.bs_interruptable_override = ctx.read_int(); // bsInterruptableOverride
			obj.npc_type = ctx.read_int();                  // npcType
			obj.spell_mana = ctx.read_int();                // spellMana
		}
	}
} // namespace phoenix::vobs