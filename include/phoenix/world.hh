// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/World.hh"

#include "phoenix/world/bsp_tree.hh"
#include "phoenix/world/vob_tree.hh"
#include "phoenix/world/way_net.hh"

namespace phoenix {
	using world ZKREM("renamed to zenkit::World") = zenkit::World;
}
