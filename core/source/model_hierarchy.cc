// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/model_hierarchy.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class hierarchy_chunk {
		unknown,
		hierarchy = 0xD100,
		stats = 0xD110,
		end = 0xD120
	};

	model_hierachy model_hierachy::parse(reader& in) {
		model_hierachy hierarchy;

		hierarchy_chunk chunk = hierarchy_chunk::unknown;
		u32 end = 0;
		bool end_hierarchy = false;

		do {
			chunk = static_cast<hierarchy_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
				case hierarchy_chunk::hierarchy: {
					(void) /* version = */ in.read_u32();
					auto node_count = in.read_u16();
					hierarchy._m_nodes.resize(node_count);

					for (int i = 0; i < node_count; ++i) {
						auto& node = hierarchy._m_nodes[i];
						node.name = in.read_line(false);
						node.parent_index = in.read_s16();
						node.transform = glm::mat4 {in.read_vec4(), in.read_vec4(), in.read_vec4(), in.read_vec4()};
					}

					hierarchy._m_bbox[0] = in.read_vec3();
					hierarchy._m_bbox[1] = in.read_vec3();
					hierarchy._m_collision_bbox[0] = in.read_vec3();
					hierarchy._m_collision_bbox[1] = in.read_vec3();
					hierarchy._m_root_translation = in.read_vec3();

					(void) /* checksum = */ in.read_u32();
					break;
				}
				case hierarchy_chunk::stats:
					// maybe a date?
					in.ignore(16);
					(void) /* path? = */ in.read_line(false);
					break;
				case hierarchy_chunk::end:
					end_hierarchy = true;
					break;
				default:
					break;
			}

			if (in.tell() != end) {
				fmt::print(stderr, "warning: model hierarchy: not all data or too much data consumed from section 0x{:X}\n", chunk);
			}

			in.seek(end);
		} while (!end_hierarchy);

		return hierarchy;
	}
}// namespace phoenix