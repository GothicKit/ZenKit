// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/misc.hh>

namespace phoenix::vobs {
	void animate::parse(animate& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.start_on = ctx.read_bool(); // startOn
	}

	void item::parse(item& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.instance = ctx.read_string(); // itemInstance
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
		for (int i = 0; i < slave_count; ++i) {
			obj.slaves.emplace_back(ctx.read_string()); // slaveVobName[i]
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
} // namespace phoenix::vobs