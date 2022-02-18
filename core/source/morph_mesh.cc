// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/morph_mesh.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class morph_mesh_chunk { unknown, sources = 0xE010, header = 0xE020, animations = 0xE030 };

	morph_mesh morph_mesh::parse(reader& in) {
		morph_mesh msh;

		morph_mesh_chunk chunk = morph_mesh_chunk::unknown;
		u32 end = 0;

		do {
			chunk = static_cast<morph_mesh_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
			case morph_mesh_chunk::sources: {
				auto count = in.read_u16();
				msh._m_sources.resize(count);

				for (int i = 0; i < count; ++i) {
					// this is actually a date though it is 4 bytes aligned
					msh._m_sources[i].file_date = {
					    in.read_u32(),
					    in.read_u16(),
					    in.read_u16(),
					    in.read_u16(),
					    in.read_u16(),
					    in.read_u16(),
					};

					// discard alignment bytes
					in.ignore(2);

					msh._m_sources[i].file_name = in.read_line();
				}

				break;
			}
			case morph_mesh_chunk::header:
				/* version = */ (void) in.read_u32();
				msh._m_name = in.read_line();
				msh._m_mesh = proto_mesh::parse(in);
				msh._m_morph_positions.resize(msh._m_mesh.positions().size());

				// quirk: technically this content is saved in the last chunk of the proto_mesh
				for (u32 i = 0; i < msh._m_morph_positions.size(); ++i) {
					msh._m_morph_positions[i] = in.read_vec3();
				}

				// to fix warnings, we just adjust `end`
				// please don't yell at me, ok?
				end = in.tell();
				break;
			case morph_mesh_chunk::animations: {
				auto animation_count = in.read_u16();
				msh._m_animations.reserve(animation_count);

				for (int i = 0; i < animation_count; ++i) {
					auto& anim = msh._m_animations.emplace_back();
					anim.name = in.read_line(false);
					anim.blend_in = in.read_f32();
					anim.blend_out = in.read_f32();
					anim.duration = in.read_f32();
					anim.layer = in.read_s32();
					anim.speed = in.read_f32();
					anim.flags = in.read_u8();

					auto vertex_count = in.read_u32();
					anim.frame_count = in.read_u32();

					anim.vertices.resize(vertex_count);
					anim.samples.resize(anim.frame_count * vertex_count);

					for (u32 j = 0; j < vertex_count; ++j) {
						anim.vertices[j] = in.read_u32();
					}

					for (u32 j = 0; j < vertex_count * anim.frame_count; ++j) {
						anim.samples[j] = in.read_vec3();
					}
				}
				break;
			}
			default:
				break;
			}

			if (in.tell() != end) {
				fmt::print(stderr,
				           "warning: morph mesh: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.seek(end);
		} while (in.tell() < in.size());

		return msh;
	}
} // namespace phoenix
