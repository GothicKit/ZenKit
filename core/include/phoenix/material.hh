// Copyright 2021 Luis Michaelis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
// Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once
#include <phoenix/archive.hh>
#include <glm/vec2.hpp>
#include <string>

namespace phoenix {
	enum class material_group : uint8_t {
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
		[[nodiscard]] uint8_t texture_anim_map_mode() const noexcept { return _m_texture_anim_map_mode; }

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
		[[nodiscard]] uint8_t dont_collapse() const noexcept { return _m_dont_collapse; }

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
		[[nodiscard]] uint8_t force_occluder() const noexcept { return _m_force_occluder; }

		/**
		 * @return
		 */
		[[nodiscard]] uint8_t environment_mapping() const noexcept { return _m_environment_mapping; }

		/**
		 * @return
		 */
		[[nodiscard]] float environment_mapping_strength() const noexcept { return _m_environment_mapping_strength; }

		/**
		 * @return
		 */
		[[nodiscard]] uint8_t wave_mode() const noexcept { return _m_wave_mode; }

		/**
		 * @return
		 */
		[[nodiscard]] uint8_t wave_speed() const noexcept { return _m_wave_speed; }

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
		[[nodiscard]] uint8_t ignore_sun() const noexcept { return _m_ignore_sun; }

		/**
		 * @return
		 */
		[[nodiscard]] uint8_t alpha_func() const noexcept { return _m_alpha_func; }

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
		uint8_t _m_texture_anim_map_mode {0};
		glm::vec2 _m_texture_anim_map_dir;
		bool _m_disable_collision {false};
		bool _m_disable_lightmap {false};
		uint8_t _m_dont_collapse {0};
		std::string _m_detail_object;
		float _m_detail_texture_scale {0.0f};
		uint8_t _m_force_occluder {0};
		uint8_t _m_environment_mapping {0};
		float _m_environment_mapping_strength {0.0f};
		uint8_t _m_wave_mode {0};
		uint8_t _m_wave_speed {0};
		float _m_wave_max_amplitude {0.0f};
		float _m_wave_grid_size {0.0f};
		uint8_t _m_ignore_sun {0};
		uint8_t _m_alpha_func {0};
		glm::vec2 _m_default_mapping {};
	};

}// namespace openzen