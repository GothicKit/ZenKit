// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/morph_mesh.hh>

TEST_SUITE("morph_mesh") {
	TEST_CASE("morph_mesh(parse:?)") {
		auto in = phoenix::buffer::mmap("./samples/morph0.mmb");
		auto mesh = phoenix::morph_mesh::parse(in);

		CHECK_EQ(mesh.name, "ITRWSMALLBOW");
		CHECK_EQ(mesh.morph_positions.size(), 28);
		CHECK_EQ(mesh.morph_positions[0], glm::vec3 {-40.9461403, -1.73638999, -26.7512894});
		CHECK_EQ(mesh.morph_positions[1], glm::vec3 {-29.6147194, -1.97599006, -9.19756984});

		auto& anims = mesh.animations;
		CHECK_EQ(anims.size(), 4);

		auto& anim = anims[1];
		CHECK_EQ(anim.name, "S_SHOOT");
		CHECK_EQ(anim.layer, 1);
		CHECK_EQ(anim.blend_in, 0.0100000007f);
		CHECK_EQ(anim.blend_out, -0.0100000007f);
		CHECK_EQ(anim.duration, 400.0f);
		CHECK_EQ(anim.speed, 0.0250000004f);
		CHECK_EQ(anim.flags, 0);

		CHECK_EQ(anim.vertices.size(), 3);
		CHECK_EQ(anim.vertices[0], 25);
		CHECK_EQ(anim.vertices[1], 26);
		CHECK_EQ(anim.vertices[2], 27);

		CHECK_EQ(anim.frame_count, 10);
		CHECK_EQ(anim.samples.size(), 30);

		CHECK_EQ(anim.samples[0], glm::vec3 {0.519770026f, 0, 1.27206039f});
		CHECK_EQ(anim.samples[9], glm::vec3 {0, 0, 0});
		CHECK_EQ(anim.samples[19], glm::vec3 {-8.51126003f, 0, -20.8299408f});

		CHECK_EQ(mesh.sources.size(), 4);
		CHECK_EQ(mesh.sources[1].file_date.year, 2000);
		CHECK_EQ(mesh.sources[1].file_date.month, 5);
		CHECK_EQ(mesh.sources[1].file_date.day, 8);
		CHECK_EQ(mesh.sources[1].file_date.hour, 9);
		CHECK_EQ(mesh.sources[1].file_date.minute, 13);
		CHECK_EQ(mesh.sources[1].file_date.second, 58);
		CHECK_EQ(mesh.sources[1].file_name, "ITRWSMALLBOWSHOOT.ASC");
	}

	TEST_CASE("model_mesh(parse:g1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("model_mesh(parse:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}
