// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/world/bsp_tree.hh>

#include <fmt/format.h>

namespace phoenix {
	enum class bsp_chunk : std::uint16_t {
		unknown,
		header = 0xC000,
		polygons = 0xC010,
		tree = 0xC040,
		outdoors = 0xC050,
		light = 0xC045,
		end = 0xC0FF
	};

	static void _parse_bsp_nodes(buffer& in,
	                             std::vector<bsp_node>& nodes,
	                             std::vector<std::uint32_t>& indices,
	                             std::uint32_t version,
	                             std::int32_t parent_index,
	                             bool leaf = false) {
		std::uint32_t self_index = nodes.size();

		auto& node = nodes.emplace_back();
		node.parent_index = parent_index;

		node.bbox[0] = in.get_vec3();
		node.bbox[1] = in.get_vec3();

		node.polygon_index = in.get_uint();
		node.polygon_count = in.get_uint();

		if (leaf) {
			indices.push_back(self_index);
		} else {
			auto flags = in.get();

			node.plane = {};
			node.plane.w = in.get_float();
			node.plane.x = in.get_float();
			node.plane.y = in.get_float();
			node.plane.z = in.get_float();

			if (version == 34144256 /*G1*/) {
				(void) in.get(); // "lod-flag"
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

	bsp_tree bsp_tree::parse(buffer& in, std::uint32_t version) {
		bsp_tree bsp;

		bool finished = false;
		bsp_chunk chunk = bsp_chunk::unknown;
		std::uint32_t end = 0;

		do {
			chunk = static_cast<bsp_chunk>(in.get_ushort());

			auto length = in.get_uint();
			end = length + in.position();

			switch (chunk) {
			case bsp_chunk::header:
				(void) in.get_ushort();
				bsp._m_mode = static_cast<bsp_tree_mode>(in.get_uint());
				break;
			case bsp_chunk::polygons:
				bsp._m_polygon_indices.resize(in.get_uint());

				for (unsigned int& index : bsp._m_polygon_indices) {
					index = in.get_uint();
				}
				break;
			case bsp_chunk::tree: {
				uint32_t node_count = in.get_uint();
				uint32_t leaf_count = in.get_uint();

				bsp._m_nodes.reserve(node_count);
				bsp._m_leaf_node_indices.reserve(leaf_count);

				_parse_bsp_nodes(in, bsp._m_nodes, bsp._m_leaf_node_indices, version, -1);

				assert(node_count == bsp._m_nodes.size());
				assert(leaf_count == bsp._m_leaf_node_indices.size());
				break;
			}
			case bsp_chunk::light: {
				bsp._m_light_points.resize(bsp._m_leaf_node_indices.size());

				for (std::uint32_t i = 0; i < bsp._m_light_points.size(); ++i) {
					bsp._m_light_points[i] = in.get_vec3();
				}
				break;
			}
			case bsp_chunk::outdoors: {
				auto sector_count = in.get_uint();
				bsp._m_sectors.reserve(sector_count);

				for (std::uint32_t i = 0; i < sector_count; ++i) {
					auto& sector = bsp._m_sectors.emplace_back();

					sector.name = in.get_line(false);

					auto node_count = in.get_uint();
					auto polygon_count = in.get_uint();

					sector.node_indices.resize(node_count);
					sector.portal_polygon_indices.resize(polygon_count);

					for (std::uint32_t j = 0; j < node_count; ++j) {
						sector.node_indices[j] = in.get_uint();
					}

					for (std::uint32_t j = 0; j < polygon_count; ++j) {
						sector.portal_polygon_indices[j] = in.get_uint();
					}
				}

				auto portal_count = in.get_uint();
				bsp._m_portal_polygon_indices.resize(portal_count);

				for (std::uint32_t i = 0; i < portal_count; ++i) {
					bsp._m_portal_polygon_indices[i] = in.get_uint();
				}
				break;
			}
			case bsp_chunk::end:
				(void) in.get();
				finished = true;
				break;
			default:
				break;
			}

			if (in.position() != end) {
				fmt::print(stderr,
				           "warning: bsp tree: not all data or too much data consumed from section 0x{:X}\n",
				           chunk);
			}

			in.position(end);
		} while (!finished);

		return bsp;
	}
} // namespace phoenix
