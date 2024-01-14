// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/Light.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

#include <sstream>

namespace zenkit {
	void LightPreset::parse(LightPreset& obj, ReadArchive& in, GameVersion version) {
		obj.load(in, version);
	}

	void LightPreset::load(ReadArchive& r, GameVersion version) {
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
				if (isdigit(c)) {
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

	void LightPreset::save(WriteArchive& w, GameVersion version) const {
		w.write_string("lightPresetInUse", this->preset);
		w.write_enum("lightType", static_cast<std::uint32_t>(this->light_type));
		w.write_float("range", this->range);
		w.write_color("color", this->color);
		w.write_float("spotConeAngle", this->cone_angle);
		w.write_bool("lightStatic", this->is_static);
		w.write_enum("lightQuality", static_cast<std::uint32_t>(this->quality));
		w.write_string("lensflareFX", this->lensflare_fx);

		if (!this->is_static) {
			w.write_bool("turnedOn", this->on);

			std::ostringstream os;
			for (auto& r : this->range_animation_scale) {
				os << r << " ";
			}
			w.write_string("rangeAniScale", os.str());

			w.write_float("rangeAniFPS", this->range_animation_fps);
			w.write_bool("rangeAniSmooth", this->range_animation_smooth);

			os.clear();
			for (auto& c : this->color_animation_list) {
				os << "(" << static_cast<int>(c.r) << " " << static_cast<int>(c.g) << " " << static_cast<int>(c.b)
				   << ") ";
			}
			w.write_string("colorAniList", os.str());

			w.write_float("colorAniFPS", this->color_animation_fps);
			w.write_bool("colorAniSmooth", this->color_animation_smooth);

			if (version == GameVersion::GOTHIC_2) {
				w.write_bool("canMove", this->can_move);
			}
		}
	}

	LightPreset LightPreset::parse(ReadArchive& ctx, GameVersion version) {
		LightPreset preset {};
		preset.load(ctx, version);
		return preset;
	}

	void VLight::parse(VLight& obj, ReadArchive& ctx, GameVersion version) {
		obj.load(ctx, version);
	}

	void VLight::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		LightPreset::load(r, version);
	}

	void VLight::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		LightPreset::save(w, version);
	}

	uint16_t VLight::get_version_identifier(GameVersion game) const {
		return game == GameVersion::GOTHIC_1 ? 46080 : 39168;
	}
} // namespace zenkit
