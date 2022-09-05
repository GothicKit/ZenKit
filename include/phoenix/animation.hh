// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/math/bbox.hh>
#include <phoenix/mesh.hh>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <tuple>
#include <vector>

namespace phoenix {
	/**
	 * @brief Represents an animation sample.
	 */
	struct animation_sample {
		glm::vec3 position;
		glm::quat rotation;
	};

	/**
	 * @brief Types of animation events.
	 */
	enum class animation_event_type : std::uint32_t {
		tag = 0,
		sound = 1,
		sound_ground = 2,
		animation_batch = 3,
		swap_mesh = 4,
		heading = 5,
		pfx = 6,
		pfx_ground = 7,
		pfx_stop = 8,
		set_mesh = 9,
		start_animation = 10,
		tremor = 11,
	};

	/**
	 * @brief Represents an animation event.
	 */
	struct animation_event {
		static constexpr const auto vmax = 4;

		animation_event_type type;
		std::uint32_t no;
		std::string tag;
		std::string content[vmax];
		float values[vmax];
		float probability; // ?
	};

	/**
	 * @brief Represents an animation (.MAN).
	 *
	 * Parses ZenGin animation files. The reference implementation can be found on GitHub:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCModelAni.cpp and an
	 * adapted version used by OpenGothic was also referenced:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zCModelAni.cpp
	 *
	 * Thanks to the original authors, Andre Taulien and Alexander Stillich as well as Try for additional work on their
	 * ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class animation {
	public:
		/**
		 * @brief Parses the animation from the given reader.
		 * @param in The reader to read from.
		 * @return The animation pared.
		 */
		[[nodiscard]] static animation parse(buffer& in);

		/**
		 * @brief Parses the animation from the given reader.
		 * @param in The reader to read from.
		 * @return The animation pared.
		 */
		[[nodiscard]] inline static animation parse(buffer&& in) {
			return parse(in);
		}

		/**
		 * @return The name of the animation
		 */
		[[nodiscard]] inline const std::string& name() const noexcept {
			return _m_name;
		}

		[[nodiscard]] inline const std::string& next() const noexcept {
			return _m_next;
		}

		[[nodiscard]] inline std::uint32_t layer() const noexcept {
			return _m_layer;
		}

		[[nodiscard]] inline std::uint32_t frames() const noexcept {
			return _m_frame_count;
		}

		[[nodiscard]] inline float frames_per_second() const noexcept {
			return _m_fps;
		}

		[[nodiscard]] inline float frames_per_second_alt() const noexcept {
			return _m_fps_source;
		}

		/**
		 * @return The bounding box of the animation (mesh) as a (min, max) tuple.
		 */
		[[nodiscard]] inline bounding_box bbox() const noexcept {
			return _m_bbox;
		}

		[[nodiscard]] inline std::uint32_t checksum() const noexcept {
			return _m_checksum;
		}

		[[nodiscard]] inline const std::vector<animation_sample>& samples() const noexcept {
			return _m_samples;
		}

		[[nodiscard]] inline const std::vector<animation_event>& events() const noexcept {
			return _m_events;
		}

		[[nodiscard]] inline const std::vector<std::uint32_t>& node_indices() const noexcept {
			return _m_node_indices;
		}

		[[nodiscard]] inline const std::string source_path() const noexcept {
			return _m_source_path;
		}

		[[nodiscard]] inline const std::string source_script() const noexcept {
			return _m_mds_source;
		}

	protected:
		animation() = default;

	private:
		std::string _m_name;
		std::string _m_next;
		std::uint32_t _m_layer {};
		std::uint32_t _m_frame_count {};
		std::uint32_t _m_node_count {};
		float _m_fps {};
		float _m_fps_source {};
		float _m_sample_position_range_min {};
		float _m_sample_position_scalar {};
		bounding_box _m_bbox {};
		std::uint32_t _m_checksum {};

		date _m_source_file_date {};
		std::string _m_source_path;
		std::string _m_mds_source;

		std::vector<animation_sample> _m_samples;
		std::vector<animation_event> _m_events;
		std::vector<std::uint32_t> _m_node_indices;
	};

} // namespace phoenix
