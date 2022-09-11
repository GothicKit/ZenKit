// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
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
	                             std::vector<std::uint64_t>& indices,
	                             std::uint32_t version,
	                             std::int32_t parent_index,
	                             bool leaf = false) {
		std::uint64_t self_index = nodes.size();

		auto& node = nodes.emplace_back();
		node.parent_index = parent_index;
		node.bbox = bounding_box::parse(in);
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

			if (version == bsp_tree::version_g1) {
				(void) in.get(); // "lod-flag"
			}

			if ((flags & 0x01) != 0) {
				node.front_index = std::int32_t(nodes.size());
				_parse_bsp_nodes(in, nodes, indices, version, std::int32_t(self_index), (flags & 0x04) != 0);
			}

			if ((flags & 0x02) != 0) {
				node.back_index = std::int32_t(nodes.size());
				_parse_bsp_nodes(in, nodes, indices, version, std::int32_t(self_index), (flags & 0x08) != 0);
			}
		}
	};

	bsp_tree bsp_tree::parse(buffer& in, std::uint32_t version) {
		bsp_tree bsp;
		bsp_chunk type = bsp_chunk::unknown;
		bool finished = false;

		do {
			type = static_cast<bsp_chunk>(in.get_ushort());

			auto length = in.get_uint();
			auto chunk = in.extract(length);

			switch (type) {
			case bsp_chunk::header:
				(void) chunk.get_ushort();
				bsp._m_mode = static_cast<bsp_tree_mode>(chunk.get_uint());
				break;
			case bsp_chunk::polygons:
				bsp._m_polygon_indices.resize(chunk.get_uint());

				for (unsigned int& index : bsp._m_polygon_indices) {
					index = chunk.get_uint();
				}
				break;
			case bsp_chunk::tree: {
				uint32_t node_count = chunk.get_uint();
				uint32_t leaf_count = chunk.get_uint();

				bsp._m_nodes.reserve(node_count);
				bsp._m_leaf_node_indices.reserve(leaf_count);

				_parse_bsp_nodes(chunk, bsp._m_nodes, bsp._m_leaf_node_indices, version, -1);

				// A set should be the preferred datastructure for this. For some reason though, std::set maxes out
				// at 255 values :|
				for (auto idx : bsp._m_leaf_node_indices) {
					auto& node = bsp._m_nodes[idx];

					for (unsigned i = 0; i < node.polygon_count; ++i) {
						bsp._m_leaf_polygons.push_back(bsp._m_polygon_indices[node.polygon_index + i]);
					}
				}

				std::sort(bsp._m_leaf_polygons.begin(), bsp._m_leaf_polygons.end());
				bsp._m_leaf_polygons.erase(std::unique(bsp._m_leaf_polygons.begin(), bsp._m_leaf_polygons.end()),
				                           bsp._m_leaf_polygons.end());

				assert(node_count == bsp._m_nodes.size());
				assert(leaf_count == bsp._m_leaf_node_indices.size());
				break;
			}
			case bsp_chunk::light: {
				bsp._m_light_points.resize(bsp._m_leaf_node_indices.size());

				for (std::uint32_t i = 0; i < bsp._m_light_points.size(); ++i) {
					bsp._m_light_points[i] = chunk.get_vec3();
				}
				break;
			}
			case bsp_chunk::outdoors: {
				auto sector_count = chunk.get_uint();
				bsp._m_sectors.reserve(sector_count);

				for (std::uint32_t i = 0; i < sector_count; ++i) {
					auto& sector = bsp._m_sectors.emplace_back();

					sector.name = chunk.get_line(false);

					auto node_count = chunk.get_uint();
					auto polygon_count = chunk.get_uint();

					sector.node_indices.resize(node_count);
					sector.portal_polygon_indices.resize(polygon_count);

					for (std::uint32_t j = 0; j < node_count; ++j) {
						sector.node_indices[j] = chunk.get_uint();
					}

					for (std::uint32_t j = 0; j < polygon_count; ++j) {
						sector.portal_polygon_indices[j] = chunk.get_uint();
					}
				}

				auto portal_count = chunk.get_uint();
				bsp._m_portal_polygon_indices.resize(portal_count);

				for (std::uint32_t i = 0; i < portal_count; ++i) {
					bsp._m_portal_polygon_indices[i] = chunk.get_uint();
				}
				break;
			}
			case bsp_chunk::end:
				(void) chunk.get();
				finished = true;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("bsp_tree: {} bytes remaining in section 0x{:4X}", chunk.remaining(), std::uint16_t(type));
			}
		} while (!finished);

		return bsp;
	}
} // namespace phoenix
