// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "doctest/doctest.h"
#include <phoenix/model_mesh.hh>

TEST_SUITE("model meshes") {
	// TODO: find a mesh with multiple attachments
	TEST_CASE("model meshes and their attachments are read correctly") {
		auto in = phoenix::reader::from("./samples/secretdoor.mdm");
		auto mesh = phoenix::model_mesh::parse(in);

		CHECK(mesh.attachments().size() == 1);
		CHECK(mesh.attachments().find("BIP01 DOOR") != mesh.attachments().end());
		CHECK(mesh.attachments().at("BIP01 DOOR").positions().size() == 8);
	}

	// TODO: find a mesh which has actual vertex weights
	TEST_CASE("model meshes and their softskins are correctly") {
		auto in = phoenix::reader::from("./samples/smoke_waterpipe.mdm");
		auto mesh = phoenix::model_mesh::parse(in);

		auto& meshes = mesh.meshes();
		CHECK(meshes.size() == 1);

		auto& sk = meshes[0];
		CHECK(sk.mesh().positions().size() == 115);
		CHECK(sk.mesh().normals().size() == 115);
		CHECK(sk.mesh().materials().size() == 1);
		CHECK(sk.mesh().submeshes().size() == 1);

		CHECK(sk.weights().size() == 0);
		CHECK(sk.wedge_normals().size() == 0);
		CHECK(sk.nodes().size() == 6);
		CHECK(sk.nodes()[0] == 0);
		CHECK(sk.nodes()[1] == 5);
		CHECK(sk.nodes()[2] == 3);

		CHECK(sk.bboxes().size() == 6);
		CHECK(sk.bboxes()[0].center == glm::vec3 {0.612892151, 41.7827187, 0.705307007});
		CHECK(sk.bboxes()[0].half_width == glm::vec3 {15.2073612, 33.4261742, 14.8513918});
		CHECK(sk.bboxes()[0].rotation ==
		      glm::mat3x3 {0.777145922, 0, -0.629320442, 0, 1, 0, 0.629320442, 0, 0.777145922});

		CHECK(sk.bboxes()[0].children.size() == 0);
	}
}
