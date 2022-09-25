// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/mob.hh>

namespace phoenix::vobs {
	void mob::parse(mob& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.name = ctx.read_string();                                // focusName
		obj.hp = ctx.read_int();                                     // hitpoints
		obj.damage = ctx.read_int();                                 // damage
		obj.movable = ctx.read_bool();                               // moveable
		obj.takable = ctx.read_bool();                               // takeable
		obj.focus_override = ctx.read_bool();                        // focusOverride
		obj.material = static_cast<sound_material>(ctx.read_enum()); // soundMaterial
		obj.visual_destroyed = ctx.read_string();                    // visualDestroyed
		obj.owner = ctx.read_string();                               // owner
		obj.owner_guild = ctx.read_string();                         // ownerGuild
		obj.destroyed = ctx.read_bool();                             // isDestroyed
	}

	void mob_inter::parse(mob_inter& obj, archive_reader& ctx, game_version version) {
		mob::parse(obj, ctx, version);
		obj.state = ctx.read_int();                       // stateNum
		obj.target = ctx.read_string();                   // triggerTarget
		obj.item = ctx.read_string();                     // useWithItem
		obj.condition_function = ctx.read_string();       // conditionFunc
		obj.on_state_change_function = ctx.read_string(); // onStateFunc
		obj.rewind = ctx.read_bool();                     // rewind
	}

	void mob_container::parse(mob_container& obj, archive_reader& ctx, game_version version) {
		mob_inter::parse(obj, ctx, version);
		obj.locked = ctx.read_bool();        // locked
		obj.key = ctx.read_string();         // keyInstance
		obj.pick_string = ctx.read_string(); // pickLockStr
		obj.contents = ctx.read_string();    // contains
	}

	void mob_door::parse(mob_door& obj, archive_reader& ctx, game_version version) {
		mob_inter::parse(obj, ctx, version);
		obj.locked = ctx.read_bool();        // locked
		obj.key = ctx.read_string();         // keyInstance
		obj.pick_string = ctx.read_string(); // pickLockStr
	}

	void mob_fire::parse(mob_fire& obj, archive_reader& ctx, game_version version) {
		mob_inter::parse(obj, ctx, version);
		obj.slot = ctx.read_string();     // fireSlot
		obj.vob_tree = ctx.read_string(); // fireVobtreeName
	}
} // namespace phoenix::vobs