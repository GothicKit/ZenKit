// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vobs/light.hh>

#include <sstream>

namespace phoenix::vobs {
	void light_preset::parse(light_preset& obj, archive_reader& in, game_version version) {
		obj.preset = in.read_string();                            // lightPresetInUse
		obj.light_type = static_cast<light_mode>(in.read_enum()); // lightType
		obj.range = in.read_float();                              // range
		obj.color = in.read_color();                              // color
		obj.cone_angle = in.read_float();                         // spotConeAngle
		obj.is_static = in.read_bool();                           // lightStatic
		obj.quality = static_cast<light_quality>(in.read_enum()); // lightQuality
		obj.lensflare_fx = in.read_string();                      // lensflareFX

		if (!obj.is_static) {
			obj.on = in.read_bool();                      // turnedOn
			auto range_ani_scale = in.read_string();      // rangeAniScale
			obj.range_animation_fps = in.read_float();    // rangeAniFPS
			obj.range_animation_smooth = in.read_bool();  // rangeAniSmooth
			auto color_animation_list = in.read_string(); // colorAniList
			obj.color_animation_fps = in.read_float();    // colorAniFPS
			obj.color_animation_smooth = in.read_bool();  // colorAniSmooth

			std::istringstream ranges {range_ani_scale};
			float value;
			while (ranges >> value) {
				obj.range_animation_scale.push_back(value);
			}

			std::istringstream colors {color_animation_list};
			colors.setf(std::ios::skipws);

			uint32_t r, g, b;
			char br = ' ';
			while (colors >> br >> r >> g >> b >> br) {
				obj.color_animation_list.emplace_back(r, g, b, 0);
			}

			if (version == game_version::gothic_2) {
				obj.can_move = in.read_bool(); // canMove
			}
		}
	}

	light_preset light_preset::parse(archive_reader& ctx, game_version version) {
		light_preset preset {};
		light_preset::parse(preset, ctx, version);
		return preset;
	}

	void light::parse(phoenix::vobs::light& obj, archive_reader& ctx, phoenix::game_version version) {
		vob::parse(obj, ctx, version);
		light_preset::parse(obj, ctx, version);
	}
} // namespace phoenix::vobs