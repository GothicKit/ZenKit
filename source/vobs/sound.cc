// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/sound.hh>

namespace phoenix::vobs {
	void sound::parse(sound& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.volume = ctx.read_float();                                        // sndVolume
		obj.mode = static_cast<sound_mode>(ctx.read_enum());                  // sndMode
		obj.random_delay = ctx.read_float();                                  // sndRandDelay
		obj.random_delay_var = ctx.read_float();                              // sndRandDelayVar
		obj.initially_playing = ctx.read_bool();                              // sndStartOn
		obj.ambient3d = ctx.read_bool();                                      // sndAmbient3D
		obj.obstruction = ctx.read_bool();                                    // sndObstruction
		obj.cone_angle = ctx.read_float();                                    // sndConeAngle
		obj.volume_type = static_cast<sound_trigger_volume>(ctx.read_enum()); // sndVolType
		obj.radius = ctx.read_float();                                        // sndRadius
		obj.sound_name = ctx.read_string();                                   // sndName
	}

	void sound_daytime::parse(sound_daytime& obj, archive_reader& ctx, game_version version) {
		sound::parse(obj, ctx, version);
		obj.start_time = ctx.read_float();   // sndStartTime
		obj.end_time = ctx.read_float();     // sndEndTime
		obj.sound_name2 = ctx.read_string(); // sndName2
	}
} // namespace phoenix::vobs
