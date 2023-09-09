// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Mesh.hh"
#include "zenkit/Misc.hh"

#include "zenkit/world/BspTree.hh"
#include "zenkit/world/VobTree.hh"
#include "zenkit/world/WayNet.hh"

#include <memory>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	/// \brief Represents a ZenGin world.
	class World {
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
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static World parse(phoenix::buffer& buf, GameVersion version);

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
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static World parse(phoenix::buffer& buf);

		/// \brief Parses a world from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \param version The Gothic version to assume when loading the world
		/// \return The parsed world object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static World parse(phoenix::buffer&& buf, GameVersion version);

		/// \brief Parses a world from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed world object.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static World parse(phoenix::buffer&& buf);

		ZKAPI void load(Read* r);
		ZKAPI void load(Read* r, GameVersion version);

	public:
		/// \brief The list of VObs defined in this world.
		std::vector<std::unique_ptr<VirtualObject>> world_vobs;

		/// \brief The mesh of the world.
		Mesh world_mesh;

		/// \brief The BSP-tree of this world.
		BspTree world_bsp_tree;

		/// \brief The way-net of this world.
		WayNet world_way_net;
	};
} // namespace zenkit
