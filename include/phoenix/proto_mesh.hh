// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/MultiResolutionMesh.hh"

namespace phoenix {
	using triangle ZKREM("renamed to zenkit::MeshTriangle") = zenkit::MeshTriangle;
	using triangle_edge ZKREM("renamed to zenkit::MeshTriangleEdge") = zenkit::MeshTriangleEdge;
	using edge ZKREM("renamed to zenkit::MeshEdge") = zenkit::MeshEdge;
	using wedge ZKREM("renamed to zenkit::MeshWedge") = zenkit::MeshWedge;
	using plane ZKREM("renamed to zenkit::MeshPlane") = zenkit::MeshPlane;
	using mesh_section ZKREM("renamed to zenkit::MeshSection") = zenkit::MeshSection;
	using sub_mesh_section ZKREM("renamed to zenkit::SubMeshSection") = zenkit::SubMeshSection;
	using sub_mesh ZKREM("renamed to zenkit::SubMesh") = zenkit::SubMesh;
	using proto_mesh ZKREM("renamed to zenkit::MultiResolutionMesh") = zenkit::MultiResolutionMesh;
} // namespace phoenix
