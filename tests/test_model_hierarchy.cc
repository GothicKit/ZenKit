// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/model_hierarchy.hh>

TEST_CASE("model hierarchies (MDHs) are read correctly", "[modelhierarchy][mdh]") {
	auto in = phoenix::reader::from("./samples/hierarchy0.mdh");
	auto mesh = phoenix::model_hierachy::parse(in);

	REQUIRE(mesh.nodes().size() == 7);
	REQUIRE(mesh.nodes()[0].name == "BIP01 MUEHLE");
	REQUIRE(mesh.nodes()[0].parent_index == -1);
	REQUIRE(mesh.nodes()[0].transform == glm::mat4x4 {
												 -1, 0, 0, 0,
												 0, 1, 0, 0,
												 0, 0, -1, 0,
												 0, 0, 0, 1});

	REQUIRE(mesh.nodes()[1].name == "BIP01 NABE");
	REQUIRE(mesh.nodes()[1].parent_index == 0);
	REQUIRE(mesh.nodes()[1].transform == glm::mat4x4 {
												 1, 0, 0, 0,
												 0, 1, 0, 0,
												 0, -0, 1, -394.040466,
												 0, 0, 0, 1});

	auto [min0, max0] = mesh.bbox();
	REQUIRE(min0 == glm::vec3 {-497.17572, -0.575592041, -105.896698});
	REQUIRE(max0 == glm::vec3 {515.717346, 364.943878, 893.536743});

	auto [min1, max1] = mesh.collision_bbox();
	REQUIRE(min1 == glm::vec3 {-248.58786f, -0.402914435f, -52.948349f});
	REQUIRE(max1 == glm::vec3 {257.858673f, 291.955109f, 446.768372f});

	REQUIRE(mesh.root_translation() == glm::vec3 {0, 0, -394.040466});
}