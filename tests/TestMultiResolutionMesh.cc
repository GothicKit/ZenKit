// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/MultiResolutionMesh.hh>
#include <zenkit/Stream.hh>

static bool compare_triangle(zenkit::MeshTriangle a, zenkit::MeshTriangle b) {
	return a.wedges[0] == b.wedges[0] && a.wedges[1] == b.wedges[1] && a.wedges[2] == b.wedges[2];
}

static bool compare_wedge(zenkit::MeshWedge const& a, zenkit::MeshWedge const& b) {
	return a.normal == b.normal && a.texture == b.texture && a.index == b.index;
}

static bool compare_plane(zenkit::MeshPlane a, zenkit::MeshPlane b) {
	return a.normal == b.normal && a.distance == b.distance;
}

TEST_SUITE("MultiResolutionMesh") {
	TEST_CASE("MultiResolutionMesh.load(GOTHIC?)") {
		auto in = zenkit::Read::from("./samples/mesh0.mrm");
		zenkit::MultiResolutionMesh mesh {};
		mesh.load(in.get());

		auto const& positions = mesh.positions;
		CHECK_EQ(positions.size(), 8);
		CHECK_EQ(positions[0], zenkit::Vec3 {200, 398.503906, 200});
		CHECK_EQ(positions[1], zenkit::Vec3 {-200, 398.503906, 200});
		CHECK_EQ(positions[7], zenkit::Vec3 {-200, 0, -200});

		CHECK(mesh.normals.empty());
		CHECK(mesh.alpha_test);

		auto box0 = mesh.bbox;
		CHECK_EQ(box0.min, zenkit::Vec3 {-200, 0, -200});
		CHECK_EQ(box0.max, zenkit::Vec3 {200, 398.503906, 200});

		auto const& submeshes = mesh.sub_meshes;
		CHECK_EQ(submeshes.size(), 1);

		auto const& submesh = submeshes[0];
		CHECK_EQ(submesh.mat.name, "EVT_TPL_GITTERKAEFIG_01");
		CHECK_EQ(submesh.mat.texture, "OCODFLGATELI.TGA");

		CHECK(submesh.colors.empty());
		CHECK(submesh.triangle_edges.empty());
		CHECK(submesh.edge_scores.empty());
		CHECK(submesh.edges.empty());

		CHECK_EQ(submesh.triangles.size(), 16);
		CHECK(compare_triangle(submesh.triangles[0], {26, 19, 12}));
		CHECK(compare_triangle(submesh.triangles[1], {8, 13, 18}));
		CHECK(compare_triangle(submesh.triangles[14], {2, 6, 29}));
		CHECK(compare_triangle(submesh.triangles[15], {28, 20, 3}));

		CHECK_EQ(submesh.wedges.size(), 32);
		CHECK(compare_wedge(submesh.wedges[0], {{0, 0, -1}, {-1.50000048, -1.49251938}, 4}));
		CHECK(compare_wedge(submesh.wedges[1], {{-1, 0, 0}, {2.49999952, -1.49251938}, 4}));
		CHECK(compare_wedge(submesh.wedges[31], {{0, 0, -1}, {-1.50000048, 2.49251938}, 7}));

		CHECK_EQ(submesh.triangle_plane_indices.size(), 16);
		CHECK_EQ(submesh.triangle_plane_indices[0], 0);
		CHECK_EQ(submesh.triangle_plane_indices[1], 1);
		CHECK_EQ(submesh.triangle_plane_indices[14], 7);
		CHECK_EQ(submesh.triangle_plane_indices[15], 5);

		CHECK_EQ(submesh.triangle_planes.size(), 8);
		CHECK(compare_plane(submesh.triangle_planes[0], {200, {1, 0, 0}}));
		CHECK(compare_plane(submesh.triangle_planes[1], {200, {0, 0, 1}}));
		CHECK(compare_plane(submesh.triangle_planes[6], {200, {0, 0, -1}}));
		CHECK(compare_plane(submesh.triangle_planes[7], {-200, {0, 0, 1}}));

		CHECK_EQ(submesh.wedge_map.size(), 32);
		CHECK_EQ(submesh.wedge_map[0], 65535);
		CHECK_EQ(submesh.wedge_map[1], 65535);
		CHECK_EQ(submesh.wedge_map[29], 2);
		CHECK_EQ(submesh.wedge_map[30], 1);
		CHECK_EQ(submesh.wedge_map[31], 0);
	}

	TEST_CASE("MultiResolutionMesh.load(GOTHIC1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("MultiResolutionMesh.load(GOTHIC2)" * doctest::skip()) {
		// TODO: Stub
	}
}
