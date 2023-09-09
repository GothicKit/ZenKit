// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/world/BspTree.hh"

namespace phoenix {
	using bsp_tree ZKREM("renamed to zenkit::BspTree") = zenkit::BspTree;
	using bsp_sector ZKREM("renamed to zenkit::BspSector") = zenkit::BspSector;
	using bsp_node ZKREM("renamed to zenkit::BspNode") = zenkit::BspNode;
	using bsp_tree_mode ZKREM("renamed to zenkit::BspTreeType") = zenkit::BspTreeType;
} // namespace phoenix
