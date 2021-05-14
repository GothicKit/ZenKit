// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <glm/vec2.hpp>
#include <phoenix/archive.hh>
#include <string>

namespace phoenix {
	enum class material_group : u8 {
		undef = 0,
		metal = 1,
		stone = 2,
		wood = 3,
		earth = 4,
		water = 5,
		snow = 6,
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
		 */
		static material read(archive_reader_ref& in);

		/**
		 * @return The name of the material.
		 */
		[[nodiscard]] const std::string& name() const noexcept { return _m_name; }

		/**
		 * @return The group of the material.
		 */
		[[nodiscard]] material_group group() const noexcept { return _m_group; }

		/**
		 * @return The color of the material.
		 */
		[[nodiscard]] ::color color() const noexcept { return _m_color; }

		/**
		 * @return The smooth angle.
		 */
		[[nodiscard]] float smooth_angle() const noexcept { return _m_smooth_angle; }

		/**
		 * @return The texture of the material.
		 */
		[[nodiscard]] const std::string& texture() const noexcept { return _m_texture; }

		/**
		 * @return The scale of the texture.
		 */
		[[nodiscard]] glm::vec2 texture_scale() const noexcept { return _m_texture_scale; }

		/**
		 * @return The FPS of the animation.
		 */
		[[nodiscard]] float texture_anim_fps() const noexcept { return _m_texture_anim_fps; }

		/**
		 * @return The mode of the animation.
		 */
		[[nodiscard]] u8 texture_anim_map_mode() const noexcept { return _m_texture_anim_map_mode; }

		/**
		 * @return The directory of the animation texture.
		 */
		[[nodiscard]] glm::vec2 texture_anim_map_dir() const noexcept { return _m_texture_anim_map_dir; }

		/**
		 * @return Is collision disabled?
		 */
		[[nodiscard]] bool disable_collision() const noexcept { return _m_disable_collision; }

		/**
		 * @return Is the lightmap disabled?
		 */
		[[nodiscard]] bool disable_lightmap() const noexcept { return _m_disable_lightmap; }

		/**
		 * @return
		 */
		[[nodiscard]] u8 dont_collapse() const noexcept { return _m_dont_collapse; }

		/**
		 * @return
		 */
		[[nodiscard]] const std::string& detail_object() const noexcept { return _m_detail_object; }

		/**
		 * @return The detailed texture scale
		 */
		[[nodiscard]] float detail_texture_scale() const noexcept { return _m_detail_texture_scale; }

		/**
		 * @return
		 */
		[[nodiscard]] u8 force_occluder() const noexcept { return _m_force_occluder; }

		/**
		 * @return
		 */
		[[nodiscard]] u8 environment_mapping() const noexcept { return _m_environment_mapping; }

		/**
		 * @return
		 */
		[[nodiscard]] float environment_mapping_strength() const noexcept { return _m_environment_mapping_strength; }

		/**
		 * @return
		 */
		[[nodiscard]] u8 wave_mode() const noexcept { return _m_wave_mode; }

		/**
		 * @return
		 */
		[[nodiscard]] u8 wave_speed() const noexcept { return _m_wave_speed; }

		/**
		 * @return
		 */
		[[nodiscard]] float wave_max_amplitude() const noexcept { return _m_wave_max_amplitude; }

		/**
		 * @return
		 */
		[[nodiscard]] float wave_grid_size() const noexcept { return _m_wave_grid_size; }

		/**
		 * @return
		 */
		[[nodiscard]] u8 ignore_sun() const noexcept { return _m_ignore_sun; }

		/**
		 * @return
		 */
		[[nodiscard]] u8 alpha_func() const noexcept { return _m_alpha_func; }

		/**
		 * @return
		 */
		[[nodiscard]] glm::vec2 default_mapping() const noexcept { return _m_default_mapping; }

	private:
		std::string _m_name;
		material_group _m_group;
		::color _m_color {0, 0, 0, 0};
		float _m_smooth_angle {0.0f};
		std::string _m_texture;
		glm::vec2 _m_texture_scale;
		float _m_texture_anim_fps {0.0f};
		u8 _m_texture_anim_map_mode {0};
		glm::vec2 _m_texture_anim_map_dir;
		bool _m_disable_collision {false};
		bool _m_disable_lightmap {false};
		u8 _m_dont_collapse {0};
		std::string _m_detail_object;
		float _m_detail_texture_scale {0.0f};
		u8 _m_force_occluder {0};
		u8 _m_environment_mapping {0};
		float _m_environment_mapping_strength {0.0f};
		u8 _m_wave_mode {0};
		u8 _m_wave_speed {0};
		float _m_wave_max_amplitude {0.0f};
		float _m_wave_grid_size {0.0f};
		u8 _m_ignore_sun {0};
		u8 _m_alpha_func {0};
		glm::vec2 _m_default_mapping {};
	};

}// namespace phoenix