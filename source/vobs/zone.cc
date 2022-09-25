// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/zone.hh>

namespace phoenix::vobs {
	void zone_music::parse(zone_music& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.enabled = ctx.read_bool();   // enabled
		obj.priority = ctx.read_int();   // priority
		obj.ellipsoid = ctx.read_bool(); // ellipsoid
		obj.reverb = ctx.read_float();   // reverbLevel
		obj.volume = ctx.read_float();   // volumeLevel
		obj.loop = ctx.read_bool();      // loop
	}

	void zone_far_plane::parse(zone_far_plane& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.vob_far_plane_z = ctx.read_float();        // vobFarPlaneZ
		obj.inner_range_percentage = ctx.read_float(); // innerRangePerc
	}

	void zone_fog::parse(zone_fog& obj, archive_reader& ctx, game_version version) {
		vob::parse(obj, ctx, version);
		obj.range_center = ctx.read_float();           // fogRangeCenter
		obj.inner_range_percentage = ctx.read_float(); // innerRangePerc
		obj.color = ctx.read_color();                  // fogColor

		if (version == game_version::gothic_2) {
			obj.fade_out_sky = ctx.read_bool();   // fadeOutSky
			obj.override_color = ctx.read_bool(); // overrideColor
		}
	}
} // namespace phoenix::vobs
