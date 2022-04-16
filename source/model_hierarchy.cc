// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/model_hierarchy.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class hierarchy_chunk { unknown, hierarchy = 0xD100, stats = 0xD110, end = 0xD120 };

	model_hierachy model_hierachy::parse(buffer& in) {
		model_hierachy hierarchy;

		hierarchy_chunk chunk = hierarchy_chunk::unknown;
		std::uint32_t end = 0;
		bool end_hierarchy = false;

		do {
			chunk = static_cast<hierarchy_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case hierarchy_chunk::hierarchy: {
				(void) /* version = */ in.get_uint();
				auto node_count = in.get_ushort();
				hierarchy._m_nodes.resize(node_count);

				for (int i = 0; i < node_count; ++i) {
					auto& node = hierarchy._m_nodes[i];
					node.name = in.get_line(false);
					node.parent_index = in.get_short();
					node.transform = glm::mat4 {in.get_vec4(), in.get_vec4(), in.get_vec4(), in.get_vec4()};
				}

				hierarchy._m_bbox[0] = in.get_vec3();
				hierarchy._m_bbox[1] = in.get_vec3();
				hierarchy._m_collision_bbox[0] = in.get_vec3();
				hierarchy._m_collision_bbox[1] = in.get_vec3();
				hierarchy._m_root_translation = in.get_vec3();

				(void) /* checksum = */ in.get_uint();
				break;
			}
			case hierarchy_chunk::stats:
				// maybe a date?
				in.skip(16);
				(void) /* path? = */ in.get_line(false);
				break;
			case hierarchy_chunk::end:
				end_hierarchy = true;
				break;
			default:
				break;
			}

			if (in.position() != end) {
				fmt::print(stderr,
				           "warning: model hierarchy: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.position(end);
		} while (!end_hierarchy);

		return hierarchy;
	}
} // namespace phoenix
