// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/detail/types.hh>

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
		glm::vec4 rotation;
	};

	/**
	 * @brief Types of animation events.
	 */
	enum class animation_event_type : u32 {
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
		u32 no;
		std::string tag;
		std::string content[vmax];
		float values[vmax];
		float probability;// ?
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
		 * @brief Read the animation from the given reader.
		 * @param in The reader to read from.
		 * @return The animation read.
		 */
		static animation read(reader& in);

		/**
		 * @return The name of the animation
		 */
		[[nodiscard]] inline const std::string& name() const noexcept { return _m_name; }

		[[nodiscard]] inline const std::string& next() const noexcept { return _m_next; }

		[[nodiscard]] inline u32 layer() const noexcept { return _m_layer; }

		[[nodiscard]] inline u32 frames() const noexcept { return _m_frame_count; }

		[[nodiscard]] inline float frames_per_second() const noexcept { return _m_fps; }

		[[nodiscard]] inline float frames_per_second_alt() const noexcept { return _m_fps_source; }

		/**
		 * @return The bounding box of the animation (mesh) as a (min, max) tuple.
		 */
		[[nodiscard]] inline std::tuple<glm::vec3, glm::vec3> bbox() const noexcept { return std::make_tuple(_m_bbox[0], _m_bbox[1]); }

		[[nodiscard]] inline u32 checksum() const noexcept { return _m_checksum; }

		[[nodiscard]] inline const std::vector<animation_sample>& samples() const noexcept { return _m_samples; }

		[[nodiscard]] inline const std::vector<animation_event>& events() const noexcept { return _m_events; }

		[[nodiscard]] inline const std::vector<u32>& node_indices() const noexcept { return _m_node_indices; }

	protected:
		animation() = default;

	private:
		std::string _m_name;
		std::string _m_next;
		u32 _m_layer {};
		u32 _m_frame_count {};
		u32 _m_node_count {};
		float _m_fps {};
		float _m_fps_source {};
		float _m_sample_position_range_min {};
		float _m_sample_position_scalar {};
		glm::vec3 _m_bbox[2] {};
		u32 _m_checksum {};

		std::vector<animation_sample> _m_samples;
		std::vector<animation_event> _m_events;
		std::vector<u32> _m_node_indices;
	};

}// namespace phoenix