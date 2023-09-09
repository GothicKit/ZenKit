// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>

namespace zenkit {
	class ReadArchive;

	/// \brief Alpha compositing modes used by the *ZenGin*.
	enum class AlphaFunction {
		DEFAULT = 0,
		NONE = 1,
		BLEND = 2,
		ADD = 3,
		SUBTRACT = 4,
		MULTIPLY = 5,
		MULTIPLY_ALT = 6,

		// Deprecated entries.
		default_ ZKREM("renamed to AlphaFunction::DEFAULT") = DEFAULT,
		none ZKREM("renamed to AlphaFunction::NONE") = NONE,
		blend ZKREM("renamed to AlphaFunction::BLEND") = BLEND,
		add ZKREM("renamed to AlphaFunction::ADD") = ADD,
		sub ZKREM("renamed to AlphaFunction::SUBTRACT") = SUBTRACT,
		mul ZKREM("renamed to AlphaFunction::MULTIPLY") = MULTIPLY,
		mul2 ZKREM("renamed to AlphaFunction::MULTIPLY_ALT") = MULTIPLY_ALT,
	};

	/// \brief The general type of a material.
	enum class MaterialGroup : std::uint8_t {
		/// \brief A material group has not been assigned to the material.
		UNDEFINED = 0,
		METAL = 1,
		STONE = 2,
		WOOD = 3,
		EARTH = 4,
		WATER = 5,
		SNOW = 6,

		/// \brief The material group is explicitly not set. Added for [OpenGothic](https://github.com/Try/OpenGothic)
		///        compatibility. It does not exist in real Gothic or Gothic 2 materials.
		NONE = 0xFF,

		// Deprecated entries.
		undefined ZKREM("renamed to MaterialGroup::UNDEFINED") = UNDEFINED,
		metal ZKREM("renamed to MaterialGroup::METAL") = METAL,
		stone ZKREM("renamed to MaterialGroup::STONE") = STONE,
		wood ZKREM("renamed to MaterialGroup::WOOD") = WOOD,
		earth ZKREM("renamed to MaterialGroup::EARTH") = EARTH,
		water ZKREM("renamed to MaterialGroup::WATER") = WATER,
		snow ZKREM("renamed to MaterialGroup::SNOW") = SNOW,
		none ZKREM("renamed to MaterialGroup::NONE") = NONE,
	};

	/// \brief Wave animation speed modes.
	enum class WaveSpeed : std::uint8_t {
		NONE = 0,
		SLOW = 1,
		NORMAL = 2,
		FAST = 3,

		// Deprecated entries.
		none ZKREM("renamed to WaveSpeed::NONE") = NONE,
		slow ZKREM("renamed to WaveSpeed::SLOW") = SLOW,
		normal ZKREM("renamed to WaveSpeed::NORMAL") = NORMAL,
		fast ZKREM("renamed to WaveSpeed::FAST") = FAST,
	};

	/// \brief Wave animation modes.
	enum class WaveMode : std::uint8_t {
		NONE = 0,
		GROUND_AMBIENT = 1,
		GROUND = 2,
		WALL_AMBIENT = 3,
		WALL = 4,
		ENVIRONMENT = 5,
		WIND_AMBIENT = 6,
		WIND = 7,

		// Deprecated entries.
		none ZKREM("renamed to WaveType::NONE") = NONE,
		ambient_ground ZKREM("renamed to WaveType::GROUND_AMBIENT") = GROUND_AMBIENT,
		ground ZKREM("renamed to WaveType::GROUND") = GROUND,
		ambient_wall ZKREM("renamed to WaveType::WALL_AMBIENT") = WALL_AMBIENT,
		wall ZKREM("renamed to WaveType::WALL") = WALL,
		env ZKREM("renamed to WaveType::ENVIRONMENT") = ENVIRONMENT,
		ambient_wind ZKREM("renamed to WaveType::WIND_AMBIENT") = WIND_AMBIENT,
		wind ZKREM("renamed to WaveType::WIND") = WIND,
	};

	/// \brief Texture animation mapping modes.
	enum class AnimationMapping {
		NONE = 0,
		LINEAR = 1,

		// Deprecated entries.
		none ZKREM("renamed to AnimationMapping::NONE") = NONE,
		linear ZKREM("renamed to AnimationMapping::LINEAR") = LINEAR,
	};

	/// \brief Represents a *ZenGin* material.
	///
	/// <p>Materials describe the way things look and sound in the *ZenGin*. Among other things, materials control which
	/// texture is used for a model, if the player can collide with it and even some animations. Materials are normally
	/// embedded into meshes.</p>
	class Material {
	public:
		/// \brief Parses a material from the given *ZenGin* archive.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib). Changes include the addition of the enumerations
		/// animation_mapping_mode, wave_mode_type and wave_speed_type and an updated version of alpha_function.</p>
		///
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \return The parsed material object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(archive_reader&&) for an owning version this function.
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static Material parse(ReadArchive& ctx);

		ZKAPI void load(ReadArchive& r);

	public:
		std::string name;
		MaterialGroup group {MaterialGroup::UNDEFINED};
		glm::u8vec4 color {0, 0, 0, 0};
		float smooth_angle {0.0f};
		std::string texture {};
		glm::vec2 texture_scale {};
		float texture_anim_fps {0.0f};
		AnimationMapping texture_anim_map_mode {AnimationMapping::NONE};
		glm::vec2 texture_anim_map_dir {};
		bool disable_collision {false};
		bool disable_lightmap {false};
		bool dont_collapse {false};
		std::string detail_object;
		float detail_texture_scale {0.0f};
		bool force_occluder {false};
		bool environment_mapping {false};
		float environment_mapping_strength {0.0f};
		WaveMode wave_mode {WaveMode::NONE};
		WaveSpeed wave_speed {WaveSpeed::NONE};
		float wave_max_amplitude {0.0f};
		float wave_grid_size {0.0f};
		bool ignore_sun {false};
		AlphaFunction alpha_func {AlphaFunction::NONE};
		glm::vec2 default_mapping {};
	};
} // namespace zenkit
