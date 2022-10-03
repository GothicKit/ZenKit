// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/mesh.hh>
#include <phoenix/world/bsp_tree.hh>
#include <phoenix/world/vob_tree.hh>
#include <phoenix/world/way_net.hh>

#include <memory>
#include <vector>

namespace phoenix {
	/// \brief Represents a ZenGin world.
	class world {
	public:
		/// \brief Parses a world from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).
		///
		/// \param[in,out] buf The buffer to read from.
		/// \param version The Gothic version to assume when loading the world
		/// \return The parsed world object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] static world parse(buffer& buf, game_version version);

		/// \brief Parses a world from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib). This function will try to determine the world version
		/// automatically. If it can't be detected, _phoenix_ will assume `game_version::gothic_1` and log
		/// an error message.
		///
		/// Using this function over #parse(buffer&, game_version) has the potential to lead to longer load times
		/// but it has not been proven that such a case actually exists. If you absolutely need the performance and
		/// you already know the game version you are trying to load, consider using #parse(buffer&, game_version)
		/// instead.
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed world object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static world parse(buffer& buf);

		/// \brief Parses a world from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \param version The Gothic version to assume when loading the world
		/// \return The parsed world object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static world parse(buffer&& buf, game_version version) {
			return world::parse(buf, version);
		}

		/// \brief Parses a world from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed world object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static world parse(buffer&& buf) {
			return world::parse(buf);
		}

		/// \brief The list of VObs defined in this world.
		std::vector<std::unique_ptr<vob>> world_vobs;

		/// \brief The mesh of the world.
		mesh world_mesh;

		/// \brief The BSP-tree of this world.
		bsp_tree world_bsp_tree;

		/// \brief The way-net of this world.
		way_net world_way_net;
	};
} // namespace phoenix
