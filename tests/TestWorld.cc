// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/Material.hh>
#include <zenkit/World.hh>
#include <zenkit/vobs/VirtualObject.hh>

#include <zenkit/Stream.hh>

TEST_SUITE("World") {
	TEST_CASE("World.load(GOTHIC1)") {
		auto in = zenkit::Read::from("./samples/world.proprietary.zen");

		zenkit::World wld {};
		wld.load(in.get());

		auto& mesh = wld.world_mesh;
		CHECK_EQ(mesh.vertices.size(), 55439);
		CHECK_EQ(mesh.features.size(), 419936);
		CHECK_EQ(mesh.materials.size(), 2263);
		CHECK_EQ(mesh.name, "");

		auto box0 = mesh.bbox;
		CHECK_EQ(box0.min, glm::vec3 {0, 0, 0});
		CHECK_EQ(box0.max, glm::vec3 {0, 0, 0});

		auto obb = mesh.obb;
		CHECK_EQ(obb.center, glm::vec3 {0, 0, 0});
		CHECK_EQ(obb.axes[0], glm::vec3 {0, 0, 0});
		CHECK_EQ(obb.axes[1], glm::vec3 {0, 0, 0});
		CHECK_EQ(obb.axes[2], glm::vec3 {0, 0, 0});
		CHECK_EQ(obb.half_width, glm::vec3 {0, 0, 0});

		auto& verts = mesh.vertices;
		CHECK_EQ(verts[0], glm::vec3 {91365, -4026.60083, 46900});
		CHECK_EQ(verts[1], glm::vec3 {92900, -4029.99976, 38399.9961});
		CHECK_EQ(verts[500], glm::vec3 {44263.8203, 708.517822, 6841.18262});
		CHECK_EQ(verts[501], glm::vec3 {45672.6094, 640.436157, 6877.81543});

		auto& feats = mesh.features;
		CHECK_EQ(feats[0].texture, glm::vec2 {1.11193848, 2.64415169});
		CHECK_EQ(feats[0].light, 4292927712);
		CHECK_EQ(feats[0].normal, glm::vec3 {0.0000220107158, 1, -0.000121058853});

		CHECK_EQ(feats[1].texture, glm::vec2 {-0.371101379, -0.909111022});
		CHECK_EQ(feats[1].light, 4292927712);
		CHECK_EQ(feats[1].normal, glm::vec3 {0.0000251403726, 1, -0.000138271935});

		CHECK_EQ(feats[500].texture, glm::vec2 {-0.524971008, 2.59478664});
		CHECK_EQ(feats[500].light, 4281084972);
		CHECK_EQ(feats[500].normal, glm::vec3 {-0.000046945388, -0.99999994, 0.000258200336});

		CHECK_EQ(feats[501].texture, glm::vec2 {1.93376923, -0.734043121});
		CHECK_EQ(feats[501].light, 4281084972);
		CHECK_EQ(feats[501].normal, glm::vec3 {0.000102534526, -1, -0.00014051389});

		auto& mats = mesh.materials;
		auto& mat0 = mats[0];
		auto& mat500 = mats[500];

		CHECK_EQ(mat0.name, "OWODWATSTOP");
		CHECK_EQ(mat0.group, zenkit::MaterialGroup::WATER);
		CHECK_EQ(mat0.texture, "OWODSEA_A0.TGA");

		CHECK_EQ(mat500.name, "OMWABROWNGREEN01");
		CHECK_EQ(mat500.group, zenkit::MaterialGroup::STONE);
		CHECK_EQ(mat500.texture, "OMWABROWNGREEN01.TGA");

		auto polys = wld.world_mesh.polygons;
		CHECK_EQ(polys.material_indices.size(), 106722);
		CHECK_EQ(polys.lightmap_indices.size(), 106722);
		CHECK_EQ(polys.feature_indices.size(), 106722 * 3);
		CHECK_EQ(polys.vertex_indices.size(), 106722 * 3);
		CHECK_EQ(polys.flags.size(), 106722);

		CHECK_EQ(polys.material_indices[0], 0);
		CHECK_EQ(polys.material_indices[26680], 20);
		CHECK_EQ(polys.material_indices[53360], 1097);
		CHECK_EQ(polys.material_indices[106721], 4);

		CHECK_EQ(polys.lightmap_indices[0], -1);
		CHECK_EQ(polys.lightmap_indices[26680], -1);
		CHECK_EQ(polys.lightmap_indices[53360], 557);
		CHECK_EQ(polys.lightmap_indices[106721], -1);

		CHECK_EQ(polys.feature_indices[0 * 3 + 0], 0);
		CHECK_EQ(polys.feature_indices[0 * 3 + 1], 1);
		CHECK_EQ(polys.feature_indices[0 * 3 + 2], 2);
		CHECK_EQ(polys.feature_indices[26680 * 3 + 0], 100792);
		CHECK_EQ(polys.feature_indices[26680 * 3 + 1], 100793);
		CHECK_EQ(polys.feature_indices[26680 * 3 + 2], 100794);
		CHECK_EQ(polys.feature_indices[53360 * 3 + 0], 210349);
		CHECK_EQ(polys.feature_indices[53360 * 3 + 1], 210350);
		CHECK_EQ(polys.feature_indices[53360 * 3 + 2], 210351);
		CHECK_EQ(polys.feature_indices[106721 * 3 + 0], 419933);
		CHECK_EQ(polys.feature_indices[106721 * 3 + 1], 419934);
		CHECK_EQ(polys.feature_indices[106721 * 3 + 2], 419935);

		CHECK_EQ(polys.vertex_indices[0 * 3 + 0], 0);
		CHECK_EQ(polys.vertex_indices[0 * 3 + 1], 2);
		CHECK_EQ(polys.vertex_indices[0 * 3 + 2], 1);
		CHECK_EQ(polys.vertex_indices[26680 * 3 + 0], 14241);
		CHECK_EQ(polys.vertex_indices[26680 * 3 + 1], 14243);
		CHECK_EQ(polys.vertex_indices[26680 * 3 + 2], 14242);
		CHECK_EQ(polys.vertex_indices[53360 * 3 + 0], 28518);
		CHECK_EQ(polys.vertex_indices[53360 * 3 + 1], 28512);
		CHECK_EQ(polys.vertex_indices[53360 * 3 + 2], 28520);
		CHECK_EQ(polys.vertex_indices[106721 * 3 + 0], 55429);
		CHECK_EQ(polys.vertex_indices[106721 * 3 + 1], 55428);
		CHECK_EQ(polys.vertex_indices[106721 * 3 + 2], 54576);

		CHECK_EQ(polys.flags[0], zenkit::PolygonFlagSet {0, 0, 0, 0, 0, 0, 0, -1, 1, 1});
		CHECK_EQ(polys.flags[26680], zenkit::PolygonFlagSet {0, 0, 0, 0, 0, 0, 0, -1, 1, 1});
		CHECK_EQ(polys.flags[53360], zenkit::PolygonFlagSet {0, 1, 1, 0, 0, 0, 0, -1, 0, 0});
		CHECK_EQ(polys.flags[106721], zenkit::PolygonFlagSet {0, 0, 0, 0, 0, 0, 0, -1, 1, 1});

		// Check the BSP tree
		auto& tree = wld.world_bsp_tree;
		CHECK_EQ(tree.mode, zenkit::BspTreeType::OUTDOOR);

		auto& tree_polys = tree.polygon_indices;
		CHECK_EQ(tree_polys.size(), 480135);
		CHECK_EQ(tree_polys[0], 0);
		CHECK_EQ(tree_polys[1], 1);
		CHECK_EQ(tree_polys[2], 2);
		CHECK_EQ(tree_polys[150], 102);
		CHECK_EQ(tree_polys[151], 103);
		CHECK_EQ(tree_polys[152], 92);

		auto& nodes = tree.nodes;
		CHECK_EQ(nodes.size(), 6644);
		CHECK_EQ(nodes[0].plane, glm::vec4 {1, 0, 0, 18540.0156f});
		CHECK_EQ(nodes[0].front_index, 1);
		CHECK_EQ(nodes[0].parent_index, -1);
		CHECK_EQ(nodes[0].back_index, 1599);
		CHECK_EQ(nodes[0].polygon_index, 0);
		CHECK_EQ(nodes[0].polygon_count, 0);
		CHECK_EQ(nodes[0].bbox.min, glm::vec3 {-71919.9609f, -12000, -59900});
		CHECK_EQ(nodes[0].bbox.max, glm::vec3 {108999.992f, 20014.0371f, 67399.9921f});
		CHECK_FALSE(nodes[0].is_leaf());

		CHECK_EQ(nodes[1].plane, glm::vec4 {0, 0, 1, 3749.99609f});
		CHECK_EQ(nodes[1].front_index, 2);
		CHECK_EQ(nodes[1].parent_index, 0);
		CHECK_EQ(nodes[1].back_index, 445);
		CHECK_EQ(nodes[1].polygon_index, 0);
		CHECK_EQ(nodes[1].polygon_count, 0);
		CHECK_EQ(nodes[1].bbox.min, glm::vec3 {15499.999f, -12000, -59900});
		CHECK_EQ(nodes[1].bbox.max, glm::vec3 {108999.992f, 19502.1973f, 67399.9921f});
		CHECK_FALSE(nodes[1].is_leaf());

		auto& leaves = tree.leaf_node_indices;
		CHECK_EQ(leaves.size(), 3318);
		CHECK_EQ(leaves[0], 5);
		CHECK_EQ(leaves[10], 26);

		CHECK(nodes[5].is_leaf());
		CHECK_EQ(nodes[5].parent_index, 4);
		CHECK_EQ(nodes[5].polygon_index, 0);
		CHECK_EQ(nodes[5].polygon_count, 22);
		CHECK_EQ(nodes[5].bbox.min, glm::vec3 {81900, -4029.99976f, 28500.0039f});
		CHECK_EQ(nodes[5].bbox.max, glm::vec3 {100000, -4026.60083f, 53899.9922f});

		CHECK(nodes[26].is_leaf());
		CHECK_EQ(nodes[26].parent_index, 25);
		CHECK_EQ(nodes[26].polygon_index, 446);
		CHECK_EQ(nodes[26].polygon_count, 24);
		CHECK_EQ(nodes[26].bbox.min, glm::vec3 {48899.9961f, -4029.99976f, 47400});
		CHECK_EQ(nodes[26].bbox.max, glm::vec3 {67900, -4026.59961f, 67399.9921f});

		auto& sectors = tree.sectors;
		CHECK_EQ(sectors.size(), 299);

		CHECK_EQ(sectors[0].name, "WALD11");
		CHECK_EQ(sectors[0].node_indices.size(), 9);
		CHECK_EQ(sectors[0].portal_polygon_indices.size(), 24);

		CHECK_EQ(sectors[50].name, "OWCAVE01");
		CHECK_EQ(sectors[50].node_indices.size(), 4);
		CHECK_EQ(sectors[50].portal_polygon_indices.size(), 2);

		auto& portal_polys = tree.portal_polygon_indices;
		CHECK_EQ(portal_polys.size(), 0);

		CHECK_EQ(tree.light_points.size(), 3318);
		CHECK_EQ(tree.light_points[0], glm::vec3 {-99, -99, -99});

		// Check the VOb tree
		auto& vobs = wld.world_vobs;
		CHECK_EQ(vobs.size(), 14);

		auto& vob0 = vobs[0];
		auto& vob13 = vobs[13];

		{
			box0 = vob0->bbox;
			CHECK_EQ(box0.min, glm::vec3 {-71919.9609, -13091.8232, -59900});
			CHECK_EQ(box0.max, glm::vec3 {108999.992, 20014.0352, 67399.9921});

			auto mat = vob0->rotation;
			CHECK_EQ(mat[0][0], 1.0f);
			CHECK_EQ(mat[1][0], 0.0f);
			CHECK_EQ(mat[2][0], 0.0f);
			CHECK_EQ(mat[0][1], 0.0f);
			CHECK_EQ(mat[1][1], 1.0f);
			CHECK_EQ(mat[2][1], 0.0f);
			CHECK_EQ(mat[0][2], 0.0f);
			CHECK_EQ(mat[1][2], 0.0f);
			CHECK_EQ(mat[2][2], 1.0f);

			CHECK_EQ(vob0->vob_name, "LEVEL-VOB");
			CHECK_EQ(vob0->visual_name, "SURFACE.3DS");
			CHECK(vob0->preset_name.empty());
			CHECK_EQ(vob0->position, glm::vec3 {0, 0, 0});
			CHECK_FALSE(vob0->show_visual);
			CHECK_EQ(vob0->sprite_camera_facing_mode, zenkit::SpriteAlignment::NONE);
			CHECK_EQ(vob0->anim_mode, zenkit::AnimationType::NONE);
			CHECK_EQ(vob0->anim_strength, 0.0f);
			CHECK_EQ(vob0->far_clip_scale, 2.0f);
			CHECK(vob0->cd_static);
			CHECK_FALSE(vob0->cd_dynamic);
			CHECK_FALSE(vob0->vob_static);
			CHECK_EQ(vob0->dynamic_shadows, zenkit::ShadowType::NONE);
			CHECK_EQ(vob0->bias, 0);
			CHECK_FALSE(vob0->ambient);
			CHECK_FALSE(vob0->physics_enabled);

			auto& children = vob0->children;
			CHECK_EQ(children.size(), 7496);

			auto& child1 = children[0];

			{
				auto box1 = child1->bbox;
				CHECK_EQ(box1.min, glm::vec3 {-18596.9004, -161.17189, 4091.1333});
				CHECK_EQ(box1.max, glm::vec3 {-18492.0723, -111.171906, 4191.26221});

				auto matc = child1->rotation;
				CHECK_EQ(matc[0][0], -0.779196978f);
				CHECK_EQ(matc[1][0], 0.0f);
				CHECK_EQ(matc[2][0], 0.626779079f);

				CHECK_EQ(matc[0][1], 0.0f);
				CHECK_EQ(matc[1][1], 1.0f);
				CHECK_EQ(matc[2][1], 0.0f);

				CHECK_EQ(matc[0][2], -0.626779079f);
				CHECK_EQ(matc[1][2], 0.0f);
				CHECK_EQ(matc[2][2], -0.779196978f);

				CHECK_EQ(child1->vob_name, "FP_CAMPFIRE_PATH_BANDITOS2_03_02");
				CHECK(child1->visual_name.empty());
				CHECK(child1->preset_name.empty());
				CHECK_EQ(child1->position, glm::vec3 {-18544.4863, -136.171906, 4141.19727});
				CHECK_FALSE(child1->show_visual);
				CHECK_EQ(child1->sprite_camera_facing_mode, zenkit::SpriteAlignment::NONE);
				CHECK_EQ(child1->anim_mode, zenkit::AnimationType::NONE);
				CHECK_EQ(child1->anim_strength, 0.0f);
				CHECK_EQ(child1->far_clip_scale, 2.0f);
				CHECK_FALSE(child1->cd_static);
				CHECK_FALSE(child1->cd_dynamic);
				CHECK_FALSE(child1->vob_static);
				CHECK_EQ(child1->dynamic_shadows, zenkit::ShadowType::NONE);
				CHECK_EQ(child1->bias, 0);
				CHECK_FALSE(child1->ambient);
				CHECK_FALSE(child1->physics_enabled);

				CHECK(child1->children.empty());
			}
		}

		{
			auto box2 = vob13->bbox;
			CHECK_EQ(box2.min, glm::vec3 {-9999.40234, -10000.0039, -9200});
			CHECK_EQ(box2.max, glm::vec3 {9060.59765, 5909.90039, 7537.47461});

			auto mat = vob13->rotation;
			CHECK_EQ(mat[0][0], 1.0f);
			CHECK_EQ(mat[1][0], 0.0f);
			CHECK_EQ(mat[2][0], 0.0f);
			CHECK_EQ(mat[0][1], 0.0f);
			CHECK_EQ(mat[1][1], 1.0f);
			CHECK_EQ(mat[2][1], 0.0f);
			CHECK_EQ(mat[0][2], 0.0f);
			CHECK_EQ(mat[1][2], 0.0f);
			CHECK_EQ(mat[2][2], 1.0f);

			CHECK_EQ(vob13->vob_name, "LEVEL-VOB");
			CHECK_EQ(vob13->visual_name, "OLDCAMP.3DS");
			CHECK(vob13->preset_name.empty());
			CHECK_EQ(vob13->position, glm::vec3 {0, 0, 0});
			CHECK_FALSE(vob13->show_visual);
			CHECK_EQ(vob13->sprite_camera_facing_mode, zenkit::SpriteAlignment::NONE);
			CHECK_EQ(vob13->anim_mode, zenkit::AnimationType::NONE);
			CHECK_EQ(vob13->anim_strength, 0.0f);
			CHECK_EQ(vob13->far_clip_scale, 2.0f);
			CHECK_FALSE(vob13->cd_static);
			CHECK_FALSE(vob13->cd_dynamic);
			CHECK_FALSE(vob13->vob_static);
			CHECK_EQ(vob13->dynamic_shadows, zenkit::ShadowType::NONE);
			CHECK_EQ(vob13->bias, 0);
			CHECK_FALSE(vob13->ambient);
			CHECK_FALSE(vob13->physics_enabled);

			auto& children = vob13->children;
			CHECK_EQ(children.size(), 3250);
		}

		// Check the waynet
#ifndef ZK_FUTURE
		auto& waynet = wld.world_way_net;
		CHECK_EQ(waynet.waypoints.size(), 2784);
		CHECK_EQ(waynet.edges.size(), 3500);

		auto& wp0 = waynet.waypoints[0];
		auto& wp100 = waynet.waypoints[100];

		CHECK_EQ(wp0.name, "LOCATION_28_07");
		CHECK_EQ(wp0.water_depth, 0);
		CHECK_FALSE(wp0.under_water);
		CHECK_EQ(wp0.position, glm::vec3 {23871.457, -553.283813, 27821.3516});
		CHECK_EQ(wp0.direction, glm::vec3 {0.86651814, 0, -0.499145567});
		CHECK(wp0.free_point);

		CHECK_EQ(wp100.name, "CASTLE_MOVEMENT_STRAIGHT3");
		CHECK_EQ(wp100.water_depth, 0);
		CHECK_FALSE(wp100.under_water);
		CHECK_EQ(wp100.position, glm::vec3 {3362.21948, 8275.1709, -21067.9473});
		CHECK_EQ(wp100.direction, glm::vec3 {-0.342115372, 0, 0.939657927});
		CHECK_FALSE(wp100.free_point);

		auto& edge0 = waynet.edges[0];
		auto& edge5 = waynet.edges[5];
		auto& edge100 = waynet.edges[100];
		auto& edge500 = waynet.edges[500];

		CHECK_EQ(edge0.a, 20);
		CHECK_EQ(edge0.b, 21);

		// edge 6 is a reference
		CHECK_EQ(edge5.a, 28);
		CHECK_EQ(edge5.b, 30);

		CHECK_EQ(edge100.a, 123);
		CHECK_EQ(edge100.b, 126);

		CHECK_EQ(edge500.a, 521);
		CHECK_EQ(edge500.b, 515);

#else
		auto& waynet = *wld.way_net;
		CHECK_EQ(waynet.points.size(), 2784);
		CHECK_EQ(waynet.edges.size(), 3500);

		auto& wp0 = *waynet.points[0];
		auto& wp100 = *waynet.points[100];

		CHECK_EQ(wp0.name, "LOCATION_28_07");
		CHECK_EQ(wp0.water_depth, 0);
		CHECK_FALSE(wp0.under_water);
		CHECK_EQ(wp0.position, glm::vec3 {23871.457, -553.283813, 27821.3516});
		CHECK_EQ(wp0.direction, glm::vec3 {0.86651814, 0, -0.499145567});

		CHECK_EQ(wp100.name, "CASTLE_MOVEMENT_STRAIGHT3");
		CHECK_EQ(wp100.water_depth, 0);
		CHECK_FALSE(wp100.under_water);
		CHECK_EQ(wp100.position, glm::vec3 {3362.21948, 8275.1709, -21067.9473});
		CHECK_EQ(wp100.direction, glm::vec3 {-0.342115372, 0, 0.939657927});
#endif
	}

	TEST_CASE("World.load(GOTHIC2)" * doctest::skip()) {
		// TODO: Stub
	}
}
