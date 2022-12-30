// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/model_mesh.hh>

TEST_SUITE("model_mesh") {
	// TODO: find a mesh with multiple attachments
	TEST_CASE("model_mesh(parse:?)") {
		auto in = phoenix::buffer::mmap("./samples/secretdoor.mdm");
		auto mesh = phoenix::model_mesh::parse(in);

		CHECK_EQ(mesh.attachments.size(), 1);
		CHECK_NE(mesh.attachments.find("BIP01 DOOR"), mesh.attachments.end());
		CHECK_EQ(mesh.attachments.at("BIP01 DOOR").positions.size(), 8);
	}

	// TODO: find a mesh which has actual vertex weights
	TEST_CASE("model_mesh(parse:?)") {
		auto in = phoenix::buffer::mmap("./samples/smoke_waterpipe.mdm");
		auto mesh = phoenix::model_mesh::parse(in);

		auto& meshes = mesh.meshes;
		CHECK_EQ(meshes.size(), 1);

		auto& sk = meshes[0];
		CHECK_EQ(sk.mesh.positions.size(), 115);
		CHECK_EQ(sk.mesh.normals.size(), 115);
		CHECK_EQ(sk.mesh.materials.size(), 1);
		CHECK_EQ(sk.mesh.sub_meshes.size(), 1);

		CHECK_EQ(sk.weights.size(), 115);
		CHECK_EQ(sk.wedge_normals.size(), 0);
		CHECK_EQ(sk.nodes.size(), 6);
		CHECK_EQ(sk.nodes[0], 0);
		CHECK_EQ(sk.nodes[1], 5);
		CHECK_EQ(sk.nodes[2], 3);

		CHECK_EQ(sk.bboxes.size(), 6);
		CHECK_EQ(sk.bboxes[0].center, glm::vec3 {0.612892151, 41.7827187, 0.705307007});
		CHECK_EQ(sk.bboxes[0].half_width, glm::vec3 {15.2073612, 33.4261742, 14.8513918});
		CHECK_EQ(sk.bboxes[0].axes[0], glm::vec3 {0.777145922, 0, -0.629320442});
		CHECK_EQ(sk.bboxes[0].axes[1], glm::vec3 {0, 1, 0});
		CHECK_EQ(sk.bboxes[0].axes[2], glm::vec3 {0.629320442, 0, 0.777145922});

		CHECK_EQ(sk.bboxes[0].children.size(), 0);
	}

	TEST_CASE("model_mesh(parse:g1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("model_mesh(parse:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}
