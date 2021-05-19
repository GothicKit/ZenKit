// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/world/vob_tree.hh>

#include <fmt/format.h>

namespace phoenix {

	vob_tree vob_tree::parse(archive_reader_ref& in, game_version version) {
		vob_tree vob {};

		archive_object obj;
		if (!in->read_object_begin(obj)) {
			throw parser_error("vob_tree: expected object where there was none");
		}

		auto packed = in->read_int() != 0;

		if (packed) {
			auto raw = in->read_raw_bytes();
			reader bin {{(const char*) raw.data(), raw.size()}};

			vob._m_bbox[0] = bin.read_vec3();
			vob._m_bbox[1] = bin.read_vec3();
			vob._m_position = bin.read_vec3();
			vob._m_rotation[0] = bin.read_vec3();
			vob._m_rotation[1] = bin.read_vec3();
			vob._m_rotation[2] = bin.read_vec3();

			u8 bit0 = bin.read_u8();
			u16 bit1;

			if (version == game_version::gothic_1) {
				bit1 = bin.read_u8();
			} else {
				bit1 = bin.read_u16();
			}

			vob._m_show_visual = static_cast<bool>((bit0 & 0b00000001) >> 0);
			vob._m_camera_alignment = (bit0 & 0b00000110) >> 1;
			vob._m_cd_static = static_cast<bool>((bit0 & 0b00001000) >> 3);
			vob._m_cd_dynamic = static_cast<bool>((bit0 & 0b00010000) >> 4);
			vob._m_vob_static = static_cast<bool>((bit0 & 0b00100000) >> 5);
			vob._m_dynamic_shadows = (bit0 & 0b11000000) >> 6;

			bool has_preset_name = static_cast<bool>((bit1 & 0b000000000000001u) >> 0u);
			bool has_vob_name = static_cast<bool>((bit1 & 0b000000000000010u) >> 1u);
			bool has_visual_name = static_cast<bool>((bit1 & 0b000000000000100u) >> 2u);
			bool has_visual_object = static_cast<bool>((bit1 & 0b000000000001000u) >> 3u);
			bool has_ai_object = static_cast<bool>((bit1 & 0b000000000010000u) >> 4u);
			// bool has_event_man_object = static_cast<bool>((bit1 & 0b000000000100000u) >> 5u);
			vob._m_physics_enabled = static_cast<bool>((bit1 & 0b000000001000000u) >> 6u);

			if (version == game_version::gothic_2) {
				vob._m_animation_mode = (bit1 & 0b000000110000000u >> 7u);
				vob._m_bias = static_cast<s32>((bit1 & 0b011111000000000u) >> 9u);
				vob._m_ambient = static_cast<bool>((bit1 & 0b100000000000000u) >> 14u);

				vob._m_animation_strength = bin.read_f32();
				vob._m_far_clip_scale = bin.read_f32();
			}

			if (has_preset_name) {
				vob._m_preset_name = in->read_string();
			}

			if (has_vob_name) {
				vob._m_vob_name = in->read_string();
			}

			if (has_visual_name) {
				vob._m_visual_name = in->read_string();
			}

			if (has_visual_object) {
				in->skip_object(false);
			}

			if (has_ai_object) {
				in->skip_object(false);
			}
		} else {
			vob._m_preset_name = in->read_string();

			auto [min, max] = in->read_bbox();
			vob._m_bbox[0] = min;
			vob._m_bbox[1] = max;

			vob._m_rotation = in->read_mat3x3();
			vob._m_position = in->read_vec3();

			vob._m_vob_name = in->read_string();
			vob._m_visual_name = in->read_string();
			vob._m_show_visual = in->read_bool();
			vob._m_camera_alignment = in->read_enum();

			if (version == game_version::gothic_1) {
				vob._m_cd_static = in->read_bool();
				vob._m_cd_dynamic = in->read_bool();
				vob._m_vob_static = in->read_bool();
				vob._m_dynamic_shadows = in->read_enum();
				// vob._m_animation_mode = in->read_byte();
			} else {
				vob._m_animation_mode = in->read_enum();
				vob._m_animation_strength = in->read_float();
				vob._m_far_clip_scale = in->read_float();
				vob._m_cd_static = in->read_bool();
				vob._m_cd_dynamic = in->read_bool();
				vob._m_vob_static = in->read_bool();
				vob._m_dynamic_shadows = in->read_enum();
				vob._m_bias = in->read_int();
				vob._m_ambient = in->read_bool();
			}

			// visual
			in->skip_object(false);

			// ai
			in->skip_object(false);
		}

		if (!in->read_object_end()) {
			in->skip_object(true);
		}

		auto child_count = in->read_int();
		vob._m_children.reserve(child_count);

		for (int i = 0; i < child_count; ++i) {
			vob._m_children.emplace_back(vob_tree::parse(in, version));
		}

		return vob;
	}
}// namespace phoenix