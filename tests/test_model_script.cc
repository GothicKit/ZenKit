// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "doctest/doctest.h"
#include <phoenix/model_script.hh>

TEST_SUITE("model script") {
	TEST_CASE("text") {
		auto buf = phoenix::buffer::open("./samples/waran.mds");
		auto script = phoenix::model_script::parse(buf);

		CHECK(script.skeleton.disable_mesh == true);
		CHECK(script.skeleton.name == "TestModelMesh.asc");

		CHECK(script.animations.size() == 2);
		CHECK(script.animations[0].name == "aniName1");
		CHECK(script.animations[0].layer == 111);
		CHECK(script.animations[0].next == "aniNext1");
		CHECK(script.animations[0].blend_in == 4.2f);
		CHECK(script.animations[0].blend_out == 0.5f);
		CHECK(script.animations[0].flags == (phoenix::mds::af_move | phoenix::mds::af_idle));
		CHECK(script.animations[0].model == "aniModel1");
		CHECK(script.animations[0].direction == phoenix::mds::animation_direction::forward);
		CHECK(script.animations[0].first_frame == 221);
		CHECK(script.animations[0].last_frame == 331);
		CHECK(script.animations[0].fps == 25);
		CHECK(script.animations[0].speed == 0);
		CHECK(script.animations[0].collision_volume_scale == 0.2f);
		CHECK(script.animations[0].pfx.empty());
		CHECK(script.animations[0].pfx_stop.empty());
		CHECK(script.animations[0].sfx.empty());
		CHECK(script.animations[0].sfx_ground.empty());
		CHECK(script.animations[0].events.empty());
		CHECK(script.animations[0].morph.empty());
		CHECK(script.animations[0].tremors.empty());

		CHECK(script.animations[1].name == "aniName2");
		CHECK(script.animations[1].layer == 112);
		CHECK(script.animations[1].next == "aniNext2");
		CHECK(script.animations[1].blend_in == 9.0f);
		CHECK(script.animations[1].blend_out == 0.0f);
		CHECK(script.animations[1].flags == phoenix::mds::af_move);
		CHECK(script.animations[1].model == "aniModel2");
		CHECK(script.animations[1].direction == phoenix::mds::animation_direction::backward);
		CHECK(script.animations[1].first_frame == 222);
		CHECK(script.animations[1].last_frame == 332);
		CHECK(script.animations[1].fps == 25);
		CHECK(script.animations[1].speed == 0);
		CHECK(script.animations[1].collision_volume_scale == 1.0f);

		CHECK(script.animations[1].events.size() == 3);
		CHECK(script.animations[1].events[0].frame == 0);
		CHECK(script.animations[1].events[0].type == phoenix::mds::event_tag_type::drop_torch);
		CHECK(script.animations[1].events[0].attached == true);
		CHECK(script.animations[1].events[1].frame == 1);
		CHECK(script.animations[1].events[1].type == phoenix::mds::event_tag_type::window);
		CHECK(script.animations[1].events[1].frames == std::vector<int32_t> {1, 2, 3, 4, 5});
		CHECK(script.animations[1].events[1].attached == false);
		CHECK(script.animations[1].events[2].frame == 2);
		CHECK(script.animations[1].events[2].type == phoenix::mds::event_tag_type::create_item);
		CHECK(script.animations[1].events[2].slot == "eventSlot");
		CHECK(script.animations[1].events[2].item == "eventItem");
		CHECK(script.animations[1].events[2].attached == false);

		CHECK(script.animations[1].sfx.size() == 2);
		CHECK(script.animations[1].sfx[0].frame == 3);
		CHECK(script.animations[1].sfx[0].name == "sfxName1");
		CHECK(script.animations[1].sfx[0].empty_slot == true);
		CHECK(script.animations[1].sfx[1].frame == 4);
		CHECK(script.animations[1].sfx[1].name == "sfxName2");
		CHECK(script.animations[1].sfx[1].range == 67.4f);
		CHECK(script.animations[1].sfx[1].empty_slot == false);

		CHECK(script.animations[1].sfx_ground.size() == 1);
		CHECK(script.animations[1].sfx_ground[0].frame == 5);
		CHECK(script.animations[1].sfx_ground[0].name == "sfxGrndName");
		CHECK(script.animations[1].sfx_ground[0].empty_slot == false);

		CHECK(script.animations[1].pfx.size() == 2);
		CHECK(script.animations[1].pfx[0].frame == 6);
		CHECK(script.animations[1].pfx[0].index == 0);
		CHECK(script.animations[1].pfx[0].name == "pfxName1");
		CHECK(script.animations[1].pfx[0].position == "pfxPosition1");
		CHECK(script.animations[1].pfx[0].attached == true);
		CHECK(script.animations[1].pfx[1].frame == 7);
		CHECK(script.animations[1].pfx[1].index == 991);
		CHECK(script.animations[1].pfx[1].name == "pfxName2");
		CHECK(script.animations[1].pfx[1].position == "pfxPosition2");
		CHECK(script.animations[1].pfx[1].attached == false);

		CHECK(script.animations[1].pfx_stop.size() == 1);
		CHECK(script.animations[1].pfx_stop[0].frame == 8);
		CHECK(script.animations[1].pfx_stop[0].index == 992);

		CHECK(script.animations[1].morph.size() == 2);
		CHECK(script.animations[1].morph[0].frame == 9);
		CHECK(script.animations[1].morph[0].animation == "mmAni1");
		CHECK(script.animations[1].morph[0].node.empty());
		CHECK(script.animations[1].morph[1].frame == 10);
		CHECK(script.animations[1].morph[1].animation == "mmAni2");
		CHECK(script.animations[1].morph[1].node == "mmNode");

		CHECK(script.animations[1].tremors.size() == 1);
		CHECK(script.animations[1].tremors[0].frame == 11);
		CHECK(script.animations[1].tremors[0].field1 == 881);
		CHECK(script.animations[1].tremors[0].field2 == 882);
		CHECK(script.animations[1].tremors[0].field3 == 883);
		CHECK(script.animations[1].tremors[0].field4 == 884);

		CHECK(script.blends.size() == 3);
		CHECK(script.blends[0].name == "blendName1");
		CHECK(script.blends[0].next == "blendNext1");
		CHECK(script.blends[1].name == "blendName2");
		CHECK(script.blends[1].next == "blendNext2");
		CHECK(script.blends[2].name == "blendName3");
		CHECK(script.blends[2].next == "blendNext3");
		CHECK(script.blends[2].blend_in == 223.1f);
		CHECK(script.blends[2].blend_out == 333.1f);

		CHECK(script.aliases.size() == 2);
		CHECK(script.aliases[0].name == "aliasName1");
		CHECK(script.aliases[0].layer == 114);
		CHECK(script.aliases[0].next == "aliasNext1");
		CHECK(script.aliases[0].blend_in == 100.1f);
		CHECK(script.aliases[0].blend_out == 200.2f);
		CHECK(script.aliases[0].flags == (phoenix::mds::af_rotate | phoenix::mds::af_queue));
		CHECK(script.aliases[0].alias == "aliasAlias1");
		CHECK(script.aliases[0].direction == phoenix::mds::animation_direction::forward);
		CHECK(script.aliases[1].name == "aliasName2");
		CHECK(script.aliases[1].layer == 115);
		CHECK(script.aliases[1].next == "aliasNext2");
		CHECK(script.aliases[1].blend_in == 101.1f);
		CHECK(script.aliases[1].blend_out == 201.2f);
		CHECK(script.aliases[1].flags == phoenix::mds::af_fly);
		CHECK(script.aliases[1].alias == "aliasAlias2");
		CHECK(script.aliases[1].direction == phoenix::mds::animation_direction::backward);

		CHECK(script.combinations.size() == 2);
		CHECK(script.combinations[0].name == "combName1");
		CHECK(script.combinations[0].layer == 116);
		CHECK(script.combinations[0].next == "combNext1");
		CHECK(script.combinations[0].blend_in == 102.1f);
		CHECK(script.combinations[0].blend_out == 202.2f);
		CHECK(script.combinations[0].flags == phoenix::mds::af_move);
		CHECK(script.combinations[0].model == "combModel1");
		CHECK(script.combinations[0].last_frame == 226);
		CHECK(script.combinations[1].name == "combName2");
		CHECK(script.combinations[1].layer == 117);
		CHECK(script.combinations[1].next == "combNext2");
		CHECK(script.combinations[1].blend_in == 103.1f);
		CHECK(script.combinations[1].blend_out == 203.2f);
		CHECK(script.combinations[1].flags == phoenix::mds::af_idle);
		CHECK(script.combinations[1].model == "combModel2");
		CHECK(script.combinations[1].last_frame == 227);

		CHECK(script.disabled_animations.size() == 2);
		CHECK(script.disabled_animations[0] == "disable1");
		CHECK(script.disabled_animations[1] == "disable2");

		CHECK(script.model_tags.size() == 2);
		CHECK(script.model_tags[0].bone == "tag1");
		CHECK(script.model_tags[1].bone == "tag2");
	}

	TEST_CASE("binary") {
		auto buf = phoenix::buffer::open("./samples/waran.msb");
		auto script = phoenix::model_script::parse_binary(buf);

		CHECK(script.skeleton.disable_mesh == true);
		CHECK(script.skeleton.name == "WAR_BODY");

		CHECK(script.animations.size() == 62);
		CHECK(script.animations[0].name == "T_FISTRUN_2_FISTRUNL");
		CHECK(script.animations[0].layer == 1);
		CHECK(script.animations[0].next == "S_FISTRUNL");
		CHECK(script.animations[0].blend_in == 0);
		CHECK(script.animations[0].blend_out == 0);
		CHECK(script.animations[0].flags == phoenix::mds::af_move);
		CHECK(script.animations[0].model == "WARAN_RUN_KM01.ASC");
		CHECK(script.animations[0].direction == phoenix::mds::animation_direction::forward);
		CHECK(script.animations[0].first_frame == 1);
		CHECK(script.animations[0].last_frame == 8);
		CHECK(script.animations[0].fps == 25);
		CHECK(script.animations[0].speed == 0); // default
		CHECK(script.animations[0].collision_volume_scale == 1);// default

		CHECK(script.aliases.size() == 38);
		CHECK(script.aliases[0].name == "S_FISTRUN");
		CHECK(script.aliases[0].layer == 1);
		CHECK(script.aliases[0].next == "S_FISTRUN");
		CHECK(script.aliases[0].blend_in == 0);
		CHECK(script.aliases[0].blend_out == 0);
		CHECK(script.aliases[0].flags == (phoenix::mds::af_move | phoenix::mds::af_idle));
		CHECK(script.aliases[0].alias == "S_FISTWALK");
		CHECK(script.aliases[0].direction == phoenix::mds::animation_direction::forward);

		CHECK(script.blends.size() == 17);
		CHECK(script.blends[0].name == "T_FISTRUNR_2_FISTRUN");
		CHECK(script.blends[0].next == "S_FISTRUN");
		CHECK(script.blends[0].blend_in == 0);
		CHECK(script.blends[0].blend_out == 0);

		CHECK(script.combinations.size() == 1);
		CHECK(script.combinations[0].name == "T_LOOK");
		CHECK(script.combinations[0].layer == 9);
		CHECK(script.combinations[0].next == "T_LOOK");
		CHECK(script.combinations[0].blend_in == 0.3f);
		CHECK(script.combinations[0].blend_out == 0.3f);
		CHECK(script.combinations[0].flags == phoenix::mds::af_move);
		CHECK(script.combinations[0].model == "C_LOOK_");
		CHECK(script.combinations[0].last_frame == 9);

		CHECK(script.meshes.size() == 2);
		CHECK(script.meshes[0] == "WAR_BODY.ASC");
		CHECK(script.meshes[1] == "WAR_FIRE_BODY.ASC");

		CHECK(script.animations[0].sfx_ground.size() == 1);
		CHECK(script.animations[0].sfx_ground[0].frame == 5);
		CHECK(script.animations[0].sfx_ground[0].name == "SNEAK");

		CHECK(script.animations[0].sfx.size() == 0);
		CHECK(script.animations[0].pfx.size() == 0);
		CHECK(script.animations[0].pfx_stop.size() == 0);
		CHECK(script.animations[0].morph.size() == 0);
		CHECK(script.animations[0].events.size() == 0);
		CHECK(script.animations[0].tremors.size() == 0);
		CHECK(script.disabled_animations.size() == 0);

		CHECK(script.animations[47].events.size() == 4);
		CHECK(script.animations[47].events[3].frame == 0);
		CHECK(script.animations[47].events[3].type == phoenix::mds::event_tag_type::window);
		CHECK(script.animations[47].events[3].frames.size() == 2);
		CHECK(script.animations[47].events[3].frames[0] == 8);
		CHECK(script.animations[47].events[3].frames[1] == 15);
	}
}
