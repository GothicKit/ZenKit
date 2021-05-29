// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/softskin_mesh.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class softmesh_chunk {
		unknown,
		mesh = 0xE100,
		end = 0xE110
	};

	softskin_mesh softskin_mesh::parse(reader& in) {
		softskin_mesh msh;

		softmesh_chunk chunk = softmesh_chunk::unknown;
		u32 end = 0;
		bool end_mesh = false;

		do {
			chunk = static_cast<softmesh_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
				case softmesh_chunk::mesh: {
					(void) /* version = */ in.read_u32();
					msh._m_mesh = proto_mesh::parse(in);

					// weights
					{
						auto weight_buffer_size = in.read_u32();
						auto weight_buffer_end = in.tell() + weight_buffer_size;

						msh._m_weights.reserve(in.read_u32());

						if (weight_buffer_size / (sizeof(float) * 4 + sizeof(u8)) == msh._m_weights.size()) {
							fmt::print(stderr, "warning: softskin mesh: not all data or too much data consumed from weight section\n");
						}

						for (u32 i = 0; i < msh._m_weights.size(); ++i) {
							auto& weight = msh._m_weights.emplace_back();
							weight.weight = in.read_f32();
							weight.position = in.read_vec3();
							weight.node_index = in.read_u8();
						}

						in.seek(weight_buffer_end);
					}

					// wedge normals
					msh._m_wedge_normals.reserve(in.read_u32());

					for (u32 i = 0; i < msh._m_wedge_normals.size(); ++i) {
						auto& normal = msh._m_wedge_normals.emplace_back();
						normal.normal = in.read_vec3();
						normal.index = in.read_u32();
					}

					// nodes
					msh._m_nodes.resize(in.read_u16());

					for (u32 i = 0; i < msh._m_nodes.size(); ++i) {
						msh._m_nodes[i] = in.read_s32();
					}

					for (u32 i = 0; i < msh._m_nodes.size(); ++i) {
						msh._m_bboxes.push_back(obb::parse(in));
					}

					// hacky bypass for warnings
					end = in.tell();
					break;
				}

				case softmesh_chunk::end:
					end_mesh = true;
					break;
				default:
					break;
			}

			if (in.tell() != end) {
				fmt::print(stderr, "warning: softskin mesh: not all data or too much data consumed from section 0x{:X}\n", chunk);
			}

			in.seek(end);
		} while (!end_mesh);

		return msh;
	}
}// namespace phoenix