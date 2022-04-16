// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/morph_mesh.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class morph_mesh_chunk { unknown, sources = 0xE010, header = 0xE020, animations = 0xE030 };

	morph_mesh morph_mesh::parse(buffer& in) {
		morph_mesh msh;

		morph_mesh_chunk chunk = morph_mesh_chunk::unknown;
		std::uint32_t end = 0;

		do {
			chunk = static_cast<morph_mesh_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case morph_mesh_chunk::sources: {
				auto count = in.get_ushort();
				msh._m_sources.resize(count);

				for (int i = 0; i < count; ++i) {
					msh._m_sources[i].file_date = date::parse(in);
					(void) in.get_ushort(); // padding
					msh._m_sources[i].file_name = in.get_line();
				}

				break;
			}
			case morph_mesh_chunk::header:
				/* version = */ (void) in.get_uint();
				msh._m_name = in.get_line();
				msh._m_mesh = proto_mesh::parse(in);
				msh._m_morph_positions.resize(msh._m_mesh.positions().size());

				// quirk: technically this content is saved in the last chunk of the proto_mesh
				for (std::uint32_t i = 0; i < msh._m_morph_positions.size(); ++i) {
					msh._m_morph_positions[i] = in.get_vec3();
				}

				// to fix warnings, we just adjust `end`
				// please don't yell at me, ok?
				end = in.position();
				break;
			case morph_mesh_chunk::animations: {
				auto animation_count = in.get_ushort();
				msh._m_animations.reserve(animation_count);

				for (int i = 0; i < animation_count; ++i) {
					auto& anim = msh._m_animations.emplace_back();
					anim.name = in.get_line(false);
					anim.blend_in = in.get_float();
					anim.blend_out = in.get_float();
					anim.duration = in.get_float();
					anim.layer = in.get_int();
					anim.speed = in.get_float();
					anim.flags = in.get();

					auto vertex_count = in.get_uint();
					anim.frame_count = in.get_uint();

					anim.vertices.resize(vertex_count);
					anim.samples.resize(anim.frame_count * vertex_count);

					for (std::uint32_t j = 0; j < vertex_count; ++j) {
						anim.vertices[j] = in.get_uint();
					}

					for (std::uint32_t j = 0; j < vertex_count * anim.frame_count; ++j) {
						anim.samples[j] = in.get_vec3();
					}
				}
				break;
			}
			default:
				break;
			}

			if (in.position() != end) {
				fmt::print(stderr,
				           "warning: morph mesh: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.position(end);
		} while (in.position() < in.limit());

		return msh;
	}
} // namespace phoenix
