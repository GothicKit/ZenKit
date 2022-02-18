// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/proto_mesh.hh>

static bool compare_triangle(phoenix::triangle a, phoenix::triangle b) {
	return a.wedges[0] == b.wedges[0] &&
		   a.wedges[1] == b.wedges[1] &&
		   a.wedges[2] == b.wedges[2];
}

static bool compare_wedge(phoenix::wedge a, phoenix::wedge b) {
	return a.normal == b.normal &&
		   a.texture == b.texture &&
		   a.index == b.index;
}

static bool compare_plane(phoenix::plane a, phoenix::plane b) {
	return a.normal == b.normal &&
		   a.distance == b.distance;
}

TEST_CASE("proto meshes (MRMs) are read correctly", "[mesh][mrm]") {
	auto in = phoenix::reader::from("./samples/mesh0.mrm");
	auto mesh = phoenix::proto_mesh::parse(in);

	const auto& positions = mesh.positions();
	REQUIRE(positions.size() == 8);
	REQUIRE(positions[0] == glm::vec3 {200, 398.503906, 200});
	REQUIRE(positions[1] == glm::vec3 {-200, 398.503906, 200});
	REQUIRE(positions[7] == glm::vec3 {-200, 0, -200});

	REQUIRE(mesh.normals().empty());
	REQUIRE(mesh.alpha_test());

	auto [min, max] = mesh.bbox();
	REQUIRE(min == glm::vec3 {-200, 0, -200});
	REQUIRE(max == glm::vec3 {200, 398.503906, 200});

	const auto& submeshes = mesh.submeshes();
	REQUIRE(submeshes.size() == 1);

	const auto& submesh = submeshes[0];
	REQUIRE(submesh.mat.name() == "EVT_TPL_GITTERKAEFIG_01");
	REQUIRE(submesh.mat.texture() == "OCODFLGATELI.TGA");

	REQUIRE(submesh.colors.empty());
	REQUIRE(submesh.triangle_edges.empty());
	REQUIRE(submesh.edge_scores.empty());
	REQUIRE(submesh.edges.empty());

	REQUIRE(submesh.triangles.size() == 16);
	REQUIRE(compare_triangle(submesh.triangles[0], {26, 19, 12}));
	REQUIRE(compare_triangle(submesh.triangles[1], {8, 13, 18}));
	REQUIRE(compare_triangle(submesh.triangles[14], {2, 6, 29}));
	REQUIRE(compare_triangle(submesh.triangles[15], {28, 20, 3}));

	REQUIRE(submesh.wedges.size() == 32);
	REQUIRE(compare_wedge(submesh.wedges[0], {{0, 0, -1}, {-1.50000048, -1.49251938}, 4}));
	REQUIRE(compare_wedge(submesh.wedges[1], {{-1, 0, 0}, {2.49999952, -1.49251938}, 4}));
	REQUIRE(compare_wedge(submesh.wedges[31], {{0, 0, -1}, {-1.50000048, 2.49251938}, 7}));

	REQUIRE(submesh.triangle_plane_indices.size() == 16);
	REQUIRE(submesh.triangle_plane_indices[0] == 0);
	REQUIRE(submesh.triangle_plane_indices[1] == 1);
	REQUIRE(submesh.triangle_plane_indices[14] == 7);
	REQUIRE(submesh.triangle_plane_indices[15] == 5);

	REQUIRE(submesh.triangle_planes.size() == 8);
	REQUIRE(compare_plane(submesh.triangle_planes[0], {200, {1, 0, 0}}));
	REQUIRE(compare_plane(submesh.triangle_planes[1], {200, {0, 0, 1}}));
	REQUIRE(compare_plane(submesh.triangle_planes[6], {200, {0, 0, -1}}));
	REQUIRE(compare_plane(submesh.triangle_planes[7], {-200, {0, 0, 1}}));

	REQUIRE(submesh.wedge_map.size() == 32);
	REQUIRE(submesh.wedge_map[0] == 65535);
	REQUIRE(submesh.wedge_map[1] == 65535);
	REQUIRE(submesh.wedge_map[29] == 2);
	REQUIRE(submesh.wedge_map[30] == 1);
	REQUIRE(submesh.wedge_map[31] == 0);
}