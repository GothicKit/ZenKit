// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
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

	model_mesh model_mesh::parse(reader& in) {
		model_mesh msh;

		model_mesh_chunk chunk = model_mesh_chunk::unknown;
		u32 end = 0;
		bool end_mesh = false;

		do {
			chunk = static_cast<model_mesh_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
				case model_mesh_chunk::header:
					(void) /* version = */ in.read_u32();
					break;
				case model_mesh_chunk::source: {
					// supposedly a date? weird values
					date file_date = {
							in.read_u32(),
							in.read_u16(),
							in.read_u16(),
							in.read_u16(),
							in.read_u16(),
							in.read_u16(),
					};

					// discard alignment bytes
					in.ignore(2);

					(void) file_date;
					(void) /* source file = */ in.read_line(false);
					break;
				}
				case model_mesh_chunk::nodes: {
					auto node_count = in.read_u16();
					std::vector<std::string> names {};

					for (u32 i = 0; i < node_count; ++i) {
						names.push_back(in.read_line());
					}

					for (u32 i = 0; i < node_count; ++i) {
						msh._m_attachments[names[i]] = proto_mesh::parse(in);
					}

					// hacky bypass for warnings
					end = in.tell();
					break;
				}
				case model_mesh_chunk::softskins: {
					(void) /* checksum = */ in.read_u32();
					auto count = in.read_u16();
					msh._m_meshes.reserve(count);

					for (int i = 0; i < count; ++i) {
						msh._m_meshes.push_back(softskin_mesh::parse(in));
					}

					// hacky bypass for warnings
					end = in.tell();
					break;
				}
				case model_mesh_chunk::end:
					end_mesh = true;
					break;
				default:
					break;
			}

			if (in.tell() != end) {
				fmt::print(stderr, "warning: model mesh: not all data or too much data consumed from section 0x{:X}\n", chunk);
			}

			in.seek(end);
		} while (!end_mesh && in.tell() < in.size() - 6);

		return msh;
	}
}// namespace phoenix
