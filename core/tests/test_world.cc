// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/world.hh>
#include "catch2.hh"

TEST_CASE("world meshes are read correctly", "[world][mesh]") {
	auto in = phoenix::reader::from("./samples/world.proprietary.zen");
	auto wld = phoenix::world::read(in);

	auto& mesh = wld.mesh();
	REQUIRE(mesh.vertices().size() == 55439);
	REQUIRE(mesh.features().size() == 419936);
	REQUIRE(mesh.materials().size() == 2263);
	REQUIRE(mesh.name() == "");

	auto [min, max] = mesh.bbox();
	REQUIRE(min == glm::vec4{0, 0, 0, 0});
	REQUIRE(max == glm::vec4{0, 0, 0, 0});

	auto& verts = mesh.vertices();
	REQUIRE(verts[0] == glm::vec3{91365, -4026.60083, 46900});
	REQUIRE(verts[1] == glm::vec3{92900, -4029.99976, 38399.9961});
	REQUIRE(verts[500] == glm::vec3{44263.8203, 708.517822, 6841.18262});
	REQUIRE(verts[501] == glm::vec3{45672.6094, 640.436157, 6877.81543});

	auto& feats = mesh.features();
	REQUIRE(feats[0].texture == glm::vec2{1.11193848, 2.64415169});
	REQUIRE(feats[0].light == 4292927712);
	REQUIRE(feats[0].normal == glm::vec3{0.0000220107158, 1, -0.000121058853});

	REQUIRE(feats[1].texture == glm::vec2{-0.371101379, -0.909111022});
	REQUIRE(feats[1].light == 4292927712);
	REQUIRE(feats[1].normal == glm::vec3{0.0000251403726, 1, -0.000138271935});

	REQUIRE(feats[500].texture == glm::vec2{-0.524971008, 2.59478664});
	REQUIRE(feats[500].light == 4281084972);
	REQUIRE(feats[500].normal == glm::vec3{-0.000046945388, -0.99999994, 0.000258200336});

	REQUIRE(feats[501].texture == glm::vec2{1.93376923, -0.734043121});
	REQUIRE(feats[501].light == 4281084972);
	REQUIRE(feats[501].normal == glm::vec3{0.000102534526, -1, -0.00014051389});

	auto& mats = mesh.materials();
	auto& mat0 = mats[0];
	auto& mat500 = mats[500];

	REQUIRE(mat0.name() == "OWODWATSTOP");
	REQUIRE(mat0.group() == phoenix::material_group::water);
	REQUIRE(mat0.texture() == "OWODSEA_A0.TGA");

	REQUIRE(mat500.name() == "OMWABROWNGREEN01");
	REQUIRE(mat500.group() == phoenix::material_group::stone);
	REQUIRE(mat500.texture() == "OMWABROWNGREEN01.TGA");
}

TEST_CASE("the bsp-tree is read correctly", "[world][bsp]") {
	auto in = phoenix::reader::from("./samples/world.proprietary.zen");
	auto wld = phoenix::world::read(in);
	auto& tree = wld.tree();

	REQUIRE(tree.mode() == phoenix::bsp_tree_mode::outdoor);

	auto& polys = tree.polygon_indices();
	REQUIRE(polys.size() == 480135);
	REQUIRE(polys[0] == 0);
	REQUIRE(polys[1] == 1);
	REQUIRE(polys[2] == 2);
	REQUIRE(polys[150] == 102);
	REQUIRE(polys[151] == 103);
	REQUIRE(polys[152] == 92);
}