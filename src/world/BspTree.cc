// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/world/BspTree.hh"
#include "zenkit/Stream.hh"

#include "../Internal.hh"

namespace zenkit {
	static constexpr auto version_g1 = 0x2090000;
	[[maybe_unused]] static constexpr auto version_g2 = 0x4090000;

	enum class BspChunkType : std::uint16_t {
		HEADER = 0xC000,
		POLYGONS = 0xC010,
		TREE = 0xC040,
		OUTDOORS = 0xC050,
		LIGHT = 0xC045,
		END = 0xC0FF
	};

	static void _parse_bsp_nodes(Read* in,
	                             std::vector<BspNode>& nodes,
	                             std::vector<std::uint64_t>& indices,
	                             std::uint32_t version,
	                             std::int32_t parent_index,
	                             bool leaf = false) {
		std::uint64_t self_index = nodes.size();

		auto& node = nodes.emplace_back();
		node.parent_index = parent_index;
		node.bbox.load(in);
		node.polygon_index = in->read_uint();
		node.polygon_count = in->read_uint();

		if (leaf) {
			indices.push_back(self_index);
		} else {
			auto flags = in->read_ubyte();

			node.plane = {};
			node.plane.w = in->read_float();
			node.plane.x = in->read_float();
			node.plane.y = in->read_float();
			node.plane.z = in->read_float();

			if (version == version_g1) {
				(void) in->read_ubyte(); // "lod-flag"
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
	}

	void BspTree::load(Read* r, std::uint32_t version) {
		proto::read_chunked<BspChunkType>(r, "BspTree", [this, version](Read* c, BspChunkType type) {
			ZKLOGI("BspTree", "Parsing chunk %x", std::uint16_t(type));

			switch (type) {
			case BspChunkType::HEADER:
				(void) c->read_ushort();
				this->mode = static_cast<BspTreeType>(c->read_uint());
				break;
			case BspChunkType::POLYGONS:
				this->polygon_indices.resize(c->read_uint());

				for (uint32_t& index : this->polygon_indices) {
					index = c->read_uint();
				}
				break;
			case BspChunkType::TREE: {
				uint32_t node_count = c->read_uint();
				uint32_t leaf_count = c->read_uint();

				this->nodes.reserve(node_count);
				this->leaf_node_indices.reserve(leaf_count);

				_parse_bsp_nodes(c, this->nodes, this->leaf_node_indices, version, -1);

				for (auto idx : this->leaf_node_indices) {
					auto& node = this->nodes[idx];

					for (uint32_t i = 0; i < node.polygon_count; ++i) {
						this->leaf_polygons.push_back(this->polygon_indices[node.polygon_index + i]);
					}
				}
				std::sort(this->leaf_polygons.begin(), this->leaf_polygons.end());

				assert(node_count == this->nodes.size());
				assert(leaf_count == this->leaf_node_indices.size());
				break;
			}
			case BspChunkType::LIGHT: {
				this->light_points.resize(this->leaf_node_indices.size());
				for (auto& light_point : this->light_points) {
					light_point = c->read_vec3();
				}
				break;
			}
			case BspChunkType::OUTDOORS: {
				auto sector_count = c->read_uint();
				this->sectors.reserve(sector_count);

				for (std::uint32_t i = 0; i < sector_count; ++i) {
					auto& sector = this->sectors.emplace_back();

					sector.name = c->read_line(false);

					auto node_count = c->read_uint();
					auto polygon_count = c->read_uint();

					sector.node_indices.resize(node_count);
					sector.portal_polygon_indices.resize(polygon_count);

					for (std::uint32_t j = 0; j < node_count; ++j) {
						sector.node_indices[j] = c->read_uint();
					}

					for (std::uint32_t j = 0; j < polygon_count; ++j) {
						sector.portal_polygon_indices[j] = c->read_uint();
					}
				}

				auto portal_count = c->read_uint();
				this->portal_polygon_indices.resize(portal_count);

				for (std::uint32_t i = 0; i < portal_count; ++i) {
					this->portal_polygon_indices[i] = c->read_uint();
				}
				break;
			}
			case BspChunkType::END:
				(void) c->read_ubyte();
				return true;
			default:
				break;
			}

			return false;
		});
	}
} // namespace zenkit
