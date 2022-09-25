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
				anim.name = chunk.get_line(false);
				anim.layer = chunk.get_uint();
				anim.frame_count = chunk.get_uint();
				anim.node_count = chunk.get_uint();
				anim.fps = chunk.get_float();
				anim.fps_source = chunk.get_float();
				anim.sample_position_range_min = chunk.get_float();
				anim.sample_position_scalar = chunk.get_float();
				anim.bbox = bounding_box::parse(chunk);
				anim.next = chunk.get_line(false);
				break;
			case animation_chunk::events:
				anim.events.reserve(chunk.get_uint());

				for (std::uint32_t i = 0; i < anim.events.size(); ++i) {
					auto& event = anim.events.emplace_back();
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
				anim.checksum = chunk.get_uint();
				anim.node_indices.resize(anim.node_count);

				for (std::uint32_t i = 0; i < anim.node_count; ++i) {
					anim.node_indices[i] = chunk.get_uint();
				}

				anim.samples.resize(anim.node_count * anim.frame_count);

				for (std::size_t i = 0; i < anim.samples.size(); ++i) {
					anim.samples[i].rotation = read_sample_quaternion(chunk);
					anim.samples[i].position =
					    read_sample_position(chunk, anim.sample_position_scalar, anim.sample_position_range_min);
				}

				break;
			case animation_chunk::source:
				anim.source_file_date = date::parse(chunk);
				anim.source_path = chunk.get_line(false);
				anim.source_script = chunk.get_line(false);
				break;
			case animation_chunk::animation: // the "animation" chunk is always empty
			default:
				break;
			}

			if (chunk.remaining() > 0) {
				PX_LOGW("animation(\"{}\"): {} bytes remaining in section 0x{:4X}",
				        anim.name,
				        chunk.remaining(),
				        std::uint16_t(type));
			}
		} while (buf.remaining() != 0);

		return anim;
	}
} // namespace phoenix
