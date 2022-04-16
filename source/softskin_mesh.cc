// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/softskin_mesh.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class softmesh_chunk { unknown, mesh = 0xE100, end = 0xE110 };

	softskin_mesh softskin_mesh::parse(buffer& in) {
		softskin_mesh msh;

		softmesh_chunk chunk = softmesh_chunk::unknown;
		std::uint32_t end = 0;
		bool end_mesh = false;

		do {
			chunk = static_cast<softmesh_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case softmesh_chunk::mesh: {
				(void) /* version = */ in.get_uint();
				msh._m_mesh = proto_mesh::parse(in);

				// weights
				{
					auto weight_buffer_size = in.get_uint();
					auto weight_buffer_end = in.position() + weight_buffer_size;

					msh._m_weights.reserve(in.get_uint());

					if (weight_buffer_size / (sizeof(float) * 4 + sizeof(std::uint8_t)) == msh._m_weights.size()) {
						fmt::print(
						    stderr,
						    "warning: softskin mesh: not all data or too much data consumed from weight section\n");
					}

					for (std::uint32_t i = 0; i < msh._m_weights.size(); ++i) {
						auto& weight = msh._m_weights.emplace_back();
						weight.weight = in.get_float();
						weight.position = in.get_vec3();
						weight.node_index = in.get();
					}

					in.position(weight_buffer_end);
				}

				// wedge normals
				msh._m_wedge_normals.reserve(in.get_uint());

				for (std::uint32_t i = 0; i < msh._m_wedge_normals.size(); ++i) {
					auto& normal = msh._m_wedge_normals.emplace_back();
					normal.normal = in.get_vec3();
					normal.index = in.get_uint();
				}

				// nodes
				msh._m_nodes.resize(in.get_ushort());

				for (std::uint32_t i = 0; i < msh._m_nodes.size(); ++i) {
					msh._m_nodes[i] = in.get_int();
				}

				for (std::uint32_t i = 0; i < msh._m_nodes.size(); ++i) {
					msh._m_bboxes.push_back(obb::parse(in));
				}

				// hacky bypass for warnings
				end = in.position();
				break;
			}

			case softmesh_chunk::end:
				end_mesh = true;
				break;
			default:
				break;
			}

			if (in.position() != end) {
				fmt::print(stderr,
				           "warning: softskin mesh: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.position(end);
		} while (!end_mesh);

		return msh;
	}
} // namespace phoenix
