// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Material.hh"
#include "zenkit/Archive.hh"

#include "phoenix/phoenix.hh"

#include "Internal.hh"

#include <sstream>

namespace zenkit {
	static constexpr auto MATERIAL_VERSION_G1 = 17408;
	[[maybe_unused]] static constexpr auto MATERIAL_VERSION_G2 = 0x9c03;

	Material Material::parse(ReadArchive& in) {
		Material mat {};
		mat.load(in);
		return mat;
	}

	void Material::load(ReadArchive& r) {
		// name of the material - ignored
		(void) r.read_string();

		ArchiveObject obj {};
		if (!r.read_object_begin(obj)) {
			throw zenkit::ParserError {"Material", "expected archive object begin which was not found"};
		}

		if (obj.class_name != "zCMaterial") {
			throw zenkit::ParserError {"Material", "expected archive class zCMaterial; got " + obj.class_name};
		}

		this->name = r.read_string();                            // name
		this->group = static_cast<MaterialGroup>(r.read_enum()); // matGroup
		this->color = r.read_color();                            // color
		this->smooth_angle = r.read_float();                     // smoothAngle
		this->texture = r.read_string();                         // texture

		std::istringstream s_texture_scale {r.read_string()}; // texScale
		s_texture_scale >> this->texture_scale.x >> this->texture_scale.y;

		this->texture_anim_fps = r.read_float(); // texAniFPS
		this->texture_anim_map_mode = static_cast<AnimationMapping>(r.read_enum());

		std::istringstream anim_map_dir {r.read_string()}; // texAniMapDir
		anim_map_dir >> this->texture_anim_map_dir.x >> this->texture_anim_map_dir.y;

		this->disable_collision = r.read_bool(); // noCollDet
		this->disable_lightmap = r.read_bool();  // noLightmap
		this->dont_collapse = r.read_bool();     // lodDontCollapse
		this->detail_object = r.read_string();

		if (obj.version == MATERIAL_VERSION_G1) {
			this->alpha_func = AlphaFunction::DEFAULT;
		} else {
			// This section is specific to G2
			this->detail_texture_scale = r.read_float();                  // detailObjectScale
			this->force_occluder = r.read_bool();                         // forceOccluder
			this->environment_mapping = r.read_bool();                    // environmentalMapping
			this->environment_mapping_strength = r.read_float();          // environmentalMappingStrength
			this->wave_mode = static_cast<WaveMode>(r.read_enum());       // waveMode
			this->wave_speed = static_cast<WaveSpeed>(r.read_enum());     // waveSpeed
			this->wave_max_amplitude = r.read_float();                    // waveMaxAmplitude
			this->wave_grid_size = r.read_float();                        // waveGridSize
			this->ignore_sun = r.read_bool();                             // ignoreSunLight
			this->alpha_func = static_cast<AlphaFunction>(r.read_enum()); // alphaFunc
		}

		this->default_mapping = r.read_vec2();

		if (!r.read_object_end()) {
			ZKLOGW("Material", "\"%s\" not fully parsed", this->name.c_str());
			r.skip_object(true);
		}
	}
} // namespace zenkit
