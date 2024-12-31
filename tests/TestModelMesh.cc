// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/ModelMesh.hh>
#include <zenkit/Stream.hh>

TEST_SUITE("ModelMesh") {
	// TODO: find a mesh with multiple attachments
	TEST_CASE("ModelMesh.load(GOTHIC?)") {
		auto in = zenkit::Read::from("./samples/secretdoor.mdm");
		zenkit::ModelMesh mesh {};
		mesh.load(in.get());

		CHECK_EQ(mesh.attachments.size(), 1);
		CHECK_NE(mesh.attachments.find("BIP01 DOOR"), mesh.attachments.end());
		CHECK_EQ(mesh.attachments.at("BIP01 DOOR").positions.size(), 8);
	}

	// TODO: find a mesh which has actual vertex weights
	TEST_CASE("ModelMesh.load(GOTHIC?)") {
		auto in = zenkit::Read::from("./samples/smoke_waterpipe.mdm");
		zenkit::ModelMesh mesh {};
		mesh.load(in.get());

		auto& meshes = mesh.meshes;
		CHECK_EQ(meshes.size(), 1);

		auto& sk = meshes[0];
		CHECK_EQ(sk.mesh.positions.size(), 115);
		CHECK_EQ(sk.mesh.normals.size(), 115);
		CHECK_EQ(sk.mesh.materials.size(), 1);
		CHECK_EQ(sk.mesh.sub_meshes.size(), 1);

		CHECK_EQ(sk.weights.size(), 115);
		CHECK_EQ(sk.weights[0].size(), 1);
		CHECK_EQ(sk.weights[0][0].weight, 1.0f);
		CHECK_EQ(sk.weights[0][0].position, zenkit::Vec3 {-5.49776077f, 35.086731f, -2.64756012f});
		CHECK_EQ(sk.weights[0][0].node_index, 0);
		CHECK_EQ(sk.weights[62].size(), 1);
		CHECK_EQ(sk.weights[62][0].weight, 1.0f);
		CHECK_EQ(sk.weights[62][0].position, zenkit::Vec3 {0.260997772f, 18.0412712f, -23.9048882f});
		CHECK_EQ(sk.weights[62][0].node_index, 4);
		CHECK_EQ(sk.weights[114].size(), 1);
		CHECK_EQ(sk.weights[114][0].weight, 1.0f);
		CHECK_EQ(sk.weights[114][0].position, zenkit::Vec3 {1.05304337f, 71.0284958f, 1.32049942f});
		CHECK_EQ(sk.weights[114][0].node_index, 0);

		CHECK_EQ(sk.wedge_normals.size(), 0);
		CHECK_EQ(sk.nodes.size(), 6);
		CHECK_EQ(sk.nodes[0], 0);
		CHECK_EQ(sk.nodes[1], 5);
		CHECK_EQ(sk.nodes[2], 3);

		CHECK_EQ(sk.bboxes.size(), 6);
		CHECK_EQ(sk.bboxes[0].center, zenkit::Vec3 {0.612892151, 41.7827187, 0.705307007});
		CHECK_EQ(sk.bboxes[0].half_width, zenkit::Vec3 {15.2073612, 33.4261742, 14.8513918});
		CHECK_EQ(sk.bboxes[0].axes[0], zenkit::Vec3 {0.777145922, 0, -0.629320442});
		CHECK_EQ(sk.bboxes[0].axes[1], zenkit::Vec3 {0, 1, 0});
		CHECK_EQ(sk.bboxes[0].axes[2], zenkit::Vec3 {0.629320442, 0, 0.777145922});

		CHECK_EQ(sk.bboxes[0].children.size(), 0);
	}

	TEST_CASE("ModelMesh.load(GOTHIC1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("ModelMesh.load(GOTHIC2)" * doctest::skip()) {
		// TODO: Stub
	}
}
