// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"

#include <cstdint>
#include <string>
#include <vector>

namespace zenkit {
	class Read;
	class Write;

	enum class BspTreeType : std::uint32_t {
		INDOOR = 0,
		OUTDOOR = 1,

		// Deprecated entries.
		indoor ZKREM("renamed to BspTreeType::INDOOR") = INDOOR,
		outdoor ZKREM("renamed to BspTreeType::OUTDOOR") = OUTDOOR,
	};

	/// \brief Represents a BSP tree node.
	struct BspNode {
		Vec4 plane;
		AxisAlignedBoundingBox bbox;
		std::uint32_t polygon_index;
		std::uint32_t polygon_count;

		std::int32_t front_index {-1};
		std::int32_t back_index {-1};
		std::int32_t parent_index {-1};
		std::uint8_t lod {0};

		[[nodiscard]] ZKAPI bool is_leaf() const noexcept {
			return front_index == -1 && back_index == -1;
		}
	};

	/// \brief Represents a BSP sector.
	struct BspSector {
		std::string name;
		std::vector<std::uint32_t> node_indices;
		std::vector<std::uint32_t> portal_polygon_indices;
	};

	/// \brief Represents a binary space partitioning tree as implemented in the ZenGin.
	///
	/// [Binary space partitioning](https://en.wikipedia.org/wiki/Binary_space_partitioning) is used for rapidly
	/// drawing three-dimensional scenes and performing ray-casts.
	class BspTree {
	public:
		ZKINT void load(Read* r, std::uint32_t version);
		ZKINT void save(Write* w, GameVersion version) const;

		/// \brief The mode of the tree (either indoor or outdoor).
		BspTreeType mode;

		/// \brief A list of polygon indices.
		std::vector<std::uint32_t> polygon_indices;

		/// \brief All BSP leaf polygon indices.
		std::vector<std::uint32_t> leaf_polygons;

		/// \brief All BSP light points.
		std::vector<Vec3> light_points;

		/// \brief All BSP sectors.
		std::vector<BspSector> sectors;

		/// \brief Polygon indices of portals.
		std::vector<std::uint32_t> portal_polygon_indices;

		/// \brief All BSP nodes associated with the tree.
		std::vector<BspNode> nodes;

		/// \brief All BSP leaf node indices.
		std::vector<std::uint64_t> leaf_node_indices;
	};
} // namespace zenkit
