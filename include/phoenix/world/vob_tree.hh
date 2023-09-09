// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/world/VobTree.hh"

#include "phoenix/vobs/vob.hh"

namespace phoenix {
	ZKAPI inline std::unique_ptr<zenkit::VirtualObject> parse_vob_tree(zenkit::ReadArchive& in,
	                                                                   zenkit::GameVersion version) {
		return zenkit::parse_vob_tree(in, version);
	}
} // namespace phoenix
