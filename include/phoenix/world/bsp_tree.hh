// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>
#include <phoenix/math/bbox.hh>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace phoenix {
	enum class bsp_tree_mode : std::uint32_t {
		indoor = 0,
		outdoor = 1,
	};

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

	struct bsp_sector {
		std::string name;
		std::vector<std::uint32_t> node_indices;
		std::vector<std::uint32_t> portal_polygon_indices;
	};

	/**
	 * @brief Represents a BSP tree.
	 *
	 * Parses ZenGin BSP trees. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zCBspTree.cpp and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zCBspTree.h
	 *
	 * Thanks to the original author, Andre Taulien as well as Try for additional work on their
	 * ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 * @note This is not currently fully finished.
	 */
	class bsp_tree {
	public:
		/**
		 * @brief Parses a BSP tree from the given reader.
		 * @param in The reader to read from.
		 * @return The tree parsed.
		 */
		[[nodiscard]] static bsp_tree parse(buffer& in, std::uint32_t version);

		/**
		 * @return The mode of the tree (either indoor or outdoor).
		 */
		[[nodiscard]] bsp_tree_mode mode() const noexcept {
			return _m_mode;
		}

		/**
		 * @return A list of polygon indices.
		 */
		[[nodiscard]] const std::vector<std::uint32_t>& polygon_indices() const noexcept {
			return _m_polygon_indices;
		}

		/**
		 * @return All BSP nodes associated with the tree.
		 */
		[[nodiscard]] const std::vector<bsp_node>& nodes() const noexcept {
			return _m_nodes;
		}

		/**
		 * @return All BSP leaf node indices.
		 */
		[[nodiscard]] const std::vector<std::uint64_t>& leaf_node_indices() const noexcept {
			return _m_leaf_node_indices;
		}

		[[nodiscard]] const std::vector<std::uint32_t>& leaf_polygons() const noexcept {
			return _m_leaf_polygons;
		}

		/**
		 * @return All BSP sectors.
		 */
		[[nodiscard]] const std::vector<bsp_sector>& sectors() const noexcept {
			return _m_sectors;
		}

		/**
		 * @return Polygon indices of portals.
		 */
		[[nodiscard]] const std::vector<std::uint32_t>& portal_polygon_indices() const noexcept {
			return _m_portal_polygon_indices;
		}

		[[nodiscard]] const std::vector<glm::vec3>& light_points() const noexcept {
			return _m_light_points;
		}

		static constexpr auto version_g1 = 0x2090000;
		static constexpr auto version_g2 = 0x4090000;
	private:
		bsp_tree_mode _m_mode;
		std::vector<std::uint32_t> _m_polygon_indices;
		std::vector<std::uint32_t> _m_leaf_polygons;
		std::vector<glm::vec3> _m_light_points;

		std::vector<bsp_sector> _m_sectors;
		std::vector<std::uint32_t> _m_portal_polygon_indices;

		std::vector<bsp_node> _m_nodes;
		std::vector<std::uint64_t> _m_leaf_node_indices;
	};
} // namespace phoenix
