// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/model_mesh.hh>

#include <fmt/format.h>
#include <phoenix/mesh.hh>

namespace phoenix {
	enum class model_mesh_chunk {
		unknown,
		header = 0xD000,
		source = 0xD010,
		nodes = 0xD020,
		softskins = 0xD030,
		end = 0xD120
	};

	model_mesh model_mesh::parse(buffer& in) {
		model_mesh msh;

		model_mesh_chunk chunk = model_mesh_chunk::unknown;
		std::uint32_t end = 0;
		bool end_mesh = false;

		do {
			chunk = static_cast<model_mesh_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case model_mesh_chunk::header:
				(void) /* version = */ in.get_uint();
				break;
			case model_mesh_chunk::source: {
				// supposedly a date? weird values
				(void) /* date = */ date::parse(in);
				(void) in.get_ushort(); // padding
				(void) /* source file = */ in.get_line(false);
				break;
			}
			case model_mesh_chunk::nodes: {
				auto node_count = in.get_ushort();
				std::vector<std::string> names {};

				for (std::uint32_t i = 0; i < node_count; ++i) {
					names.push_back(in.get_line());
				}

				for (std::uint32_t i = 0; i < node_count; ++i) {
					msh._m_attachments[names[i]] = proto_mesh::parse(in);
				}

				// hacky bypass for warnings
				end = in.position();
				break;
			}
			case model_mesh_chunk::softskins: {
				(void) /* checksum = */ in.get_uint();
				auto count = in.get_ushort();
				msh._m_meshes.reserve(count);

				for (int i = 0; i < count; ++i) {
					msh._m_meshes.push_back(softskin_mesh::parse(in));
				}

				// hacky bypass for warnings
				end = in.position();
				break;
			}
			case model_mesh_chunk::end:
				end_mesh = true;
				break;
			default:
				break;
			}

			if (in.position() != end) {
				fmt::print(stderr,
				           "warning: model mesh: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.position(end);
		} while (!end_mesh && in.position() < in.limit() - 6);

		return msh;
	}
} // namespace phoenix
