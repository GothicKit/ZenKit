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

	void Material::save(WriteArchive& w, GameVersion version) const {
		w.write_string("", this->name);

		w.write_object_begin("%",
		                     "zCMaterial",
		                     version == GameVersion::GOTHIC_1 ? MATERIAL_VERSION_G1 : MATERIAL_VERSION_G2);
		w.write_string("name", this->name);
		w.write_enum("matGroup", static_cast<uint32_t>(this->group));
		w.write_color("color", this->color);
		w.write_float("smoothAngle", this->smooth_angle);
		w.write_string("texture", this->texture);

		char buf[100];

		{
			snprintf(buf, sizeof buf - 1, "%.9g %.9g", this->texture_scale.x, this->texture_scale.y);
			w.write_string("texScale", buf);
		}

		w.write_float("texAniFPS", this->texture_anim_fps);
		w.write_enum("texAniMapMode", static_cast<uint32_t>(this->texture_anim_map_mode));

		{
			snprintf(buf, sizeof buf - 1, "%.9g %.9g", this->texture_anim_map_dir.x, this->texture_anim_map_dir.y);
			w.write_string("texAniMapDir", buf);
		}

		w.write_bool("noCollDet", this->disable_collision);
		w.write_bool("noLightmap", this->disable_lightmap);
		w.write_bool("losDontCollapse", this->dont_collapse);
		w.write_string("detailObject", this->detail_object);

		if (version == GameVersion::GOTHIC_2) {
			w.write_float("detailObjectScale", this->detail_object_scale);
			w.write_bool("forceOccluder", this->force_occluder);
			w.write_bool("environmentalMapping", this->environment_mapping);
			w.write_float("environmentalMappingStrength", this->environment_mapping_strength);
			w.write_enum("waveMode", static_cast<uint32_t>(this->wave_mode));
			w.write_enum("waveSpeed", static_cast<uint32_t>(this->wave_speed));
			w.write_float("waveMaxAmplitude", this->wave_max_amplitude);
			w.write_float("waveGridSize", this->wave_grid_size);
			w.write_bool("ignoreSunLight", this->ignore_sun);
			w.write_bool("alphaFunc", static_cast<bool>(this->alpha_func));
		}

		w.write_vec2("defaultMapping", this->default_mapping);
		w.write_object_end();
	}
} // namespace zenkit
