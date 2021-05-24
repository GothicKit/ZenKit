// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <vector>

namespace phoenix {
	enum class bsp_tree_mode : u32 {
		indoor = 0,
		outdoor = 1,
	};

	struct bsp_node {
		glm::vec4 plane;
		glm::vec3 bbox[2];
		u32 polygon_index;
		u32 polygon_count;

		s32 front_index {-1};
		s32 back_index {-1};
		s32 parent_index {-1};

		inline bool is_leaf() const noexcept { return front_index == -1 && back_index == -1; }
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
		[[nodiscard]] static bsp_tree parse(reader& in);

		/**
		 * @return The mode of the tree (either indoor or outdoor).
		 */
		[[nodiscard]] bsp_tree_mode mode() const noexcept { return _m_mode; }

		/**
		 * @return A list of polygon indices.
		 */
		[[nodiscard]] const std::vector<u32>& polygon_indices() const noexcept { return _m_polygon_indices; }

		/**
		 * @return All BSP nodes associated with the tree.
		 */
		[[nodiscard]] const std::vector<bsp_node>& nodes() const noexcept { return _m_nodes; }

		/**
		 * @return All BSP leaf node indices.
		 */
		[[nodiscard]] const std::vector<u32>& leaf_node_indices() const noexcept { return _m_leaf_node_indices; }

	private:
		bsp_tree_mode _m_mode;
		std::vector<u32> _m_polygon_indices;

		std::vector<bsp_node> _m_nodes;
		std::vector<u32> _m_leaf_node_indices;
	};
}// namespace phoenix