// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/morph_mesh.hh>

TEST_SUITE("morph mesh") {
	TEST_CASE("morph meshes (MMBs) are read correctly") {
		auto in = phoenix::buffer::mmap("./samples/morph0.mmb");
		auto mesh = phoenix::morph_mesh::parse(in);

		CHECK(mesh.name == "ITRWSMALLBOW");
		CHECK(mesh.morph_positions.size() == 28);
		CHECK(mesh.morph_positions[0] == glm::vec3 {-40.9461403, -1.73638999, -26.7512894});
		CHECK(mesh.morph_positions[1] == glm::vec3 {-29.6147194, -1.97599006, -9.19756984});

		auto& anims = mesh.animations;
		CHECK(anims.size() == 4);

		auto& anim = anims[1];
		CHECK(anim.name == "S_SHOOT");
		CHECK(anim.layer == 1);
		CHECK(anim.blend_in == 0.0100000007f);
		CHECK(anim.blend_out == -0.0100000007f);
		CHECK(anim.duration == 400);
		CHECK(anim.speed == 0.0250000004f);
		CHECK(anim.flags == 0);

		CHECK(anim.vertices.size() == 3);
		CHECK(anim.vertices[0] == 25);
		CHECK(anim.vertices[1] == 26);
		CHECK(anim.vertices[2] == 27);

		CHECK(anim.frame_count == 10);
		CHECK(anim.samples.size() == 30);

		CHECK(anim.samples[0] == glm::vec3 {0.519770026f, 0, 1.27206039f});
		CHECK(anim.samples[9] == glm::vec3 {0, 0, 0});
		CHECK(anim.samples[19] == glm::vec3 {-8.51126003f, 0, -20.8299408f});

		CHECK(mesh.sources.size() == 4);
		CHECK(mesh.sources[1].file_date.year == 2000);
		CHECK(mesh.sources[1].file_date.month == 5);
		CHECK(mesh.sources[1].file_date.day == 8);
		CHECK(mesh.sources[1].file_date.hour == 9);
		CHECK(mesh.sources[1].file_date.minute == 13);
		CHECK(mesh.sources[1].file_date.second == 58);
		CHECK(mesh.sources[1].file_name == "ITRWSMALLBOWSHOOT.ASC");
	}
}
