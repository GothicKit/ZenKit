// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Mesh.hh"

#include "phoenix/material.hh"
#include "phoenix/math.hh"

namespace phoenix {
	using light_map ZKREM("renamed to zenkit::LightMap") = zenkit::LightMap;
	using vertex_feature ZKREM("renamed to zenkit::VertexFeature") = zenkit::VertexFeature;
	using polygon_flags ZKREM("renamed to zenkit::PolygonFlags") = zenkit::PolygonFlagSet;
	using polygon_list ZKREM("renamed to zenkit::PolygonList") = zenkit::PolygonList;
	using mesh ZKREM("renamed to zenkit::Mesh") = zenkit::Mesh;
} // namespace phoenix
