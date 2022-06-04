// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "doctest/doctest.h"
#include <phoenix/model_script.hh>

TEST_SUITE("model script") {
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

		CHECK(script.animations[0].sfx.size() == 1);
		CHECK(script.animations[0].sfx[0].frame == 5);
		CHECK(script.animations[0].sfx[0].name == "SNEAK");
		CHECK(script.animations[0].sfx[0].range == 1000);
		CHECK(script.animations[0].sfx[0].empty_slot == true);

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
