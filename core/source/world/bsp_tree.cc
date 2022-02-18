// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/world/bsp_tree.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class bsp_chunk : u16 {
		unknown,
		header = 0xC000,
		polygons = 0xC010,
		tree = 0xC040,
		outdoors = 0xC050,
		light = 0xC045,
		end = 0xC0FF
	};

	static void _parse_bsp_nodes(reader& in,
	                             std::vector<bsp_node>& nodes,
	                             std::vector<u32>& indices,
	                             u32 version,
	                             s32 parent_index,
	                             bool leaf = false) {
		u32 self_index = nodes.size();

		auto& node = nodes.emplace_back();
		node.parent_index = parent_index;

		node.bbox[0] = in.read_vec3();
		node.bbox[1] = in.read_vec3();

		node.polygon_index = in.read_u32();
		node.polygon_count = in.read_u32();

		if (leaf) {
			indices.push_back(self_index);
		} else {
			auto flags = in.read_u8();

			node.plane = {};
			node.plane.w = in.read_f32();
			node.plane.x = in.read_f32();
			node.plane.y = in.read_f32();
			node.plane.z = in.read_f32();

			if (version == 0 /*G1*/) {
				in.ignore(1); // "lod-flag"
			}

			if ((flags & 0x01) != 0) {
				node.front_index = nodes.size();
				_parse_bsp_nodes(in, nodes, indices, version, self_index, (flags & 0x04) != 0);
			}

			if ((flags & 0x02) != 0) {
				node.back_index = nodes.size();
				_parse_bsp_nodes(in, nodes, indices, version, self_index, (flags & 0x08) != 0);
			}
		}
	};

	bsp_tree bsp_tree::parse(reader& in) {
		bsp_tree bsp;

		bool finished = false;
		bsp_chunk chunk = bsp_chunk::unknown;
		u32 end = 0;

		do {
			chunk = static_cast<bsp_chunk>(in.read_u16());

			auto length = in.read_u32();
			end = length + in.tell();

			switch (chunk) {
			case bsp_chunk::header:
				(void) in.read_u16();
				bsp._m_mode = static_cast<bsp_tree_mode>(in.read_u32());
				break;
			case bsp_chunk::polygons:
				bsp._m_polygon_indices.resize(in.read_u32());

				for (unsigned int& index : bsp._m_polygon_indices) {
					index = in.read_u32();
				}
				break;
			case bsp_chunk::tree: {
				uint32_t node_count = in.read_u32();
				uint32_t leaf_count = in.read_u32();

				bsp._m_nodes.reserve(node_count);
				bsp._m_leaf_node_indices.reserve(leaf_count);

				_parse_bsp_nodes(in, bsp._m_nodes, bsp._m_leaf_node_indices, 0, -1);

				assert(node_count == bsp._m_nodes.size());
				assert(leaf_count == bsp._m_leaf_node_indices.size());
				break;
			}
			case bsp_chunk::light: {
				bsp._m_light_points.resize(bsp._m_leaf_node_indices.size());

				for (u32 i = 0; i < bsp._m_light_points.size(); ++i) {
					bsp._m_light_points[i] = in.read_vec3();
				}
				break;
			}
			case bsp_chunk::outdoors: {
				auto sector_count = in.read_u32();
				bsp._m_sectors.reserve(sector_count);

				for (u32 i = 0; i < sector_count; ++i) {
					auto& sector = bsp._m_sectors.emplace_back();

					sector.name = in.read_line(false);

					auto node_count = in.read_u32();
					auto polygon_count = in.read_u32();

					sector.node_indices.resize(node_count);
					sector.portal_polygon_indices.resize(polygon_count);

					for (u32 j = 0; j < node_count; ++j) {
						sector.node_indices[j] = in.read_u32();
					}

					for (u32 j = 0; j < polygon_count; ++j) {
						sector.portal_polygon_indices[j] = in.read_u32();
					}
				}

				auto portal_count = in.read_u32();
				bsp._m_portal_polygon_indices.resize(portal_count);

				for (u32 i = 0; i < portal_count; ++i) {
					bsp._m_portal_polygon_indices[i] = in.read_u32();
				}
				break;
			}
			case bsp_chunk::end:
				in.ignore(1);
				finished = true;
				break;
			default:
				break;
			}

			if (in.tell() != end) {
				fmt::print(stderr,
				           "warning: bsp tree: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.seek(end);
		} while (!finished);

		return bsp;
	}
} // namespace phoenix
