// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/math.hh>
#include <phoenix/mesh.hh>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <tuple>
#include <vector>

namespace phoenix {
	/// \brief Represents an animation sample.
	struct animation_sample {
		glm::vec3 position;
		glm::quat rotation;
	};

	/// \brief Types of animation events.
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

	/// \brief Represents an animation event.
	struct animation_event {
		static constexpr const auto vmax = 4;

		animation_event_type type;
		std::uint32_t no;
		std::string tag;
		std::string content[vmax];
		float values[vmax];
		float probability; // ?
	};

	/// \brief Represents a model animation.
	class animation {
	public:
		/// \brief Parses an animation from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed animation.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static animation parse(buffer& in);

		/// \brief Parses an animation from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed animation.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static animation parse(buffer&& in) {
			return parse(in);
		}

		/// \return The name of the animation
		[[nodiscard]] inline const std::string& name() const noexcept {
			return _m_name;
		}

		/// \return The next animation in queue.
		[[nodiscard]] inline const std::string& next() const noexcept {
			return _m_next;
		}

		/// \return The layer this animation is played in.
		[[nodiscard]] inline std::uint32_t layer() const noexcept {
			return _m_layer;
		}

		/// \return The number of frames of this animation.
		[[nodiscard]] inline std::uint32_t frames() const noexcept {
			return _m_frame_count;
		}

		/// \return  The number of frames of this animation to play per second.
		[[nodiscard]] inline float frames_per_second() const noexcept {
			return _m_fps;
		}

		/// \return Unknown.
		[[nodiscard]] inline float frames_per_second_alt() const noexcept {
			return _m_fps_source;
		}

		/// \return The bounding box of the animation.
		[[nodiscard]] inline bounding_box bbox() const noexcept {
			return _m_bbox;
		}

		/// \return A checksum for this animation (inner workings unknown).
		[[nodiscard]] inline std::uint32_t checksum() const noexcept {
			return _m_checksum;
		}

		/// \return The list of animation samples of this animation.
		[[nodiscard]] inline const std::vector<animation_sample>& samples() const noexcept {
			return _m_samples;
		}

		/// \return The list of animation events of this animation.
		[[nodiscard]] inline const std::vector<animation_event>& events() const noexcept {
			return _m_events;
		}

		/// \return A list of model hierarchy node indices.
		[[nodiscard]] inline const std::vector<std::uint32_t>& node_indices() const noexcept {
			return _m_node_indices;
		}

		/// \return The original path of the animation script this animation was generated from.
		[[nodiscard]] inline const std::string& source_path() const noexcept {
			return _m_source_path;
		}

		/// \return The original model script snippet this animation was generated from.
		[[nodiscard]] inline const std::string& source_script() const noexcept {
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
