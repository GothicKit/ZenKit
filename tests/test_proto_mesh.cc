// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/proto_mesh.hh>

static bool compare_triangle(phoenix::triangle a, phoenix::triangle b) {
	return a.wedges[0] == b.wedges[0] && a.wedges[1] == b.wedges[1] && a.wedges[2] == b.wedges[2];
}

static bool compare_wedge(phoenix::wedge a, phoenix::wedge b) {
	return a.normal == b.normal && a.texture == b.texture && a.index == b.index;
}

static bool compare_plane(phoenix::plane a, phoenix::plane b) {
	return a.normal == b.normal && a.distance == b.distance;
}

TEST_SUITE("mesh") {
	TEST_CASE("proto meshes (MRMs) are read correctly") {
		auto in = phoenix::buffer::mmap("./samples/mesh0.mrm");
		auto mesh = phoenix::proto_mesh::parse(in);

		const auto& positions = mesh.positions;
		CHECK(positions.size() == 8);
		CHECK(positions[0] == glm::vec3 {200, 398.503906, 200});
		CHECK(positions[1] == glm::vec3 {-200, 398.503906, 200});
		CHECK(positions[7] == glm::vec3 {-200, 0, -200});

		CHECK(mesh.normals.empty());
		CHECK(mesh.alpha_test);

		auto box0 = mesh.bbox;
		CHECK(box0.min == glm::vec3 {-200, 0, -200});
		CHECK(box0.max == glm::vec3 {200, 398.503906, 200});

		const auto& submeshes = mesh.sub_meshes;
		CHECK(submeshes.size() == 1);

		const auto& submesh = submeshes[0];
		CHECK(submesh.mat.name == "EVT_TPL_GITTERKAEFIG_01");
		CHECK(submesh.mat.texture == "OCODFLGATELI.TGA");

		CHECK(submesh.colors.empty());
		CHECK(submesh.triangle_edges.empty());
		CHECK(submesh.edge_scores.empty());
		CHECK(submesh.edges.empty());

		CHECK(submesh.triangles.size() == 16);
		CHECK(compare_triangle(submesh.triangles[0], {26, 19, 12}));
		CHECK(compare_triangle(submesh.triangles[1], {8, 13, 18}));
		CHECK(compare_triangle(submesh.triangles[14], {2, 6, 29}));
		CHECK(compare_triangle(submesh.triangles[15], {28, 20, 3}));

		CHECK(submesh.wedges.size() == 32);
		CHECK(compare_wedge(submesh.wedges[0], {{0, 0, -1}, {-1.50000048, -1.49251938}, 4}));
		CHECK(compare_wedge(submesh.wedges[1], {{-1, 0, 0}, {2.49999952, -1.49251938}, 4}));
		CHECK(compare_wedge(submesh.wedges[31], {{0, 0, -1}, {-1.50000048, 2.49251938}, 7}));

		CHECK(submesh.triangle_plane_indices.size() == 16);
		CHECK(submesh.triangle_plane_indices[0] == 0);
		CHECK(submesh.triangle_plane_indices[1] == 1);
		CHECK(submesh.triangle_plane_indices[14] == 7);
		CHECK(submesh.triangle_plane_indices[15] == 5);

		CHECK(submesh.triangle_planes.size() == 8);
		CHECK(compare_plane(submesh.triangle_planes[0], {200, {1, 0, 0}}));
		CHECK(compare_plane(submesh.triangle_planes[1], {200, {0, 0, 1}}));
		CHECK(compare_plane(submesh.triangle_planes[6], {200, {0, 0, -1}}));
		CHECK(compare_plane(submesh.triangle_planes[7], {-200, {0, 0, 1}}));

		CHECK(submesh.wedge_map.size() == 32);
		CHECK(submesh.wedge_map[0] == 65535);
		CHECK(submesh.wedge_map[1] == 65535);
		CHECK(submesh.wedge_map[29] == 2);
		CHECK(submesh.wedge_map[30] == 1);
		CHECK(submesh.wedge_map[31] == 0);
	}
}
