// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/misc.hh>

namespace phoenix::vobs {
	void animate::parse(animate& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.start_on = ctx.read_bool(); // startOn

		if (obj.saved) {
			// TODO: in save-games zone music behaves differently
			(void) ctx.read_bool(); // isRunning
		}
	}

	void item::parse(item& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.instance = ctx.read_string(); // itemInstance

		if (obj.saved) {
			// TODO: items in save-games contain two extra variables
			(void) ctx.read_int(); // amount
			(void) ctx.read_int(); // flags
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
			// TODO: in G2 save-games code master behaves differently
			(void) ctx.read_byte(); // numSlavesTriggered

			for (auto i = 0; i < slave_count; ++i) {
				// [slaveTriggered1 % 0 0]
				ctx.skip_object(false);
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

		obj.npc_instance = ctx.read_string();
		obj.model_scale = ctx.read_vec3();
		obj.model_fatness = ctx.read_float();

		auto overlay_count = ctx.read_int(); // numOverlays
		for (auto i = 0; i < overlay_count; ++i) {
			obj.overlays.push_back(ctx.read_string()); // overlay
		}

		obj.flags = ctx.read_int();
		obj.guild = ctx.read_int();
		obj.guild_true = ctx.read_int();
		obj.level = ctx.read_int();
		obj.xp = ctx.read_int();
		obj.xp_next_level = ctx.read_int();
		obj.lp = ctx.read_int();

		auto talent_count = ctx.read_int();
		obj.talents.resize(talent_count);

		archive_object hdr;
		for (auto i = 0; i < talent_count; ++i) {
			if (!ctx.read_object_begin(hdr))
				throw parser_error {"vobs::npc"};

			// empty object
			if (hdr.class_name == "%") {
				ctx.skip_object(true);
				continue;
			}

			obj.talents[i].talent = ctx.read_int();
			obj.talents[i].value = ctx.read_int();
			obj.talents[i].skill = ctx.read_int();

			if (!ctx.read_object_end()) {
				PX_LOGW("vob_tree: oCNpcTalent object not fully parsed");
				ctx.skip_object(true);
			}
		}

		obj.fight_tactic = ctx.read_int();
		obj.fight_mode = ctx.read_int();
		obj.wounded = ctx.read_bool();
		obj.mad = ctx.read_bool();
		obj.mad_time = ctx.read_int();
		obj.player = ctx.read_bool();

		for (auto i = 0; i < 8; ++i) {
			obj.attributes[i] = ctx.read_int();
		}

		if (version == game_version::gothic_2) {
			// TODO: what are these (hc<n>)?
			for (auto i = 0; i < 4; ++i) {
				(void) ctx.read_int(); // hc1
			}
		}

		for (auto i = 0; i < 5; ++i) {
			obj.missions[i] = ctx.read_int();
		}

		obj.start_ai_state = ctx.read_string();

		auto vars = ctx.read_raw_bytes((version == game_version::gothic_1 ? 50 : 100) * 4);
		for (auto i = 0u; i < vars.limit() / 4; ++i) {
			obj.aivar[i] = vars.get_int();
		}

		obj.script_waypoint = ctx.read_string();
		obj.attitude = ctx.read_int();
		obj.attitude_temp = ctx.read_int();
		obj.name_nr = ctx.read_int();

		// unknown.
		[[maybe_unused]] auto spells = ctx.read_raw_bytes(4);
		[[maybe_unused]] auto unknown_count = ctx.read_int();

		// [carryVob % 0 0]
		ctx.skip_object(false);

		// [enemy % 0 0]
		ctx.skip_object(false);

		obj.moveLock = ctx.read_bool();

		if (version == game_version::gothic_1) {
			for (auto i = 0; i < 9; ++i) {
				obj.packed[i] = ctx.read_string();
			}
		} else {
			(void) ctx.read_string();
		}

		auto item_count = ctx.read_int();
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

		auto inv_slot_count = ctx.read_int();
		obj.slots.resize(inv_slot_count);
		for (auto i = 0; i < inv_slot_count; ++i) {
			obj.slots[i].used = ctx.read_bool();
			obj.slots[i].name = ctx.read_string();

			if (obj.slots[i].used) {
				if (!ctx.read_object_begin(hdr) || !ctx.read_object_end())
					throw parser_error {"vobs::npc"};

				// TODO: Warn if not found.
				for (auto j = 0u; j < obj.items.size(); ++j) {
					if (obj.items[j]->id == hdr.index) {
						obj.slots[i].item_index = j;
						break;
					}
				}

				obj.slots[i].in_inventory = ctx.read_bool();
			}
		}

		obj.current_state_valid = ctx.read_bool();
		obj.current_state_name = ctx.read_string();
		obj.current_state_index = ctx.read_int();
		obj.current_state_is_routine = ctx.read_bool();
		obj.next_state_valid = ctx.read_bool();
		obj.next_state_name = ctx.read_string();
		obj.next_state_index = ctx.read_int();
		obj.next_state_is_routine = ctx.read_bool();
		obj.last_ai_state = ctx.read_int();
		obj.has_routine = ctx.read_bool();
		obj.routine_changed = ctx.read_bool();
		obj.routine_overlay = ctx.read_bool();
		obj.routine_overlay_count = ctx.read_int();
		obj.walkmode_routine = ctx.read_int();
		obj.weaponmode_routine = ctx.read_bool();
		obj.start_new_routine = ctx.read_bool();
		obj.ai_state_driven = ctx.read_int();
		obj.ai_state_pos = ctx.read_vec3();
		obj.current_routine = ctx.read_string();
		obj.respawn = ctx.read_bool();
		obj.respawn_time = ctx.read_int();

		auto protection = ctx.read_raw_bytes(sizeof(int32_t) * 8);
		for (auto i = 0; i < 8; ++i) {
			obj.protection[i] = protection.get_int();
		}

		if (version == game_version::gothic_2) {
			// TODO: these need to be saved
			(void) ctx.read_int(); // bsInterruptableOverride
			(void) ctx.read_int(); // npcType
			(void) ctx.read_int(); // spellMana
		}
	}
} // namespace phoenix::vobs