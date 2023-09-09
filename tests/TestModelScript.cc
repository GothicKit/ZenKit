// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/Logger.hh>
#include <zenkit/ModelScript.hh>
#include <zenkit/Stream.hh>

TEST_SUITE("ModelScript") {
	TEST_CASE("ModelScript.load(GOTHIC?)") {
		zenkit::Logger::set_default(zenkit::LogLevel::INFO);
		auto r = zenkit::Read::from("./samples/waran.mds");
		zenkit::ModelScript script {};
		script.load(r.get());

		CHECK_EQ(script.skeleton.disable_mesh, true);
		CHECK_EQ(script.skeleton.name, "TestModelMesh.asc");

		CHECK_EQ(script.animations.size(), 2);
		CHECK_EQ(script.animations[0].name, "aniName1");
		CHECK_EQ(script.animations[0].layer, 111);
		CHECK_EQ(script.animations[0].next, "aniNext1");
		CHECK_EQ(script.animations[0].blend_in, 4.2f);
		CHECK_EQ(script.animations[0].blend_out, 0.5f);
		CHECK_EQ(script.animations[0].flags, (zenkit::AnimationFlags::MOVE | zenkit::AnimationFlags::IDLE));
		CHECK_EQ(script.animations[0].model, "aniModel1");
		CHECK_EQ(script.animations[0].direction, zenkit::AnimationDirection::FORWARD);
		CHECK_EQ(script.animations[0].first_frame, 221);
		CHECK_EQ(script.animations[0].last_frame, -331);
		CHECK_EQ(script.animations[0].fps, 25.0f);
		CHECK_EQ(script.animations[0].speed, 0.0f);
		CHECK_EQ(script.animations[0].collision_volume_scale, 0.2f);
		CHECK(script.animations[0].pfx.empty());
		CHECK(script.animations[0].pfx_stop.empty());
		CHECK(script.animations[0].sfx.empty());
		CHECK(script.animations[0].sfx_ground.empty());
		CHECK(script.animations[0].events.empty());
		CHECK(script.animations[0].morph.empty());
		CHECK(script.animations[0].tremors.empty());

		CHECK_EQ(script.animations[1].name, "aniName2");
		CHECK_EQ(script.animations[1].layer, 112);
		CHECK_EQ(script.animations[1].next, "aniNext2");
		CHECK_EQ(script.animations[1].blend_in, 9.0f);
		CHECK_EQ(script.animations[1].blend_out, 0.0f);
		CHECK_EQ(script.animations[1].flags, zenkit::AnimationFlags::MOVE);
		CHECK_EQ(script.animations[1].model, "aniModel2");
		CHECK_EQ(script.animations[1].direction, zenkit::AnimationDirection::BACKWARD);
		CHECK_EQ(script.animations[1].first_frame, 222);
		CHECK_EQ(script.animations[1].last_frame, 332);
		CHECK_EQ(script.animations[1].fps, 25.0f);
		CHECK_EQ(script.animations[1].speed, 0.0f);
		CHECK_EQ(script.animations[1].collision_volume_scale, 1.0f);

		CHECK_EQ(script.animations[1].events.size(), 3);
		CHECK_EQ(script.animations[1].events[0].frame, 0);
		CHECK_EQ(script.animations[1].events[0].type, zenkit::MdsEventType::TORCH_DROP);
		CHECK_EQ(script.animations[1].events[0].attached, true);
		CHECK_EQ(script.animations[1].events[1].frame, 1);
		CHECK_EQ(script.animations[1].events[1].type, zenkit::MdsEventType::COMBO_WINDOW);
		CHECK_EQ(script.animations[1].events[1].frames, std::vector<int32_t> {1, 2, 3, 4, 5});
		CHECK_EQ(script.animations[1].events[1].attached, false);
		CHECK_EQ(script.animations[1].events[2].frame, 0); // Defaulted
		CHECK_EQ(script.animations[1].events[2].type, zenkit::MdsEventType::ITEM_CREATE);
		CHECK_EQ(script.animations[1].events[2].slot, "eventSlot");
		CHECK_EQ(script.animations[1].events[2].item, "eventItem");
		CHECK_EQ(script.animations[1].events[2].attached, false);

		CHECK_EQ(script.animations[1].sfx.size(), 3);
		CHECK_EQ(script.animations[1].sfx[0].frame, 3);
		CHECK_EQ(script.animations[1].sfx[0].name, "sfxName1");
		CHECK_EQ(script.animations[1].sfx[0].empty_slot, true);
		CHECK_EQ(script.animations[1].sfx[1].frame, 4);
		CHECK_EQ(script.animations[1].sfx[1].name, "sfxName2");
		CHECK_EQ(script.animations[1].sfx[1].range, 67.4f);
		CHECK_EQ(script.animations[1].sfx[1].empty_slot, false);

		CHECK_EQ(script.animations[1].sfx_ground.size(), 1);
		CHECK_EQ(script.animations[1].sfx_ground[0].frame, 5);
		CHECK_EQ(script.animations[1].sfx_ground[0].name, "sfxGrndName");
		CHECK_EQ(script.animations[1].sfx_ground[0].empty_slot, false);

		CHECK_EQ(script.animations[1].pfx.size(), 3);
		CHECK_EQ(script.animations[1].pfx[0].frame, 6);
		CHECK_EQ(script.animations[1].pfx[0].index, 0);
		CHECK_EQ(script.animations[1].pfx[0].name, "pfxName1");
		CHECK_EQ(script.animations[1].pfx[0].position, "pfxPosition1");
		CHECK_EQ(script.animations[1].pfx[0].attached, true);
		CHECK_EQ(script.animations[1].pfx[1].frame, 7);
		CHECK_EQ(script.animations[1].pfx[1].index, 991);
		CHECK_EQ(script.animations[1].pfx[1].name, "pfxName2");
		CHECK_EQ(script.animations[1].pfx[1].position, "pfxPosition2");
		CHECK_EQ(script.animations[1].pfx[1].attached, false);
		CHECK_EQ(script.animations[1].pfx[2].frame, 9);
		CHECK_EQ(script.animations[1].pfx[2].index, 991);
		CHECK_EQ(script.animations[1].pfx[2].name, "pfxName3");
		CHECK_EQ(script.animations[1].pfx[2].position, "pfxPosition3");
		CHECK_EQ(script.animations[1].pfx[2].attached, true);

		CHECK_EQ(script.animations[1].pfx_stop.size(), 1);
		CHECK_EQ(script.animations[1].pfx_stop[0].frame, 8);
		CHECK_EQ(script.animations[1].pfx_stop[0].index, 992);

		CHECK_EQ(script.animations[1].morph.size(), 2);
		CHECK_EQ(script.animations[1].morph[0].frame, 9);
		CHECK_EQ(script.animations[1].morph[0].animation, "mmAni1");
		CHECK(script.animations[1].morph[0].node.empty());
		CHECK_EQ(script.animations[1].morph[1].frame, 10);
		CHECK_EQ(script.animations[1].morph[1].animation, "mmAni2");
		CHECK_EQ(script.animations[1].morph[1].node, "mmNode");

		CHECK_EQ(script.animations[1].tremors.size(), 1);
		CHECK_EQ(script.animations[1].tremors[0].frame, 11);
		CHECK_EQ(script.animations[1].tremors[0].field1, 881);
		CHECK_EQ(script.animations[1].tremors[0].field2, 882);
		CHECK_EQ(script.animations[1].tremors[0].field3, 883);
		CHECK_EQ(script.animations[1].tremors[0].field4, 884);

		CHECK_EQ(script.blends.size(), 3);
		CHECK_EQ(script.blends[0].name, "blendName1");
		CHECK_EQ(script.blends[0].next, "blendNext1");
		CHECK_EQ(script.blends[1].name, "blendName2");
		CHECK_EQ(script.blends[1].next, "blendNext2");
		CHECK_EQ(script.blends[2].name, "blendName3");
		CHECK_EQ(script.blends[2].next, "blendNext3");
		CHECK_EQ(script.blends[2].blend_in, 223.1f);
		CHECK_EQ(script.blends[2].blend_out, 333.1f);

		CHECK_EQ(script.aliases.size(), 2);
		CHECK_EQ(script.aliases[0].name, "aliasName1");
		CHECK_EQ(script.aliases[0].layer, 114);
		CHECK_EQ(script.aliases[0].next, "aliasNext1");
		CHECK_EQ(script.aliases[0].blend_in, 100.1f);
		CHECK_EQ(script.aliases[0].blend_out, 200.2f);
		CHECK_EQ(script.aliases[0].flags, (zenkit::AnimationFlags::ROTATE | zenkit::AnimationFlags::QUEUE));
		CHECK_EQ(script.aliases[0].alias, "aliasAlias1");
		CHECK_EQ(script.aliases[0].direction, zenkit::AnimationDirection::FORWARD);
		CHECK_EQ(script.aliases[1].name, "aliasName2");
		CHECK_EQ(script.aliases[1].layer, 115);
		CHECK_EQ(script.aliases[1].next, "aliasNext2");
		CHECK_EQ(script.aliases[1].blend_in, 101.1f);
		CHECK_EQ(script.aliases[1].blend_out, 201.2f);
		CHECK_EQ(script.aliases[1].flags, zenkit::AnimationFlags::FLY);
		CHECK_EQ(script.aliases[1].alias, "aliasAlias2");
		CHECK_EQ(script.aliases[1].direction, zenkit::AnimationDirection::BACKWARD);

		CHECK_EQ(script.combinations.size(), 2);
		CHECK_EQ(script.combinations[0].name, "combName1");
		CHECK_EQ(script.combinations[0].layer, 116);
		CHECK_EQ(script.combinations[0].next, "combNext1");
		CHECK_EQ(script.combinations[0].blend_in, 102.1f);
		CHECK_EQ(script.combinations[0].blend_out, 202.2f);
		CHECK_EQ(script.combinations[0].flags, zenkit::AnimationFlags::MOVE);
		CHECK_EQ(script.combinations[0].model, "combModel1");
		CHECK_EQ(script.combinations[0].last_frame, 226);
		CHECK_EQ(script.combinations[1].name, "combName2");
		CHECK_EQ(script.combinations[1].layer, 117);
		CHECK_EQ(script.combinations[1].next, "combNext2");
		CHECK_EQ(script.combinations[1].blend_in, 103.1f);
		CHECK_EQ(script.combinations[1].blend_out, 203.2f);
		CHECK_EQ(script.combinations[1].flags, zenkit::AnimationFlags::IDLE);
		CHECK_EQ(script.combinations[1].model, "combModel2");
		CHECK_EQ(script.combinations[1].last_frame, 227);

		CHECK_EQ(script.disabled_animations.size(), 2);
		CHECK_EQ(script.disabled_animations[0], "disable1");
		CHECK_EQ(script.disabled_animations[1], "disable2");

		CHECK_EQ(script.model_tags.size(), 2);
		CHECK_EQ(script.model_tags[0].bone, "tag1");
		CHECK_EQ(script.model_tags[1].bone, "tag2");
	}

	TEST_CASE("ModelScript.load(GOTHIC?,BINARY)") {
		auto buf = zenkit::Read::from("./samples/waran.msb");
		zenkit::ModelScript script {};
		script.load(buf.get());

		CHECK_EQ(script.skeleton.disable_mesh, true);
		CHECK_EQ(script.skeleton.name, "WAR_BODY");

		CHECK_EQ(script.animations.size(), 62);
		CHECK_EQ(script.animations[0].name, "T_FISTRUN_2_FISTRUNL");
		CHECK_EQ(script.animations[0].layer, 1);
		CHECK_EQ(script.animations[0].next, "S_FISTRUNL");
		CHECK_EQ(script.animations[0].blend_in, 0.0f);
		CHECK_EQ(script.animations[0].blend_out, 0.0f);
		CHECK_EQ(script.animations[0].flags, zenkit::AnimationFlags::MOVE);
		CHECK_EQ(script.animations[0].model, "WARAN_RUN_KM01.ASC");
		CHECK_EQ(script.animations[0].direction, zenkit::AnimationDirection::FORWARD);
		CHECK_EQ(script.animations[0].first_frame, 1);
		CHECK_EQ(script.animations[0].last_frame, 8);
		CHECK_EQ(script.animations[0].fps, 25.0f);
		CHECK_EQ(script.animations[0].speed, 0.0f);                  // default
		CHECK_EQ(script.animations[0].collision_volume_scale, 1.0f); // default

		CHECK_EQ(script.aliases.size(), 38);
		CHECK_EQ(script.aliases[0].name, "S_FISTRUN");
		CHECK_EQ(script.aliases[0].layer, 1);
		CHECK_EQ(script.aliases[0].next, "S_FISTRUN");
		CHECK_EQ(script.aliases[0].blend_in, 0.0f);
		CHECK_EQ(script.aliases[0].blend_out, 0.0f);
		CHECK_EQ(script.aliases[0].flags, (zenkit::AnimationFlags::MOVE | zenkit::AnimationFlags::IDLE));
		CHECK_EQ(script.aliases[0].alias, "S_FISTWALK");
		CHECK_EQ(script.aliases[0].direction, zenkit::AnimationDirection::FORWARD);

		CHECK_EQ(script.blends.size(), 17);
		CHECK_EQ(script.blends[0].name, "T_FISTRUNR_2_FISTRUN");
		CHECK_EQ(script.blends[0].next, "S_FISTRUN");
		CHECK_EQ(script.blends[0].blend_in, 0.0f);
		CHECK_EQ(script.blends[0].blend_out, 0.0f);

		CHECK_EQ(script.combinations.size(), 1);
		CHECK_EQ(script.combinations[0].name, "T_LOOK");
		CHECK_EQ(script.combinations[0].layer, 9);
		CHECK_EQ(script.combinations[0].next, "T_LOOK");
		CHECK_EQ(script.combinations[0].blend_in, 0.3f);
		CHECK_EQ(script.combinations[0].blend_out, 0.3f);
		CHECK_EQ(script.combinations[0].flags, zenkit::AnimationFlags::MOVE);
		CHECK_EQ(script.combinations[0].model, "C_LOOK_");
		CHECK_EQ(script.combinations[0].last_frame, 9);

		CHECK_EQ(script.meshes.size(), 2);
		CHECK_EQ(script.meshes[0], "WAR_BODY.ASC");
		CHECK_EQ(script.meshes[1], "WAR_FIRE_BODY.ASC");

		CHECK_EQ(script.animations[0].sfx_ground.size(), 1);
		CHECK_EQ(script.animations[0].sfx_ground[0].frame, 5);
		CHECK_EQ(script.animations[0].sfx_ground[0].name, "SNEAK");

		CHECK_EQ(script.animations[0].sfx.size(), 0);
		CHECK_EQ(script.animations[0].pfx.size(), 0);
		CHECK_EQ(script.animations[0].pfx_stop.size(), 0);
		CHECK_EQ(script.animations[0].morph.size(), 0);
		CHECK_EQ(script.animations[0].events.size(), 0);
		CHECK_EQ(script.animations[0].tremors.size(), 0);
		CHECK_EQ(script.disabled_animations.size(), 0);

		CHECK_EQ(script.animations[47].events.size(), 4);
		CHECK_EQ(script.animations[47].events[3].frame, 0);
		CHECK_EQ(script.animations[47].events[3].type, zenkit::MdsEventType::COMBO_WINDOW);
		CHECK_EQ(script.animations[47].events[3].frames.size(), 2);
		CHECK_EQ(script.animations[47].events[3].frames[0], 8);
		CHECK_EQ(script.animations[47].events[3].frames[1], 15);
	}

	TEST_CASE("ModelScript.load(GOTHIC1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("ModelScript.load(GOTHIC1,BINARY)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("ModelScript.load(GOTHIC2)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("ModelScript.load(GOTHIC2,BINARY)" * doctest::skip()) {
		// TODO: Stub
	}
}
