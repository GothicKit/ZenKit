// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "doctest/doctest.h"
#include <phoenix/model_script.hh>

TEST_SUITE("model script") {
	TEST_CASE("binary") {
		auto buf = phoenix::buffer::open("./samples/waran.msb");
		auto script = phoenix::model_script::parse_binary(buf);

		CHECK(script.mesh_and_tree.has_value());
		CHECK((*script.mesh_and_tree).disable == true);
		CHECK((*script.mesh_and_tree).name == "WAR_BODY");

		CHECK(script.animation.has_value());
		CHECK((*script.animation).name == "S_FREEZE_VICTIM");
		CHECK((*script.animation).layer == 1);
		CHECK((*script.animation).next == "S_FREEZE_VICTIM");
		CHECK((*script.animation).blend_in == 0);
		CHECK((*script.animation).blend_out == 0);
		CHECK((*script.animation).flags == phoenix::model_script_animation_flags::move);
		CHECK((*script.animation).model == "WARAN_STUMBLEB_KM01.ASC");
		CHECK((*script.animation).direction == phoenix::model_script_animation_direction::forward);
		CHECK((*script.animation).first_frame == 4);
		CHECK((*script.animation).last_frame == 4);
		CHECK((*script.animation).fps == 25);
		CHECK((*script.animation).speed == 0);
		CHECK((*script.animation).collision_volume_scale == 1);

		CHECK(script.alias.has_value());
		CHECK((*script.alias).name == "T_VICTIM_SLE_2_STAND");
		CHECK((*script.alias).layer == 1);
		CHECK((*script.alias).next.empty());
		CHECK((*script.alias).blend_in == 0);
		CHECK((*script.alias).blend_out == 0.1f);
		CHECK((*script.alias).flags == phoenix::model_script_animation_flags::move);
		CHECK((*script.alias).alias == "T_SLEEP_2_STAND");
		CHECK((*script.alias).direction == phoenix::model_script_animation_direction::forward);

		CHECK(script.blend.has_value());
		CHECK((*script.blend).name == "T_DIVE_2_SWIM");
		CHECK((*script.blend).next == "S_SWIM");
		CHECK((*script.blend).blend_in == 0.2f);
		CHECK((*script.blend).blend_out == 0.2f);

		CHECK(script.combination.has_value());
		CHECK((*script.combination).name == "T_LOOK");
		CHECK((*script.combination).layer == 9);
		CHECK((*script.combination).next == "T_LOOK");
		CHECK((*script.combination).blend_in == 0.3f);
		CHECK((*script.combination).blend_out == 0.3f);
		CHECK((*script.combination).flags == phoenix::model_script_animation_flags::move);
		CHECK((*script.combination).model == "C_LOOK_");
		CHECK((*script.combination).last_frame == 9);

		CHECK(script.meshes.size() == 2);
		CHECK(script.meshes[0] == "WAR_BODY.ASC");
		CHECK(script.meshes[1] == "WAR_FIRE_BODY.ASC");

		CHECK(script.sfx.size() == 47);
		CHECK(script.sfx[0].frame == 5);
		CHECK(script.sfx[0].name == "SNEAK");
		CHECK(script.sfx[0].range == 1000);
		CHECK(script.sfx[0].empty_slot == true);
		CHECK(script.sfx[46].frame == 1);
		CHECK(script.sfx[46].name == "WAR_WARN");
		CHECK(script.sfx[46].range == 2500);
		CHECK(script.sfx[46].empty_slot == false);

		CHECK(script.pfx.size() == 0);
		CHECK(script.pfx_stop.size() == 0);
		CHECK(script.morph_animations.size() == 0);
		CHECK(script.model_tag.size() == 0);

		CHECK(script.event_tag.size() == 6);
		CHECK(script.event_tag[0].frame == 0);
		CHECK(script.event_tag[0].type == phoenix::model_script_event_type::hit_limb);
		CHECK(script.event_tag[0].slot.empty());
		CHECK(script.event_tag[0].slot2.empty());
		CHECK(script.event_tag[0].item.empty());
		CHECK(script.event_tag[0].frames.empty());
		CHECK(script.event_tag[0].fight_mode == phoenix::model_script_fight_mode::none);

		CHECK(script.event_tag[3].frame == 0);
		CHECK(script.event_tag[3].type == phoenix::model_script_event_type::window);
		CHECK(script.event_tag[3].slot.empty());
		CHECK(script.event_tag[3].slot2.empty());
		CHECK(script.event_tag[3].item.empty());
		CHECK(script.event_tag[3].frames.size() == 1);
		CHECK(script.event_tag[3].frames[0] == 8);
		CHECK(script.event_tag[3].fight_mode == phoenix::model_script_fight_mode::none);

		CHECK(!script.disable.has_value());
	}
}
