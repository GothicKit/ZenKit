// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <glm/vec2.hpp>
#include <phoenix/archive.hh>
#include <string>

namespace phoenix {
	enum class alpha_function {
		test,
		transparent,
		additive,
		multiply,
	};

	alpha_function alpha_function_from_int(std::uint32_t b);

	enum class material_group : std::uint8_t {
		undef = 0,
		metal = 1,
		stone = 2,
		wood = 3,
		earth = 4,
		water = 5,
		snow = 6,
		none = 7,
	};

	/**
	 * @brief Represents a material.
	 *
	 * Parses ZenGin materials. These are normally found embedded within meshes.
	 * The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zCMaterial.cpp and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCMaterial.cpp
	 *
	 * Thanks to the original author, Andre Taulien as well as Try for additional work on their
	 * ZenLib fork!
	 *
	 * @see https://github.com/Try/ZenLib
	 * @see https://github.com/ataulien/ZenLib
	 */
	class material {
	public:
		/**
		 * @brief Reads a material from the archive.
		 * @param[in,out] in The reader to read from.
		 * @return The material.
		 * @throws parser_error if parsing fails.
		 */
		static material parse(archive_reader_ref& in);

	public:
		std::string name;
		material_group group;
		glm::u8vec4 color {0, 0, 0, 0};
		float smooth_angle {0.0f};
		std::string texture;
		glm::vec2 texture_scale;
		float texture_anim_fps {0.0f};
		std::uint8_t texture_anim_map_mode {0}; // TODO: this might be a bool or an enum
		glm::vec2 texture_anim_map_dir;
		bool disable_collision {false};
		bool disable_lightmap {false};
		bool dont_collapse {false};
		std::string detail_object;
		float detail_texture_scale {0.0f};
		bool force_occluder {false};
		std::uint8_t environment_mapping {0}; // TODO: this might be a bool or an enum
		float environment_mapping_strength {0.0f};
		std::uint8_t wave_mode {0}; // TODO: this might be a bool or an enum
		std::uint8_t wave_speed {0};
		float wave_max_amplitude {0.0f};
		float wave_grid_size {0.0f};
		bool ignore_sun {false};
		alpha_function alpha_func {0};
		glm::vec2 default_mapping {};
	};

} // namespace phoenix
