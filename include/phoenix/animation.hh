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
			return animation::parse(in);
		}

	public:
		/// \brief The name of the animation
		std::string name {};

		/// \brief The next animation in queue.
		std::string next {};

		/// \brief The layer this animation is played in.
		std::uint32_t layer {};

		/// \brief The number of frames of this animation.
		std::uint32_t frame_count {};
		std::uint32_t node_count {};

		/// \brief The number of frames of this animation to play per second.
		float fps {};

		/// \brief Unknown.
		float fps_source {};

		float sample_position_range_min {};
		float sample_position_scalar {};

		/// \brief The bounding box of the animation.
		bounding_box bbox {};

		/// \brief A checksum for this animation (inner workings unknown).
		std::uint32_t checksum {};

		/// \brief The original path of the animation script this animation was generated from.
		std::string source_path {};

		/// \brief The original model script snippet this animation was generated from.
		std::string source_script {};

		/// \brief The list of animation samples of this animation.
		std::vector<animation_sample> samples {};

		/// \brief The list of animation events of this animation.
		std::vector<animation_event> events {};

		/// \brief A list of model hierarchy node indices.
		std::vector<std::uint32_t> node_indices;
	};
} // namespace phoenix
