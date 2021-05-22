// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/world.hh>

TEST_CASE("world meshes are read correctly", "[world][mesh][proprietary]") {
	auto in = phoenix::reader::from("./samples/world.proprietary.zen");
	auto wld = phoenix::world::parse(in, game_version::gothic_1);

	auto& mesh = wld.mesh();
	REQUIRE(mesh.vertices().size() == 55439);
	REQUIRE(mesh.features().size() == 419936);
	REQUIRE(mesh.materials().size() == 2263);
	REQUIRE(mesh.name() == "");

	auto [min, max] = mesh.bbox();
	REQUIRE(min == glm::vec3 {0, 0, 0});
	REQUIRE(max == glm::vec3 {0, 0, 0});

	auto& verts = mesh.vertices();
	REQUIRE(verts[0] == glm::vec3 {91365, -4026.60083, 46900});
	REQUIRE(verts[1] == glm::vec3 {92900, -4029.99976, 38399.9961});
	REQUIRE(verts[500] == glm::vec3 {44263.8203, 708.517822, 6841.18262});
	REQUIRE(verts[501] == glm::vec3 {45672.6094, 640.436157, 6877.81543});

	auto& feats = mesh.features();
	REQUIRE(feats[0].texture == glm::vec2 {1.11193848, 2.64415169});
	REQUIRE(feats[0].light == 4292927712);
	REQUIRE(feats[0].normal == glm::vec3 {0.0000220107158, 1, -0.000121058853});

	REQUIRE(feats[1].texture == glm::vec2 {-0.371101379, -0.909111022});
	REQUIRE(feats[1].light == 4292927712);
	REQUIRE(feats[1].normal == glm::vec3 {0.0000251403726, 1, -0.000138271935});

	REQUIRE(feats[500].texture == glm::vec2 {-0.524971008, 2.59478664});
	REQUIRE(feats[500].light == 4281084972);
	REQUIRE(feats[500].normal == glm::vec3 {-0.000046945388, -0.99999994, 0.000258200336});

	REQUIRE(feats[501].texture == glm::vec2 {1.93376923, -0.734043121});
	REQUIRE(feats[501].light == 4281084972);
	REQUIRE(feats[501].normal == glm::vec3 {0.000102534526, -1, -0.00014051389});

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

TEST_CASE("the bsp-tree is read correctly", "[world][bsp][proprietary]") {
	auto in = phoenix::reader::from("./samples/world.proprietary.zen");
	auto wld = phoenix::world::parse(in, game_version::gothic_1);
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

TEST_CASE("the vob-tree is read correctly", "[world][vob][proprietary]") {
	auto in = phoenix::reader::from("./samples/world.proprietary.zen");
	auto wld = phoenix::world::parse(in, game_version::gothic_1);
	auto& vobs = wld.vobs();

	REQUIRE(vobs.size() == 14);

	// FIXME: Test all kinds of VOBs
	auto& vob0 = vobs[0];
	auto& vob0_data = vob0.get<phoenix::vob::base>();

	auto& vob13 = vobs[13];
	auto& vob13_data = vob13.get<phoenix::vob::base>();

	{
		auto [min, max] = vob0_data.bbox;
		REQUIRE(min == glm::vec3 {-71919.9609, -13091.8232, -59900});
		REQUIRE(max == glm::vec3 {108999.992, 20014.0352, 67399.9921});

		auto mat = vob0_data.rotation;
		REQUIRE(mat[0][0] == 1.0f);
		REQUIRE(mat[0][1] == 0.0f);
		REQUIRE(mat[0][2] == 0.0f);
		REQUIRE(mat[1][0] == 0.0f);
		REQUIRE(mat[1][1] == 1.0f);
		REQUIRE(mat[1][2] == 0.0f);
		REQUIRE(mat[2][0] == 0.0f);
		REQUIRE(mat[2][1] == 0.0f);
		REQUIRE(mat[2][2] == 1.0f);

		REQUIRE(vob0_data.vob_name == "LEVEL-VOB");
		REQUIRE(vob0_data.visual_name == "SURFACE.3DS");
		REQUIRE(vob0_data.preset_name.empty());
		REQUIRE(vob0_data.position == glm::vec3 {0, 0, 0});
		REQUIRE(!vob0_data.show_visual);
		REQUIRE(vob0_data.camera_alignment == 0);
		REQUIRE(vob0_data.animation_mode == 0);
		REQUIRE(vob0_data.animation_strength == 0.0f);
		REQUIRE(vob0_data.far_clip_scale == 0);
		REQUIRE(vob0_data.cd_static);
		REQUIRE(!vob0_data.cd_dynamic);
		REQUIRE(!vob0_data.vob_static);
		REQUIRE(vob0_data.dynamic_shadows == 0);
		REQUIRE(vob0_data.bias == 0);
		REQUIRE(!vob0_data.ambient);
		REQUIRE(!vob0_data.physics_enabled);

		auto& children = vob0.children();
		REQUIRE(children.size() == 7496);

		auto& child1 = children[0];
		auto& child1_data = child1.get<phoenix::vob::base>();

		{
			auto [minc, maxc] = child1_data.bbox;
			REQUIRE(minc == glm::vec3 {-18596.9004, -161.17189, 4091.1333});
			REQUIRE(maxc == glm::vec3 {-18492.0723, -111.171906, 4191.26221});

			auto matc = child1_data.rotation;
			REQUIRE(matc[0][0] == -0.779196978f);
			REQUIRE(matc[0][1] == 0.0f);
			REQUIRE(matc[0][2] == 0.626779079f);
			REQUIRE(matc[1][0] == 0.0f);
			REQUIRE(matc[1][1] == 1.0f);
			REQUIRE(matc[1][2] == 0.0f);
			REQUIRE(matc[2][0] == -0.626779079f);
			REQUIRE(matc[2][1] == 0.0f);
			REQUIRE(matc[2][2] == -0.779196978f);

			REQUIRE(child1_data.vob_name == "FP_CAMPFIRE_PATH_BANDITOS2_03_02");
			REQUIRE(child1_data.visual_name.empty());
			REQUIRE(child1_data.preset_name.empty());
			REQUIRE(child1_data.position == glm::vec3 {-18544.4863, -136.171906, 4141.19727});
			REQUIRE(!child1_data.show_visual);
			REQUIRE(child1_data.camera_alignment == 0);
			REQUIRE(child1_data.animation_mode == 0);
			REQUIRE(child1_data.animation_strength == 0.0f);
			REQUIRE(child1_data.far_clip_scale == 0);
			REQUIRE(!child1_data.cd_static);
			REQUIRE(!child1_data.cd_dynamic);
			REQUIRE(!child1_data.vob_static);
			REQUIRE(child1_data.dynamic_shadows == 0);
			REQUIRE(child1_data.bias == 0);
			REQUIRE(!child1_data.ambient);
			REQUIRE(!child1_data.physics_enabled);

			REQUIRE(child1.children().empty());
		}
	}

	{
		auto [min, max] = vob13_data.bbox;
		REQUIRE(min == glm::vec3 {-9999.40234, -10000.0039, -9200});
		REQUIRE(max == glm::vec3 {9060.59765, 5909.90039, 7537.47461});

		auto mat = vob13_data.rotation;
		REQUIRE(mat[0][0] == 1.0f);
		REQUIRE(mat[0][1] == 0.0f);
		REQUIRE(mat[0][2] == 0.0f);
		REQUIRE(mat[1][0] == 0.0f);
		REQUIRE(mat[1][1] == 1.0f);
		REQUIRE(mat[1][2] == 0.0f);
		REQUIRE(mat[2][0] == 0.0f);
		REQUIRE(mat[2][1] == 0.0f);
		REQUIRE(mat[2][2] == 1.0f);

		REQUIRE(vob13_data.vob_name == "LEVEL-VOB");
		REQUIRE(vob13_data.visual_name == "OLDCAMP.3DS");
		REQUIRE(vob13_data.preset_name.empty());
		REQUIRE(vob13_data.position == glm::vec3 {0, 0, 0});
		REQUIRE(!vob13_data.show_visual);
		REQUIRE(vob13_data.camera_alignment == 0);
		REQUIRE(vob13_data.animation_mode == 0);
		REQUIRE(vob13_data.animation_strength == 0.0f);
		REQUIRE(vob13_data.far_clip_scale == 0);
		REQUIRE(!vob13_data.cd_static);
		REQUIRE(!vob13_data.cd_dynamic);
		REQUIRE(!vob13_data.vob_static);
		REQUIRE(vob13_data.dynamic_shadows == 0);
		REQUIRE(vob13_data.bias == 0);
		REQUIRE(!vob13_data.ambient);
		REQUIRE(!vob13_data.physics_enabled);

		auto& children = vob13.children();
		REQUIRE(children.size() == 3250);
	}
}

TEST_CASE("the way-net is read correctly", "[world][waynet][proprietary]") {
	auto in = phoenix::reader::from("./samples/world.proprietary.zen");
	auto wld = phoenix::world::parse(in, game_version::gothic_1);
	auto& waynet = wld.waynet();

	REQUIRE(waynet.waypoints().size() == 2784);
	REQUIRE(waynet.edges().size() == 3500);

	auto& wp0 = waynet.waypoints()[0];
	auto& wp100 = waynet.waypoints()[100];
	auto& wp500 = waynet.waypoints()[500];

	REQUIRE(wp0.name == "LOCATION_28_07");
	REQUIRE(wp0.water_depth == 0);
	REQUIRE(!wp0.under_water);
	REQUIRE(wp0.position == glm::vec3 {23871.457, -553.283813, 27821.3516});
	REQUIRE(wp0.direction == glm::vec3 {0.86651814, 0, -0.499145567});
	REQUIRE(wp0.free_point);

	REQUIRE(wp100.name == "CASTLE_MOVEMENT_STRAIGHT3");
	REQUIRE(wp100.water_depth == 0);
	REQUIRE(!wp100.under_water);
	REQUIRE(wp100.position == glm::vec3 {3362.21948, 8275.1709, -21067.9473});
	REQUIRE(wp100.direction == glm::vec3 {-0.342115372, 0, 0.939657927});
	REQUIRE(!wp100.free_point);

	REQUIRE(wp500.name == "OW_FOGDUNGEON_32");
	REQUIRE(wp500.water_depth == 0);
	REQUIRE(!wp500.under_water);
	REQUIRE(wp500.position == glm::vec3 {26636.0645, -1802.15601, 10523.1445});
	REQUIRE(wp500.direction == glm::vec3 {-0.999390841, 0, 0.0348994918});
	REQUIRE(!wp500.free_point);

	auto& edge0 = waynet.edges()[0];
	auto& edge5 = waynet.edges()[5];
	auto& edge100 = waynet.edges()[100];
	auto& edge500 = waynet.edges()[500];

	REQUIRE(edge0.a == 20);
	REQUIRE(edge0.b == 21);

	// edge 6 is a reference
	REQUIRE(edge5.a == 28);
	REQUIRE(edge5.b == 30);

	REQUIRE(edge100.a == 123);
	REQUIRE(edge100.b == 126);

	REQUIRE(edge500.a == 521);
	REQUIRE(edge500.b == 515);
}