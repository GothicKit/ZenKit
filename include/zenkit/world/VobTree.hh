// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Archive.hh"
#include "zenkit/Misc.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <memory>

namespace zenkit {
	/// \brief Parses a VOB tree from the given reader.
	/// \param in The reader to read from.
	/// \param version The version of Gothic being used.
	/// \return The tree parsed.
	ZKAPI std::shared_ptr<VirtualObject> parse_vob_tree(ReadArchive& in, GameVersion version);
	ZKAPI void save_vob_tree(WriteArchive& w, GameVersion version, std::shared_ptr<VirtualObject> const& obj);
} // namespace zenkit
