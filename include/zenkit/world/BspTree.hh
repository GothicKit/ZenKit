// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <unordered_set>
#include <vector>

namespace zenkit {
	enum class BspTreeType : std::uint32_t {
		INDOOR = 0,
		OUTDOOR = 1,

		// Deprecated entries.
		indoor ZKREM("renamed to BspTreeType::INDOOR") = INDOOR,
		outdoor ZKREM("renamed to BspTreeType::OUTDOOR") = OUTDOOR,
	};

	/// \brief Represents a BSP tree node.
	struct BspNode {
		glm::vec4 plane;
		AxisAlignedBoundingBox bbox;
		std::uint32_t polygon_index;
		std::uint32_t polygon_count;

		std::int32_t front_index {-1};
		std::int32_t back_index {-1};
		std::int32_t parent_index {-1};

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

	public:
		/// \brief The mode of the tree (either indoor or outdoor).
		BspTreeType mode;

		/// \brief A list of polygon indices.
		std::vector<std::uint32_t> polygon_indices;

		/// \brief All BSP leaf polygon indices.
		std::vector<std::uint32_t> leaf_polygons;

		/// \brief All BSP light points.
		std::vector<glm::vec3> light_points;

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
