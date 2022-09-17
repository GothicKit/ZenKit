// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/archive.hh>
#include <phoenix/phoenix.hh>
#include <phoenix/vobs/vob.hh>

#include <glm/mat3x3.hpp>

#include <memory>
#include <variant>

namespace phoenix {
	/// \brief Parses a VOB tree from the given reader.
	/// \param in The reader to read from.
	/// \param version The version of Gothic being used.
	/// \return The tree parsed.
	std::unique_ptr<vob> parse_vob_tree(std::unique_ptr<archive_reader>& in, game_version version);
} // namespace phoenix
