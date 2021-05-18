// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/world/bsp_tree.hh>
#include <phoenix/world/vob_tree.hh>
#include <phoenix/world/world_mesh.hh>

#include <vector>

namespace phoenix {

	/**
	 * @brief Represents a world (.ZEN).
	 *
	 * Parses ZenGin world files. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zenParser.cpp and the
	 * original version by Andre Taulien was also referenced:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zenParser.cpp
	 *
	 * Thanks to the original author, Andre Taulien as well as Try for additional work on their
	 * ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class world {
	public:
		/**
		 * @brief Reads a world from the given reader.
		 * @param in The reader to read from.
		 * @return The world read.
		 */
		[[nodiscard]] static world read(reader& in, game_version version);

		/**
		 * @return All root VOBs of the world.
		 */
		[[nodiscard]] inline const std::vector<vob_tree>& vobs() const noexcept { return _m_root_vobs; }

		/**
		 * @return The world's mesh.
		 */
		[[nodiscard]] inline const world_mesh& mesh() const noexcept { return _m_mesh; }

		/**
		 * @return The world's BSP-tree.
		 * @see https://en.wikipedia.org/wiki/Binary_space_partitioning
		 */
		[[nodiscard]] inline const bsp_tree& tree() const noexcept { return _m_tree; }

	private:
		std::vector<vob_tree> _m_root_vobs;
		world_mesh _m_mesh;
		bsp_tree _m_tree;
	};

}// namespace phoenix