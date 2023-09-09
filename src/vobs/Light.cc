// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Light.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

#include <sstream>

namespace zenkit::vobs {
	void LightPreset::parse(LightPreset& obj, ReadArchive& in, GameVersion version) {
		obj.load(in, version);
	}

	void LightPreset::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		this->preset = r.read_string();                           // lightPresetInUse
		this->light_type = static_cast<LightType>(r.read_enum()); // lightType
		this->range = r.read_float();                             // range
		this->color = r.read_color();                             // color
		this->cone_angle = r.read_float();                        // spotConeAngle
		this->is_static = r.read_bool();                          // lightStatic
		this->quality = static_cast<LightQuality>(r.read_enum()); // lightQuality
		this->lensflare_fx = r.read_string();                     // lensflareFX

		if (!this->is_static) {
			this->on = r.read_bool();                     // turnedOn
			auto range_ani_scale = r.read_string();       // rangeAniScale
			this->range_animation_fps = r.read_float();   // rangeAniFPS
			this->range_animation_smooth = r.read_bool(); // rangeAniSmooth
			auto cal = r.read_string();                   // colorAniList
			this->color_animation_fps = r.read_float();   // colorAniFPS
			this->color_animation_smooth = r.read_bool(); // colorAniSmooth

			std::istringstream ranges {range_ani_scale};
			float value;
			while (ranges >> value) {
				this->range_animation_scale.push_back(value);
			}

			std::istringstream colors {cal};
			colors.setf(std::ios::skipws);

			// # Original Format in ABNF:
			//
			//   color-ani-list = *(color-ani-list-element SP)
			//   color-ani-list-element = greyscale-color-element | rgb-color-element
			//
			//   greyscale-color-element = color-scalar
			//   rgb-color-element = "(" color-scalar SP color-scalar SP color-scalar ")"
			//
			//   color-scalar = 1*3DIGIT

			char c;
			uint32_t cr, cg, cb;
			while (colors >> c) {
				if (::isdigit(c)) {
					colors.unget();
					colors >> cr;
					this->color_animation_list.emplace_back(cr, cr, cr, 255);
					continue;
				}

				if (c != '(') {
					ZKLOGW("LightPreset", "Failed parsing `colorAniList`: invalid char '%c'", c);
				}

				colors >> cr >> cg >> cb >> c;

				if (c != ')') {
					ZKLOGW("LightPreset", "Failed parsing `colorAniList`: expected ')', got '%c'", c);
				}

				this->color_animation_list.emplace_back(cr, cg, cb, 255);
			}

			if (version == GameVersion::GOTHIC_2) {
				this->can_move = r.read_bool(); // canMove
			}
		}
	}

	LightPreset LightPreset::parse(ReadArchive& ctx, GameVersion version) {
		LightPreset preset {};
		preset.load(ctx, version);
		return preset;
	}

	void Light::parse(Light& obj, ReadArchive& ctx, GameVersion version) {
		obj.load(ctx, version);
	}

	void Light::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		VirtualObject::load(r, version);
		LightPreset::load(r, version);
	}
} // namespace zenkit::vobs
