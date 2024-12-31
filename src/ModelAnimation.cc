// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/ModelAnimation.hh"
#include "zenkit/Stream.hh"

#include <math.h>

namespace zenkit {
	/// \brief The highest number representable by a single rotation component.
	constexpr float SAMPLE_ROTATION_RANGE = static_cast<float>(1 << 16) - 1.0f;

	/// \brief The scaling factor applied to each rotation component.
	constexpr float SAMPLE_ROTATION_SCALE = (1.0f / SAMPLE_ROTATION_RANGE) * 2.1f;

	/// \brief The number half way to `SAMPLE_ROTATION_RANGE`.
	constexpr std::uint16_t SAMPLE_ROTATION_MID = (1 << 15) - 1;

	enum class AnimationChunkType : std::uint16_t {
		MARKER = 0xa000u,
		SOURCE = 0xa010u,
		HEADER = 0xa020u,
		EVENTS = 0xa030u,
		SAMPLES = 0xa090u,
	};

	/// \brief Reads the position of a single animation sample from the given buffer.
	/// \param r The stream to read from.
	/// \param scale The scaling factor to apply (taken from the animation's header).
	/// \param minimum The value of the smallest position component in the animation (part of its header).
	/// \return A vector containing the parsed position.
	/// \see http://phoenix.gothickit.dev/engine/formats/animation/#sample-positions
	static Vec3 read_sample_position(Read* r, float scale, float minimum) {
		Vec3 v {};
		v.x = static_cast<float>(r->read_ushort()) * scale + minimum;
		v.y = static_cast<float>(r->read_ushort()) * scale + minimum;
		v.z = static_cast<float>(r->read_ushort()) * scale + minimum;
		return v;
	}

	/// \brief Reads the rotation of a single animation sample from the given buffer.
	/// \param r The stream to read from.
	/// \return A quaternion containing the parsed rotation.
	/// \see http://phoenix.gothickit.dev/engine/formats/animation/#sample-rotations
	static Quat read_sample_quaternion(Read* r) {
		Quat v {};
		v.x = (static_cast<float>(r->read_ushort()) - SAMPLE_ROTATION_MID) * SAMPLE_ROTATION_SCALE;
		v.y = (static_cast<float>(r->read_ushort()) - SAMPLE_ROTATION_MID) * SAMPLE_ROTATION_SCALE;
		v.z = (static_cast<float>(r->read_ushort()) - SAMPLE_ROTATION_MID) * SAMPLE_ROTATION_SCALE;

		float len_q = v.x * v.x + v.y * v.y + v.z * v.z;

		if (len_q > 1.0f) {
			float l = 1.0f / sqrtf(len_q);
			v.x *= l;
			v.y *= l;
			v.z *= l;
			v.w = 0;
		} else {
			// We know the quaternion has to be a unit quaternion, so we can calculate the missing value.
			v.w = sqrtf(1.0f - len_q);
		}

		return v;
	}

	bool AnimationSample::operator==(AnimationSample const& other) const noexcept {
		return this->position == other.position && this->rotation == other.rotation;
	}

	void ModelAnimation::load(Read* r) {
		proto::read_chunked<AnimationChunkType>(r, "ModelAnimation", [this](Read* c, AnimationChunkType type) {
			switch (type) {
			case AnimationChunkType::MARKER:
				break;
			case AnimationChunkType::SOURCE:
				// Quirk: This was intended to be a date but the original code uses an uninitialized variable here
				//        so the actual data stored does not make any sense.
				this->source_date.load(c);
				this->source_path = c->read_line(false);
				this->source_script = c->read_line(false);
				break;
			case AnimationChunkType::HEADER:
				(void) /* version = */ c->read_ushort(); // Should be 0xc for G2
				this->name = c->read_line(false);
				this->layer = c->read_uint();
				this->frame_count = c->read_uint();
				this->node_count = c->read_uint();
				this->fps = c->read_float();
				this->fps_source = c->read_float();
				this->sample_position_min = c->read_float();
				this->sample_position_scale = c->read_float();
				this->bbox.load(c);
				this->next = c->read_line(false);
				break;
			case AnimationChunkType::EVENTS:
				this->events.resize(c->read_uint());
				for (auto& event : this->events) {
					event.type = static_cast<AnimationEventType>(c->read_uint());
					event.frame = c->read_uint();
					event.tag = c->read_line(true);

					for (auto& j : event.content) {
						j = c->read_line(true);
					}

					for (float& value : event.values) {
						value = c->read_float();
					}

					event.probability = c->read_float();
				}

				break;
			case AnimationChunkType::SAMPLES:
				this->checksum = c->read_uint();

				this->node_indices.resize(this->node_count);
				for (auto& i : this->node_indices) {
					i = c->read_uint();
				}

				this->samples.resize(this->node_count * this->frame_count);
				for (auto& i : this->samples) {
					i.rotation = read_sample_quaternion(c);
					i.position = read_sample_position(c, this->sample_position_scale, this->sample_position_min);
				}

				break;
			default:
				break;
			}

			return false;
		});
	}
} // namespace zenkit
