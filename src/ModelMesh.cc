// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/model_mesh.hh>

namespace phoenix {
	enum class model_mesh_chunk {
		unknown,
		header = 0xD000,
		source = 0xD010,
		nodes = 0xD020,
		softskins = 0xD030,
		end = 0xD040,
		proto = 0xB100,
	};

	model_mesh model_mesh::parse(buffer& in) {
		model_mesh msh {};
		model_mesh_chunk type = model_mesh_chunk::unknown;
		bool end_mesh = false;

		std::vector<std::string> attachment_names {};

		do {
			type = static_cast<model_mesh_chunk>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case model_mesh_chunk::header:
				(void) /* version = */ chunk.get_uint();
				break;
			case model_mesh_chunk::source: {
				// supposedly a date? weird values
				(void) /* date = */ date::parse(chunk);
				(void) /* source file = */ chunk.get_line(false);
				break;
			}
			case model_mesh_chunk::nodes: {
				auto node_count = chunk.get_ushort();

				for (std::uint32_t i = 0; i < node_count; ++i) {
					attachment_names.push_back(chunk.get_line());
				}
				break;
			}
			case model_mesh_chunk::proto:
				msh.attachments[attachment_names[msh.attachments.size()]] = proto_mesh::parse_from_section(chunk);
				break;
			case model_mesh_chunk::softskins: {
				msh.checksum = chunk.get_uint();
				auto count = chunk.get_ushort();
				msh.meshes.reserve(count);

				// Quirk: the meshes are not embedded within the chunk (as in: the stored length does not contain
				//        the size of these meshes) so they have to be read directly from `in`.
				for (int32_t i = 0; i < count; ++i) {
					msh.meshes.push_back(softskin_mesh::parse(in));
				}
				break;
			}
			case model_mesh_chunk::end:
				end_mesh = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("model_mesh: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (!end_mesh);

		return msh;
	}
} // namespace phoenix
