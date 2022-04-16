// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/animation.hh>
#include <phoenix/detail/error.hh>

#include <fmt/format.h>

#include <cmath>

namespace phoenix {
	static const float SAMPLE_ROT_BITS = float(1 << 16) - 1.0f;
	static const float SAMPLE_QUAT_SCALAR = (1.0f / SAMPLE_ROT_BITS) * 2.1f;
	static const std::uint16_t SAMPLE_QUAT_MIDDLE = (1 << 15) - 1;

	enum class animation_chunk {
		animation = 0xa000u,
		source = 0xa010u,
		header = 0xa020u,
		events = 0xa030u,
		data = 0xa090u,
		unknown
	};

	/**
	 * Adapted from ZenLib
	 * (https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCModelAni.cpp#L43).
	 * Thanks to Andre Taulien and Alexander Stillich!
	 */
	static glm::vec3 read_sample_position(buffer& in, float sample_pos_s, float sample_pos_range_min) {
		glm::vec3 v {};
		v.x = (float) in.get_ushort() * sample_pos_s + sample_pos_range_min;
		v.y = (float) in.get_ushort() * sample_pos_s + sample_pos_range_min;
		v.z = (float) in.get_ushort() * sample_pos_s + sample_pos_range_min;
		return v;
	}

	/**
	 * Adapted from ZenLib
	 * (https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCModelAni.cpp#L50).
	 * Thanks to Andre Taulien and Alexander Stillich!
	 */
	static glm::vec4 read_sample_quaternion(buffer& in) {
		glm::vec4 v {};
		v.x = ((float) in.get_ushort() - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALAR;
		v.y = ((float) in.get_ushort() - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALAR;
		v.z = ((float) in.get_ushort() - SAMPLE_QUAT_MIDDLE) * SAMPLE_QUAT_SCALAR;

		float len_q = v.x * v.x + v.y * v.y + v.z * v.z;

		if (len_q > 1.0f) {
			float l = 1.0f / sqrtf(len_q);
			v.x *= l;
			v.y *= l;
			v.z *= l;
			v.w = 0;
		} else {
			v.w = sqrtf(1.0f - len_q);
		}

		return v;
	}

	animation animation::parse(buffer& in) {
		animation anim {};

		animation_chunk chunk = animation_chunk::unknown;
		std::uint32_t end = 0;

		do {
			chunk = static_cast<animation_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case animation_chunk::header:
				(void) /* version = */ in.get_ushort();
				anim._m_name = in.get_line();
				anim._m_layer = in.get_uint();
				anim._m_frame_count = in.get_uint();
				anim._m_node_count = in.get_uint();
				anim._m_fps = in.get_float();
				anim._m_fps_source = in.get_float();
				anim._m_sample_position_range_min = in.get_float();
				anim._m_sample_position_scalar = in.get_float();
				anim._m_bbox[0] = in.get_vec3();
				anim._m_bbox[1] = in.get_vec3();
				anim._m_next = in.get_line();
				break;
			case animation_chunk::events:
				anim._m_events.reserve(in.get_uint());

				for (std::uint32_t i = 0; i < anim._m_events.size(); ++i) {
					auto& event = anim._m_events.emplace_back();
					event.type = static_cast<animation_event_type>(in.get_uint());
					event.no = in.get_uint();
					event.tag = in.get_line();

					for (auto& j : event.content) {
						j = in.get_line();
					}

					for (float& value : event.values) {
						value = in.get_float();
					}

					event.probability = in.get_float();
				}

				break;
			case animation_chunk::data:
				anim._m_checksum = in.get_uint();
				anim._m_node_indices.resize(anim._m_node_count);

				for (std::uint32_t i = 0; i < anim._m_node_count; ++i) {
					anim._m_node_indices[i] = in.get_uint();
				}

				anim._m_samples.resize(anim._m_node_count * anim._m_frame_count);

				for (std::size_t i = 0; i < anim._m_samples.size(); ++i) {
					// first rotation, then position. values seem more reasonable the other way around tho.
					// TODO: investigate in opengothic!
					anim._m_samples[i].rotation = read_sample_quaternion(in);
					anim._m_samples[i].position =
					    read_sample_position(in, anim._m_sample_position_scalar, anim._m_sample_position_range_min);
				}

				break;
			case animation_chunk::source:
				// this is actually a date though it is 4 bytes aligned
				anim._m_source_file_date = {
				    in.get_uint(),
				    in.get_ushort(),
				    in.get_ushort(),
				    in.get_ushort(),
				    in.get_ushort(),
				    in.get_ushort(),
				};

				// discard alignment bytes
				(void) in.get_ushort();

				anim._m_source_path = in.get_line(false);
				anim._m_mds_source = in.get_line(false);
				break;
			case animation_chunk::animation:
				break;
			default:
				break;
			}

			if (in.position() != end) {
				fmt::print(stderr,
				           "warning: animation: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.position(end);
		} while (in.position() < in.limit());

		return anim;
	}
} // namespace phoenix
