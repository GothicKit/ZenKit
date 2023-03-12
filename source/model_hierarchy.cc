// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/model_hierarchy.hh>

namespace phoenix {
	enum class hierarchy_chunk { unknown, hierarchy = 0xD100, stats = 0xD110, end = 0xD120 };

	model_hierarchy model_hierarchy::parse(buffer& in) {
		model_hierarchy hierarchy {};

		hierarchy_chunk type = hierarchy_chunk::unknown;
		bool end_hierarchy = false;

		do {
			type = static_cast<hierarchy_chunk>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case hierarchy_chunk::hierarchy: {
				(void) /* version = */ chunk.get_uint();
				auto node_count = chunk.get_ushort();

				for (int32_t i = 0; i < node_count; ++i) {
					auto& node = hierarchy.nodes.emplace_back();
					node.name = chunk.get_line(false);
					node.parent_index = chunk.get_short();
					node.transform = chunk.get_mat4x4();
				}

				hierarchy.bbox = bounding_box::parse(chunk);
				hierarchy.collision_bbox = bounding_box::parse(chunk);
				hierarchy.root_translation = chunk.get_vec3();
				hierarchy.checksum = chunk.get_uint();
				break;
			}
			case hierarchy_chunk::stats:
				// maybe a date?
				chunk.skip(16);
				(void) /* path? = */ chunk.get_line(false);
				break;
			case hierarchy_chunk::end:
				end_hierarchy = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("model_hierarchy: ",
				        chunk.remaining(),
				        " bytes remaining in section ",
				        std::hex,
				        std::uint16_t(type));
			}
		} while (!end_hierarchy);

		return hierarchy;
	}
} // namespace phoenix
