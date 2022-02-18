// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/model_mesh.hh>

// TODO: find a mesh with multiple attachments
TEST_CASE("model meshes and their attachments are read correctly", "[modelmesh][mdm]") {
	auto in = phoenix::reader::from("./samples/secretdoor.mdm");
	auto mesh = phoenix::model_mesh::parse(in);

	REQUIRE(mesh.attachments().size() == 1);
	REQUIRE(mesh.attachments().find("BIP01 DOOR") != mesh.attachments().end());
	REQUIRE(mesh.attachments().at("BIP01 DOOR").positions().size() == 8);
}


// TODO: find a mesh which has actual vertex weights
TEST_CASE("model meshes and their softskins are correctly", "[modelmesh][mdm]") {
	auto in = phoenix::reader::from("./samples/smoke_waterpipe.mdm");
	auto mesh = phoenix::model_mesh::parse(in);

	auto& meshes = mesh.meshes();
	REQUIRE(meshes.size() == 1);

	auto& sk = meshes[0];
	REQUIRE(sk.mesh().positions().size() == 115);
	REQUIRE(sk.mesh().normals().size() == 115);
	REQUIRE(sk.mesh().materials().size() == 1);
	REQUIRE(sk.mesh().submeshes().size() == 1);

	REQUIRE(sk.weights().size() == 0);
	REQUIRE(sk.wedge_normals().size() == 0);
	REQUIRE(sk.nodes().size() == 6);
	REQUIRE(sk.nodes()[0] == 0);
	REQUIRE(sk.nodes()[1] == 5);
	REQUIRE(sk.nodes()[2] == 3);

	REQUIRE(sk.bboxes().size() == 6);
	REQUIRE(sk.bboxes()[0].center == glm::vec3 {0.612892151, 41.7827187, 0.705307007});
	REQUIRE(sk.bboxes()[0].half_width == glm::vec3 {15.2073612, 33.4261742, 14.8513918});
	REQUIRE(sk.bboxes()[0].rotation == glm::mat3x3 {0.777145922, 0, -0.629320442,
													0, 1, 0,
													0.629320442, 0, 0.777145922});

	REQUIRE(sk.bboxes()[0].children.size() == 0);
}

// data/unpk/g1_de_gog/_WORK/DATA/ANIMS/_COMPILED/SMOKE_WATERPIPE.MDM