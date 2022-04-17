// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "doctest/doctest.h"
#include <phoenix/model_hierarchy.hh>

TEST_SUITE("model hierarchies") {
	TEST_CASE("model hierarchies (MDHs) are read correctly") {
		auto in = phoenix::buffer::open("./samples/hierarchy0.mdh");
		auto mesh = phoenix::model_hierachy::parse(in);

		CHECK(mesh.nodes().size() == 7);
		CHECK(mesh.nodes()[0].name == "BIP01 MUEHLE");
		CHECK(mesh.nodes()[0].parent_index == -1);
		CHECK(mesh.nodes()[0].transform == glm::mat4x4 {-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1});

		CHECK(mesh.nodes()[1].name == "BIP01 NABE");
		CHECK(mesh.nodes()[1].parent_index == 0);
		CHECK(mesh.nodes()[1].transform == glm::mat4x4 {1, 0, 0, 0, 0, 1, -0, 0, 0, 0, 1, 0, 0, 0, -394.040466, 1});

		auto box0 = mesh.bbox();
		auto min0 = std::get<0>(box0), max0 = std::get<1>(box0);

		CHECK(min0 == glm::vec3 {-497.17572, -0.575592041, -105.896698});
		CHECK(max0 == glm::vec3 {515.717346, 364.943878, 893.536743});

		auto box1 = mesh.collision_bbox();
		auto min1 = std::get<0>(box1), max1 = std::get<1>(box1);
		CHECK(min1 == glm::vec3 {-248.58786f, -0.402914435f, -52.948349f});
		CHECK(max1 == glm::vec3 {257.858673f, 291.955109f, 446.768372f});

		CHECK(mesh.root_translation() == glm::vec3 {0, 0, -394.040466});
	}
}
