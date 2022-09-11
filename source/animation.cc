// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/animation.hh>

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
	static glm::quat read_sample_quaternion(buffer& in) {
		glm::quat v {};
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

	animation animation::parse(buffer& buf) {
		animation anim {};
		animation_chunk type = animation_chunk::unknown;

		do {
			type = static_cast<animation_chunk>(buf.get_ushort());
			auto chunk = buf.extract(buf.get_uint());

			switch (type) {
			case animation_chunk::header:
				(void) /* version = */ chunk.get_ushort();
				anim._m_name = chunk.get_line(false);
				anim._m_layer = chunk.get_uint();
				anim._m_frame_count = chunk.get_uint();
				anim._m_node_count = chunk.get_uint();
				anim._m_fps = chunk.get_float();
				anim._m_fps_source = chunk.get_float();
				anim._m_sample_position_range_min = chunk.get_float();
				anim._m_sample_position_scalar = chunk.get_float();
				anim._m_bbox = bounding_box::parse(chunk);
				anim._m_next = chunk.get_line(false);
				break;
			case animation_chunk::events:
				anim._m_events.reserve(chunk.get_uint());

				for (std::uint32_t i = 0; i < anim._m_events.size(); ++i) {
					auto& event = anim._m_events.emplace_back();
					event.type = static_cast<animation_event_type>(chunk.get_uint());
					event.no = chunk.get_uint();
					event.tag = chunk.get_line();

					for (auto& j : event.content) {
						j = chunk.get_line();
					}

					for (float& value : event.values) {
						value = chunk.get_float();
					}

					event.probability = chunk.get_float();
				}

				break;
			case animation_chunk::data:
				anim._m_checksum = chunk.get_uint();
				anim._m_node_indices.resize(anim._m_node_count);

				for (std::uint32_t i = 0; i < anim._m_node_count; ++i) {
					anim._m_node_indices[i] = chunk.get_uint();
				}

				anim._m_samples.resize(anim._m_node_count * anim._m_frame_count);

				for (std::size_t i = 0; i < anim._m_samples.size(); ++i) {
					anim._m_samples[i].rotation = read_sample_quaternion(chunk);
					anim._m_samples[i].position =
					    read_sample_position(chunk, anim._m_sample_position_scalar, anim._m_sample_position_range_min);
				}

				break;
			case animation_chunk::source:
				anim._m_source_file_date = date::parse(chunk);
				anim._m_source_path = chunk.get_line(false);
				anim._m_mds_source = chunk.get_line(false);
				break;
			case animation_chunk::animation: // the "animation" chunk is always empty
			default:
				break;
			}

			if (chunk.remaining() > 0) {
				PX_LOGW("animation(\"{}\"): {} bytes remaining in section 0x{:4X}",
				        anim.name(),
				        chunk.remaining(),
				        std::uint16_t(type));
			}
		} while (buf.remaining() != 0);

		return anim;
	}
} // namespace phoenix
