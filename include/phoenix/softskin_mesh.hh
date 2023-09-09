// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/SoftSkinMesh.hh"

#include "phoenix/math.hh"

namespace phoenix {
	using softskin_mesh ZKREM("renamed to zenkit::SoftSkinMesh") = zenkit::SoftSkinMesh;
	using wedge_normal ZKREM("renamed to zenkit::SoftSkinWedgeNormal") = zenkit::SoftSkinWedgeNormal;
	using weight_entry ZKREM("renamed to zenkit::SoftSkinWeightEntry") = zenkit::SoftSkinWeightEntry;
} // namespace phoenix
