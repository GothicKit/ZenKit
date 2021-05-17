// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/world/bsp_tree.hh>
#include <phoenix/world/vob.hh>
#include <phoenix/world/world_mesh.hh>

#include <vector>

namespace phoenix {

	class world {
	public:
		/**
		 * @brief Reads a world from the given reader.
		 * @param in The reader to read from.
		 * @return The world read.
		 */
		static world read(reader& in);

		inline const std::vector<vob>& vobs() const noexcept { return _m_root_vobs; }
		inline const world_mesh& mesh() const noexcept { return _m_mesh; }
		inline const bsp_tree& tree() const noexcept { return _m_tree; }

	private:
		std::vector<vob> _m_root_vobs;
		world_mesh _m_mesh;
		bsp_tree _m_tree;
	};

}// namespace phoenix