// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/material.hh>

#include <sstream>

namespace phoenix {
	static constexpr auto MATERIAL_VERSION_G1_V108k = 17408;

	material material::parse(archive_reader_ref& in) {
		material mat {};

		// name of the material - ignored
		(void) in->read_string();

		archive_object obj {};
		if (!in->read_object_begin(obj)) {
			throw parser_error("expected archive object begin which was not found");
		}

		if (obj.class_name != "zCMaterial") {
			throw parser_error("expected archive class zCMaterial; got " + obj.class_name);
		}

		if (obj.version == MATERIAL_VERSION_G1_V108k) {
			mat._m_name = in->read_string();
			mat._m_group = static_cast<material_group>(in->read_byte()); // Quirk: This is not an enum
			mat._m_color = in->read_color();                             // Quirk: This is not a color
			mat._m_smooth_angle = in->read_float();
			mat._m_texture = in->read_string();

			std::istringstream texture_scale {in->read_string()};
			texture_scale >> mat._m_texture_scale.x >> mat._m_texture_scale.y;

			mat._m_texture_anim_fps = in->read_float();
			mat._m_texture_anim_map_mode = in->read_byte();

			std::istringstream anim_map_dir {in->read_string()};
			anim_map_dir >> mat._m_texture_anim_map_dir.x >> mat._m_texture_anim_map_dir.y;

			mat._m_disable_collision = in->read_bool();
			mat._m_disable_lightmap = in->read_bool();
			mat._m_dont_collapse = in->read_byte();
			mat._m_detail_object = in->read_string();
			mat._m_default_mapping = in->read_vec2();
		} else {
			mat._m_name = in->read_string();
			mat._m_group = static_cast<material_group>(in->read_byte()); // Quirk: This is not an enum
			mat._m_color = in->read_color();                             // Quirk: This is not a color
			mat._m_smooth_angle = in->read_float();
			mat._m_texture = in->read_string();

			std::istringstream texture_scale {in->read_string()};
			texture_scale >> mat._m_texture_scale.x >> mat._m_texture_scale.y;

			mat._m_texture_anim_fps = in->read_float();
			mat._m_texture_anim_map_mode = in->read_byte();

			std::istringstream anim_map_dir {in->read_string()};
			anim_map_dir >> mat._m_texture_anim_map_dir.x >> mat._m_texture_anim_map_dir.y;

			mat._m_disable_collision = in->read_bool();
			mat._m_disable_lightmap = in->read_bool();
			mat._m_dont_collapse = in->read_byte();
			mat._m_detail_object = in->read_string();

			// This section is specific to G2
			mat._m_detail_texture_scale = in->read_float();
			mat._m_force_occluder = in->read_byte();
			mat._m_environment_mapping = in->read_byte();
			mat._m_environment_mapping_strength = in->read_float();
			mat._m_wave_mode = in->read_byte();
			mat._m_wave_speed = in->read_byte();
			mat._m_wave_max_amplitude = in->read_float();
			mat._m_wave_grid_size = in->read_float();
			mat._m_ignore_sun = in->read_byte();
			mat._m_alpha_func = in->read_byte();

			// The mapping comes last :)
			mat._m_default_mapping = in->read_vec2();
		}

		if (!in->read_object_end()) {
			throw parser_error("expected end of archive object which was not found.");
		}

		return mat;
	}
} // namespace phoenix
