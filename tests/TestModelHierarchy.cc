// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/model_hierarchy.hh>

TEST_SUITE("model_hierarchy") {
	TEST_CASE("model_hierarchy(parse:?)") {
		auto in = phoenix::buffer::mmap("./samples/hierarchy0.mdh");
		auto mesh = phoenix::model_hierarchy::parse(in);

		CHECK_EQ(mesh.nodes.size(), 7);
		CHECK_EQ(mesh.nodes[0].name, "BIP01 MUEHLE");
		CHECK_EQ(mesh.nodes[0].parent_index, -1);
		CHECK_EQ(mesh.nodes[0].transform, glm::mat4x4 {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1});

		CHECK_EQ(mesh.nodes[1].name, "BIP01 NABE");
		CHECK_EQ(mesh.nodes[1].parent_index, 0);
		CHECK_EQ(mesh.nodes[1].transform, glm::mat4x4 {1, 0, 0, 0, 0, 1, -0, 0, 0, 0, 1, 0, 0, 0, -394.040466, 1});

		auto box0 = mesh.bbox;
		CHECK_EQ(box0.min, glm::vec3 {-497.17572, -0.575592041, -105.896698});
		CHECK_EQ(box0.max, glm::vec3 {515.717346, 364.943878, 893.536743});

		auto box1 = mesh.collision_bbox;
		CHECK_EQ(box1.min, glm::vec3 {-248.58786f, -0.402914435f, -52.948349f});
		CHECK_EQ(box1.max, glm::vec3 {257.858673f, 291.955109f, 446.768372f});

		CHECK_EQ(mesh.root_translation, glm::vec3 {0, 0, -394.040466});
	}

	TEST_CASE("model_hierarchy(parse:g1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("model_hierarchy(parse:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}
