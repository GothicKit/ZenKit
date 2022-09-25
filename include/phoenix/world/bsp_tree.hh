// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/math.hh>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace phoenix {
	enum class bsp_tree_mode : std::uint32_t {
		indoor = 0,
		outdoor = 1,
	};

	/// \brief Represents a BSP tree node.
	struct bsp_node {
		glm::vec4 plane;
		bounding_box bbox;
		std::uint32_t polygon_index;
		std::uint32_t polygon_count;

		std::int32_t front_index {-1};
		std::int32_t back_index {-1};
		std::int32_t parent_index {-1};

		inline bool is_leaf() const noexcept {
			return front_index == -1 && back_index == -1;
		}
	};

	/// \brief Represents a BSP sector.
	struct bsp_sector {
		std::string name;
		std::vector<std::uint32_t> node_indices;
		std::vector<std::uint32_t> portal_polygon_indices;
	};

	/// \brief Represents a binary space partitioning tree as implemented in the ZenGin.
	///
	/// [Binary space partitioning](https://en.wikipedia.org/wiki/Binary_space_partitioning) is used for rapidly
	/// drawing three-dimensional scenes and performing ray-casts.
	class bsp_tree {
	public:
		/// \brief Parses a BSP tree from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).
		///
		/// \param[in,out] buf The buffer to read from.
		/// \param version The version identifier of the tree in the buffer.
		/// \return The parsed BSP tree.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static bsp_tree parse(buffer& in, std::uint32_t version);

		/// \brief Parses a BSP tree from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \param version The version identifier of the tree in the buffer.
		/// \return The parsed BSP tree.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static bsp_tree parse(buffer&& in, std::uint32_t version) {
			return parse(in, version);
		}

	public:
		/// \brief The mode of the tree (either indoor or outdoor).
		bsp_tree_mode mode;

		/// \brief A list of polygon indices.
		std::vector<std::uint32_t> polygon_indices;

		/// \brief All BSP leaf polygon indices.
		std::vector<std::uint32_t> leaf_polygons;

		/// \brief All BSP light points.
		std::vector<glm::vec3> light_points;

		/// \brief All BSP sectors.
		std::vector<bsp_sector> sectors;

		/// \brief Polygon indices of portals.
		std::vector<std::uint32_t> portal_polygon_indices;

		/// \brief All BSP nodes associated with the tree.
		std::vector<bsp_node> nodes;

		/// \brief All BSP leaf node indices.
		std::vector<std::uint64_t> leaf_node_indices;
	};
} // namespace phoenix
