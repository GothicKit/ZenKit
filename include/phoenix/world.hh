// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>
#include <phoenix/mesh.hh>
#include <phoenix/world/bsp_tree.hh>
#include <phoenix/world/vob_tree.hh>
#include <phoenix/world/way_net.hh>

#include <memory>
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
		 * @brief Parses a world from the given reader.
		 * @param in The reader to read from.
		 * @return The world parsed.
		 */
		[[nodiscard]] static world parse(buffer& in, game_version version);

		std::vector<std::unique_ptr<vob::vob>> world_vobs;
		mesh world_mesh;
		bsp_tree world_bsp_tree;
		way_net world_way_net;
	};
} // namespace phoenix
