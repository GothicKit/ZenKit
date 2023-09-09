// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/MorphMesh.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	enum class MorphMeshChunkType : std::uint16_t {
		SOURCES = 0xE010,
		HEADER = 0xE020,
		ANIMATIONS = 0xE030,
		PROTO = 0xB100,
		MORPH = 0xB1FF
	};

	MorphMesh MorphMesh::parse(phoenix::buffer& in) {
		MorphMesh msh {};

		auto r = Read::from(&in);
		msh.load(r.get());

		return msh;
	}

	MorphMesh MorphMesh::parse(phoenix::buffer&& buf) {
		return MorphMesh::parse(buf);
	}

	void MorphMesh::load(Read* r) {
		proto::read_chunked<MorphMeshChunkType>(r, "MorphMesh", [this](Read* c, MorphMeshChunkType type) {
			switch (type) {
			case MorphMeshChunkType::SOURCES: {
				auto count = c->read_ushort();
				this->sources.resize(count);

				for (int32_t i = 0; i < count; ++i) {
					this->sources[i].file_date.load(c);
					this->sources[i].file_name = c->read_line(true);
				}

				break;
			}
			case MorphMeshChunkType::HEADER:
				/* version = */ (void) c->read_uint();
				this->name = c->read_line(true);
				break;
			case MorphMeshChunkType::PROTO: {
				this->mesh.load_from_section(c);
				this->morph_positions.resize(this->mesh.positions.size());
				break;
			}
			case MorphMeshChunkType::MORPH:
				for (std::uint32_t i = 0; i < this->morph_positions.size(); ++i) {
					this->morph_positions[i] = c->read_vec3();
				}
				break;
			case MorphMeshChunkType::ANIMATIONS: {
				auto animation_count = c->read_ushort();
				this->animations.reserve(animation_count);

				for (int32_t i = 0; i < animation_count; ++i) {
					auto& anim = this->animations.emplace_back();
					anim.name = c->read_line(false);
					anim.blend_in = c->read_float();
					anim.blend_out = c->read_float();
					anim.duration = c->read_float();
					anim.layer = c->read_int();
					anim.speed = c->read_float();
					anim.flags = c->read_ubyte();

					auto vertex_count = c->read_uint();
					anim.frame_count = c->read_uint();

					anim.vertices.resize(vertex_count);
					anim.samples.resize(anim.frame_count * vertex_count);

					for (std::uint32_t j = 0; j < vertex_count; ++j) {
						anim.vertices[j] = c->read_uint();
					}

					for (std::uint32_t j = 0; j < vertex_count * anim.frame_count; ++j) {
						anim.samples[j] = c->read_vec3();
					}
				}
				break;
			}
			default:
				break;
			}

			return false;
		});
	}
} // namespace zenkit
