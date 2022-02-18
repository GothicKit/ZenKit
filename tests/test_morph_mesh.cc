// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/morph_mesh.hh>

TEST_CASE("morph meshes (MMBs) are read correctly", "[morphmesh][mmb]") {
	auto in = phoenix::reader::from("./samples/morph0.mmb");
	auto mesh = phoenix::morph_mesh::parse(in);

	REQUIRE(mesh.name() == "ITRWSMALLBOW");
	REQUIRE(mesh.positions().size() == 28);
	REQUIRE(mesh.positions()[0] == glm::vec3 {-40.9461403, -1.73638999, -26.7512894});
	REQUIRE(mesh.positions()[1] == glm::vec3 {-29.6147194, -1.97599006, -9.19756984});

	auto& anims = mesh.animations();
	REQUIRE(anims.size() == 4);

	auto& anim = anims[1];
	REQUIRE(anim.name == "S_SHOOT");
	REQUIRE(anim.layer == 1);
	REQUIRE(anim.blend_in == 0.0100000007f);
	REQUIRE(anim.blend_out == -0.0100000007f);
	REQUIRE(anim.duration == 400);
	REQUIRE(anim.speed == 0.0250000004f);
	REQUIRE(anim.flags == 0);

	REQUIRE(anim.vertices.size() == 3);
	REQUIRE(anim.vertices[0] == 25);
	REQUIRE(anim.vertices[1] == 26);
	REQUIRE(anim.vertices[2] == 27);

	REQUIRE(anim.frame_count == 10);
	REQUIRE(anim.samples.size() == 30);

	REQUIRE(anim.samples[0] == glm::vec3 {0.519770026f, 0, 1.27206039f});
	REQUIRE(anim.samples[9] == glm::vec3 {0, 0, 0});
	REQUIRE(anim.samples[19] == glm::vec3 {-8.51126003f, 0, -20.8299408f});

	REQUIRE(mesh.sources().size() == 4);
	REQUIRE(mesh.sources()[1].file_date.year == 2000);
	REQUIRE(mesh.sources()[1].file_date.month == 5);
	REQUIRE(mesh.sources()[1].file_date.day == 8);
	REQUIRE(mesh.sources()[1].file_date.hour == 9);
	REQUIRE(mesh.sources()[1].file_date.minute == 13);
	REQUIRE(mesh.sources()[1].file_date.second == 58);
	REQUIRE(mesh.sources()[1].file_name == "ITRWSMALLBOWSHOOT.ASC");
}