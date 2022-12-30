// Copyright © 2022 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/vobs/light.hh>
#include <phoenix/vobs/misc.hh>
#include <phoenix/vobs/mob.hh>
#include <phoenix/vobs/sound.hh>
#include <phoenix/vobs/trigger.hh>
#include <phoenix/vobs/vob.hh>
#include <phoenix/vobs/zone.hh>

TEST_SUITE("vobs") {
	static phoenix::archive_object obj;

	TEST_CASE("zCVob(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCVob.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vob>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVob");

		phoenix::vob::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->bbox.min, glm::vec3 {-18966.623f, -236.707687f, 4373.23486f});
		CHECK_EQ(vob->bbox.max, glm::vec3 {-18772.623f, -42.7076874f, 4567.23486f});
		CHECK_EQ(vob->position, glm::vec3 {-18869.623f, -139.707687f, 4470.23486f});
		CHECK_EQ(vob->rotation,
		         glm::mat3x3 {
		             {1, 0, 0},
		             {0, 1, 0},
		             {0, 0, 1},
		         });
		CHECK(vob->show_visual);
		CHECK_EQ(vob->sprite_camera_facing_mode, phoenix::sprite_alignment::none);
		CHECK_FALSE(vob->cd_static);
		CHECK_FALSE(vob->cd_dynamic);
		CHECK_FALSE(vob->vob_static);
		CHECK_EQ(vob->dynamic_shadows, phoenix::shadow_mode::none);
		CHECK_FALSE(vob->physics_enabled);
		CHECK_EQ(vob->anim_mode, phoenix::animation_mode::none);
		CHECK_EQ(vob->bias, 0);
		CHECK_FALSE(vob->ambient);
		CHECK_EQ(vob->anim_strength, 0.0f);
		CHECK_EQ(vob->far_clip_scale, 0.0f);
		CHECK_EQ(vob->preset_name, "");
		CHECK_EQ(vob->vob_name, "");
		CHECK_EQ(vob->visual_name, "FIRE.pfx");
		CHECK_EQ(vob->associated_visual_type, phoenix::visual_type::particle_system);
		CHECK_EQ(vob->visual_decal, std::nullopt);
		CHECK_EQ(vob->saved, std::nullopt);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobAnimate(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCVobAnimate.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::animate>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobAnimate:zCVob");

		phoenix::vobs::animate::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_FALSE(vob->start_on);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneVobFarPlane(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCZoneVobFarPlane.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::zone_far_plane>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneVobFarPlane:zCVob");

		phoenix::vobs::zone_far_plane::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->vob_far_plane_z, 9000.0f);
		CHECK_EQ(vob->inner_range_percentage, 0.699999988f);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneZFog(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCZoneZFog.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::zone_fog>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneZFog:zCVob");

		phoenix::vobs::zone_fog::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->range_center, 8000.0f);
		CHECK_EQ(vob->inner_range_percentage, 0.850000024f);
		CHECK_EQ(vob->color, glm::u8vec4 {120, 120, 120, 20});
		CHECK_FALSE(vob->fade_out_sky);
		CHECK_FALSE(vob->override_color);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobLensFlare(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCVobLensFlare.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::lens_flare>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobLensFlare:zCVob");

		phoenix::vobs::lens_flare::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->fx, "TORCHFX01");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCItem(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCItem.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::item>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCItem:zCVob");

		phoenix::vobs::item::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->instance, "ITMW_1H_AXE_01");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCCSTrigger(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCCSTrigger.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCCSTrigger:zCTrigger:zCVob");

		phoenix::vobs::trigger::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->target, "AMB_PSI_CS003.CS");
		CHECK_EQ(vob->flags, 3);
		CHECK_EQ(vob->filter_flags, 23);
		CHECK_EQ(vob->vob_target, "");
		CHECK_EQ(vob->max_activation_count, -1);
		CHECK_EQ(vob->retrigger_delay_sec, 0.0f);
		CHECK_EQ(vob->damage_threshold, 0.0f);
		CHECK_EQ(vob->fire_delay_sec, 0.0f);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMOB(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCMOB.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMOB:zCVob");

		phoenix::vobs::mob::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->name, "");
		CHECK_EQ(vob->hp, 10);
		CHECK_EQ(vob->damage, 0);
		CHECK_FALSE(vob->movable);
		CHECK_FALSE(vob->takable);
		CHECK_FALSE(vob->focus_override);
		CHECK_EQ(vob->material, phoenix::sound_material::wood);
		CHECK_EQ(vob->visual_destroyed, "");
		CHECK_EQ(vob->owner, "");
		CHECK_EQ(vob->owner_guild, "");
		CHECK_FALSE(vob->destroyed);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobInter(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCMobInter.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_inter>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_inter::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->state, 1);
		CHECK_EQ(vob->target, "OW_ORC_MAINGATE_01");
		CHECK_EQ(vob->item, "");
		CHECK_EQ(vob->condition_function, "MC_ORCCITY_GATE");
		CHECK_EQ(vob->on_state_change_function, "");
		CHECK_FALSE(vob->rewind);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobFire(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCMobFire.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_fire>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobFire:oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_fire::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->slot, "BIP01 FIRE");
		CHECK_EQ(vob->vob_tree, "FIRETREE_MEDIUM.ZEN");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobContainer(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCMobContainer.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_container>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobContainer:oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_container::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_FALSE(vob->locked);
		CHECK_EQ(vob->key, "");
		CHECK_EQ(vob->pick_string, "");
		CHECK_EQ(vob->contents, "");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobDoor(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCMobDoor.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_door>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobDoor:oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_door::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_FALSE(vob->locked);
		CHECK_EQ(vob->key, "");
		CHECK_EQ(vob->pick_string, "");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCPFXControler(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCPFXControler.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::pfx_controller>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCPFXControler:zCVob");

		phoenix::vobs::pfx_controller::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->pfx_name, "CS_miltenfog.PFX");
		CHECK(vob->kill_when_done);
		CHECK(vob->initially_running);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	static const std::vector<float> G1_LIGHT_RANGE_ANIMATION_SCALE {};
	static const std::vector<glm::u8vec4> G1_LIGHT_COLOR_ANIMATION_LIST {
	    glm::u8vec4 {211, 147, 107, 0}, glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {211, 147, 107, 0},
	    glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {225, 197, 100, 0}, glm::u8vec4 {223, 173, 117, 0},
	    glm::u8vec4 {227, 209, 106, 0}, glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {211, 147, 107, 0},
	    glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {225, 197, 100, 0}, glm::u8vec4 {227, 209, 106, 0},
	    glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {211, 147, 107, 0}, glm::u8vec4 {225, 197, 100, 0},
	    glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {225, 197, 100, 0}, glm::u8vec4 {211, 147, 107, 0},
	    glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {227, 209, 106, 0}, glm::u8vec4 {225, 197, 100, 0},
	    glm::u8vec4 {211, 147, 107, 0}, glm::u8vec4 {225, 197, 100, 0}, glm::u8vec4 {223, 173, 117, 0},
	    glm::u8vec4 {225, 197, 100, 0}, glm::u8vec4 {227, 209, 106, 0}, glm::u8vec4 {223, 173, 117, 0},
	    glm::u8vec4 {211, 147, 107, 0}, glm::u8vec4 {223, 173, 117, 0}, glm::u8vec4 {211, 147, 107, 0},
	    glm::u8vec4 {225, 197, 100, 0}, glm::u8vec4 {227, 209, 106, 0}, glm::u8vec4 {223, 173, 117, 0},
	};

	TEST_CASE("zCVobLight(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCVobLight.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::light>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobLight:zCVob");

		phoenix::vobs::light::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->preset, "");
		CHECK_EQ(vob->light_type, phoenix::light_mode::point);
		CHECK_EQ(vob->range, 2000.0f);
		CHECK_EQ(vob->color, glm::u8vec4 {223, 173, 117, 255});
		CHECK_EQ(vob->cone_angle, 0.0f);
		CHECK_FALSE(vob->is_static);
		CHECK_EQ(vob->quality, phoenix::light_quality::low);
		CHECK_EQ(vob->lensflare_fx, "");
		CHECK(vob->on);
		CHECK_EQ(vob->range_animation_scale, G1_LIGHT_RANGE_ANIMATION_SCALE);
		CHECK_EQ(vob->range_animation_fps, 0.0f);
		CHECK(vob->range_animation_smooth);
		CHECK_EQ(vob->color_animation_list, G1_LIGHT_COLOR_ANIMATION_LIST);
		CHECK_EQ(vob->color_animation_fps, 11.0000067f);
		CHECK_FALSE(vob->color_animation_smooth);
		CHECK(vob->can_move);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSound(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCVobSound.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::sound>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSound:zCVob");

		phoenix::vobs::sound::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->volume, 100.0f);
		CHECK_EQ(vob->mode, phoenix::sound_mode::loop);
		CHECK_EQ(vob->random_delay, 5.0f);
		CHECK_EQ(vob->random_delay_var, 2.0f);
		CHECK(vob->initially_playing);
		CHECK_FALSE(vob->ambient3d);
		CHECK_FALSE(vob->obstruction);
		CHECK_EQ(vob->cone_angle, 0.0f);
		CHECK_EQ(vob->volume_type, phoenix::sound_trigger_volume::spherical);
		CHECK_EQ(vob->radius, 1500.0f);
		CHECK_EQ(vob->sound_name, "FIRE_MEDIUM");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSoundDaytime(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCVobSoundDaytime.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::sound_daytime>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSoundDaytime:zCVobSound:zCVob");

		phoenix::vobs::sound_daytime::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->start_time, 8.0f);
		CHECK_EQ(vob->end_time, 18.0f);
		CHECK_EQ(vob->sound_name2, "INSECTS_AND_NIGHTINGALES");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCZoneMusic(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCZoneMusic.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::zone_music>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCZoneMusic:zCVob");

		phoenix::vobs::zone_music::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK(vob->enabled);
		CHECK_EQ(vob->priority, 1);
		CHECK_FALSE(vob->ellipsoid);
		CHECK_EQ(vob->reverb, -9.0f);
		CHECK_EQ(vob->volume, 1.0f);
		CHECK(vob->loop);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	static const std::vector<phoenix::vobs::trigger_list::target> G1_TRIGGER_LIST_TARGETS {
	    {"EVT_CASTLE_PLATE", 0.0f},
	    {"EVT_CASTLE_FLOOR_5", 6.0f},
	    {"EVT_CASTLE_FLOOR_4", 2.0f},
	    {"EVT_CASTLE_FLOOR_3", 1.0f},
	    {"EVT_CASTLE_FLOOR_2", 0.0f},
	    {"EVT_CASTLE_FLOOR_1", 4.0f},
	};

	TEST_CASE("zCTriggerList(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCTriggerList.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_list>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerList:zCTrigger:zCVob");

		phoenix::vobs::trigger_list::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->mode, phoenix::trigger_batch_mode::all);
		CHECK_EQ(vob->targets, G1_TRIGGER_LIST_TARGETS);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTriggerScript(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCTriggerScript.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_script>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerScript:zCTrigger:zCVob");

		phoenix::vobs::trigger_script::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->function, "ON_NC_GATE_TRIGGER");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	static const std::vector<phoenix::animation_sample> G1_MOVER_KEYFRAMES {
	    {glm::vec3 {-23325.1992f, 3438.91333f, -21834.9473f},
	     glm::quat {0.105035283f, 0.091305837f, 0.747364759f, 0.649674594f}},
	    {glm::vec3 {-23325.1543f, 3438.91333f, -21844.3672f},
	     glm::quat {-0.00970827043f, 0.138834357f, -0.0690778494f, 0.98785609f}},
	};

	TEST_CASE("zCMover(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/zCMover.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_mover>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCMover:zCTrigger:zCVob");

		phoenix::vobs::trigger_mover::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->behavior, phoenix::mover_behavior::trigger_control);
		CHECK_EQ(vob->touch_blocker_damage, 0.0f);
		CHECK_EQ(vob->stay_open_time_sec, 2.0f);
		CHECK_FALSE(vob->locked);
		CHECK_FALSE(vob->auto_link);
		CHECK_FALSE(vob->auto_rotate);
		CHECK_EQ(vob->speed, 0.00200023991f);
		CHECK_EQ(vob->lerp_mode, phoenix::mover_lerp_mode::curve);
		CHECK_EQ(vob->speed_mode, phoenix::mover_speed_mode::slow_start_end);
		CHECK_EQ(vob->keyframes, G1_MOVER_KEYFRAMES);
		CHECK_EQ(vob->sfx_open_start, "GATE_START");
		CHECK_EQ(vob->sfx_open_end, "GATE_STOP");
		CHECK_EQ(vob->sfx_transitioning, "GATE_LOOP");
		CHECK_EQ(vob->sfx_close_start, "GATE_START");
		CHECK_EQ(vob->sfx_close_end, "GATE_STOP");
		CHECK_EQ(vob->sfx_lock, "");
		CHECK_EQ(vob->sfx_unlock, "");
		CHECK_EQ(vob->sfx_use_locked, "");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTriggerChangeLevel(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/G1/VOb/oCTriggerChangeLevel.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_change_level>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerChangeLevel:zCTrigger:zCVob");

		phoenix::vobs::trigger_change_level::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->level_name, "ORCTEMPEL.ZEN");
		CHECK_EQ(vob->start_vob, "ENTRANCE_ORCTEMPLE_SURFACE");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCNpc(parse:g1)") {
		// TODO: Stub
	}
}
