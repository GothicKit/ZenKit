// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <glm/vec2.hpp>
#include <phoenix/archive.hh>
#include <string>

namespace phoenix {
	/// \brief Alpha compositing modes used by the *ZenGin*.
	enum class alpha_function {
		default_ = 0,
		none = 1,
		blend = 2,
		add = 3,
		sub = 4,
		mul = 5,
		mul2 = 6,
	};

	/// \brief The general type of a material.
	enum class material_group : std::uint8_t {
		/// \brief A material group has not been assigned to the material.
		undefined = 0,
		metal = 1,
		stone = 2,
		wood = 3,
		earth = 4,
		water = 5,
		snow = 6,

		/// \brief The material group is explicitly not set. Added for [OpenGothic](https://github.com/Try/OpenGothic)
		///        compatibility. It does not exist in real Gothic or Gothic 2 materials.
		none = 0xFF,
	};

	/// \brief Wave animation speed modes.
	enum class wave_speed_type : std::uint8_t {
		none = 0,
		slow = 1,
		normal = 2,
		fast = 3,
	};

	/// \brief Wave animation modes.
	enum class wave_mode_type : std::uint8_t {
		none = 0,
		ambient_ground = 1,
		ground = 2,
		ambient_wall = 3,
		wall = 4,
		env = 5,
		ambient_wind = 6,
		wind = 7
	};

	/// \brief Texture animation mapping modes.
	enum class animation_mapping_mode {
		none = 0,
		linear = 1,
	};

	/// \brief Represents a *ZenGin* material.
	///
	/// <p>Materials describe the way things look and sound in the *ZenGin*. Among other things, materials control which
	/// texture is used for a model, if the player can collide with it and even some animations. Materials are normally
	/// embedded into meshes.</p>
	class material {
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
		/// \see #parse(std::unique_ptr<archive_reader>&&) for an owning version this function.
		[[nodiscard]] static material parse(std::unique_ptr<archive_reader>& ctx);

		/// \brief Parses a material from the given *ZenGin* archive.
		/// \param[in] ctx The archive reader to read from.
		/// \return The parsed material object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(std::unique_ptr<archive_reader>&)
		[[nodiscard]] inline static material parse(std::unique_ptr<archive_reader>&& in) {
			return material::parse(in);
		}

	public:
		std::string name;
		material_group group {material_group::undefined};
		glm::u8vec4 color {0, 0, 0, 0};
		float smooth_angle {0.0f};
		std::string texture {};
		glm::vec2 texture_scale {};
		float texture_anim_fps {0.0f};
		animation_mapping_mode texture_anim_map_mode {animation_mapping_mode::none};
		glm::vec2 texture_anim_map_dir {};
		bool disable_collision {false};
		bool disable_lightmap {false};
		bool dont_collapse {false};
		std::string detail_object;
		float detail_texture_scale {0.0f};
		bool force_occluder {false};
		bool environment_mapping {false};
		float environment_mapping_strength {0.0f};
		wave_mode_type wave_mode {wave_mode_type::none};
		wave_speed_type wave_speed {wave_speed_type::none};
		float wave_max_amplitude {0.0f};
		float wave_grid_size {0.0f};
		bool ignore_sun {false};
		alpha_function alpha_func {alpha_function::none};
		glm::vec2 default_mapping {};
	};
} // namespace phoenix
