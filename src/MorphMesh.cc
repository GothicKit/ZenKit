// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/morph_mesh.hh>

namespace phoenix {
	enum class morph_mesh_chunk {
		unknown,
		sources = 0xE010,
		header = 0xE020,
		animations = 0xE030,
		proto = 0xB100,
		morph = 0xB1FF
	};

	morph_mesh morph_mesh::parse(buffer& in) {
		morph_mesh msh {};
		morph_mesh_chunk type = morph_mesh_chunk::unknown;

		do {
			type = static_cast<morph_mesh_chunk>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case morph_mesh_chunk::sources: {
				auto count = chunk.get_ushort();
				msh.sources.resize(count);

				for (int32_t i = 0; i < count; ++i) {
					msh.sources[i].file_date = date::parse(chunk);
					msh.sources[i].file_name = chunk.get_line();
				}

				break;
			}
			case morph_mesh_chunk::header:
				/* version = */ (void) chunk.get_uint();
				msh.name = chunk.get_line();
				break;
			case morph_mesh_chunk::proto:
				msh.mesh = proto_mesh::parse_from_section(chunk);
				msh.morph_positions.resize(msh.mesh.positions.size());
				break;
			case morph_mesh_chunk::morph:
				for (std::uint32_t i = 0; i < msh.morph_positions.size(); ++i) {
					msh.morph_positions[i] = chunk.get_vec3();
				}
				break;
			case morph_mesh_chunk::animations: {
				auto animation_count = chunk.get_ushort();
				msh.animations.reserve(animation_count);

				for (int32_t i = 0; i < animation_count; ++i) {
					auto& anim = msh.animations.emplace_back();
					anim.name = chunk.get_line(false);
					anim.blend_in = chunk.get_float();
					anim.blend_out = chunk.get_float();
					anim.duration = chunk.get_float();
					anim.layer = chunk.get_int();
					anim.speed = chunk.get_float();
					anim.flags = chunk.get();

					auto vertex_count = chunk.get_uint();
					anim.frame_count = chunk.get_uint();

					anim.vertices.resize(vertex_count);
					anim.samples.resize(anim.frame_count * vertex_count);

					for (std::uint32_t j = 0; j < vertex_count; ++j) {
						anim.vertices[j] = chunk.get_uint();
					}

					for (std::uint32_t j = 0; j < vertex_count * anim.frame_count; ++j) {
						anim.samples[j] = chunk.get_vec3();
					}
				}
				break;
			}
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("morph_mesh: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (in.remaining() != 0);

		return msh;
	}
} // namespace phoenix
