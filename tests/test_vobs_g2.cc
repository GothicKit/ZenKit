// Copyright © 2022 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/vobs/camera.hh>
#include <phoenix/vobs/light.hh>
#include <phoenix/vobs/misc.hh>
#include <phoenix/vobs/mob.hh>
#include <phoenix/vobs/sound.hh>
#include <phoenix/vobs/trigger.hh>
#include <phoenix/vobs/vob.hh>
#include <phoenix/vobs/zone.hh>

TEST_SUITE("vobs") {
	static phoenix::archive_object obj;

	TEST_CASE("zCVob(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCVob.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vob>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVob");

		phoenix::vob::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->bbox.min, glm::vec3 {30897.1035f, 4760.24951f, -14865.5723f});
		CHECK_EQ(vob->bbox.max, glm::vec3 {30929.8301f, 4836.17529f, -14817.3135f});
		CHECK_EQ(vob->position, glm::vec3 {30913.4668f, 4798.9751f, -14841.4434f});
		CHECK_EQ(vob->rotation,
		         glm::mat3x3 {
		             {0.920505285f, 0, -0.390731275f},
		             {0, 1, 0},
		             {0.390731275f, 0, 0.920505285f},
		         });
		CHECK(vob->show_visual);
		CHECK_EQ(vob->sprite_camera_facing_mode, phoenix::sprite_alignment::none);
		CHECK_FALSE(vob->cd_static);
		CHECK_FALSE(vob->cd_dynamic);
		CHECK(vob->vob_static);
		CHECK_EQ(vob->dynamic_shadows, phoenix::shadow_mode::none);
		CHECK_FALSE(vob->physics_enabled);
		CHECK_EQ(vob->anim_mode, phoenix::animation_mode::none);
		CHECK_EQ(vob->bias, 0);
		CHECK_FALSE(vob->ambient);
		CHECK_EQ(vob->anim_strength, 0.0f);
		CHECK_EQ(vob->far_clip_scale, 1.0f);
		CHECK_EQ(vob->preset_name, "");
		CHECK_EQ(vob->vob_name, "");
		CHECK_EQ(vob->visual_name, "OW_MISC_WALL_TORCH_01.3DS");
		CHECK_EQ(vob->associated_visual_type, phoenix::visual_type::proto_mesh);
		CHECK_EQ(vob->visual_decal, std::nullopt);
		CHECK_EQ(vob->saved, std::nullopt);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCCSCamera(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCCSCamera.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::cs_camera>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCCSCamera:zCVob");

		phoenix::vobs::cs_camera::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->trajectory_for, phoenix::camera_trajectory::world);
		CHECK_EQ(vob->target_trajectory_for, phoenix::camera_trajectory::world);
		CHECK_EQ(vob->loop_mode, phoenix::camera_loop::none);
		CHECK_EQ(vob->lerp_mode, phoenix::camera_lerp_mode::path);
		CHECK_FALSE(vob->ignore_for_vob_rotation);
		CHECK_FALSE(vob->ignore_for_vob_rotation_target);
		CHECK_FALSE(vob->adapt);
		CHECK_FALSE(vob->ease_first);
		CHECK_FALSE(vob->ease_last);
		CHECK_EQ(vob->total_duration, 20.0f);
		CHECK_EQ(vob->auto_focus_vob, "");
		CHECK_FALSE(vob->auto_player_movable);
		CHECK_FALSE(vob->auto_untrigger_last);
		CHECK_EQ(vob->auto_untrigger_last_delay, 0.0f);
		CHECK_EQ(vob->position_count, 2);
		CHECK_EQ(vob->target_count, 1);

		const auto& frames = vob->frames;
		CHECK_EQ(frames[0]->time, 0.0f);
		CHECK_EQ(frames[0]->roll_angle, 0.0f);
		CHECK_EQ(frames[0]->fov_scale, 1.0f);
		CHECK_EQ(frames[0]->motion_type, phoenix::camera_motion::slow);
		CHECK_EQ(frames[0]->motion_type_fov, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[0]->motion_type_roll, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[0]->motion_type_time_scale, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[0]->tension, 0.0f);
		CHECK_EQ(frames[0]->cam_bias, 0.0f);
		CHECK_EQ(frames[0]->continuity, 0.0f);
		CHECK_EQ(frames[0]->time_scale, 1.0f);
		CHECK_FALSE(frames[0]->time_fixed);
		CHECK_EQ(frames[0]->original_pose,
		         glm::mat4x4 {
		             {0.202226311f, 3.00647909e-11f, -0.979338825f, 0},
		             {0.00805913191f, 0.999966145f, 0.00166415179f, 0.0f},
		             {0.979305685f, -0.00822915602f, 0.202219456f, 0.0f},
		             {81815.7656f, 3905.95044f, 29227.1875f, 1.0f},
		         });

		CHECK_EQ(frames[1]->time, 20.0f);
		CHECK_EQ(frames[1]->roll_angle, 0.0f);
		CHECK_EQ(frames[1]->fov_scale, 1.0f);
		CHECK_EQ(frames[1]->motion_type, phoenix::camera_motion::slow);
		CHECK_EQ(frames[1]->motion_type_fov, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[1]->motion_type_roll, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[1]->motion_type_time_scale, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[1]->tension, 0.0f);
		CHECK_EQ(frames[1]->cam_bias, 0.0f);
		CHECK_EQ(frames[1]->continuity, 0.0f);
		CHECK_EQ(frames[1]->time_scale, 1.0f);
		CHECK_FALSE(frames[1]->time_fixed);
		CHECK_EQ(frames[1]->original_pose,
		         glm::mat4x4 {
		             {1.0f, 0.0f, -0.0f, 0.0f},
		             {-0.0f, 1.0f, -0.0f, 0.0f},
		             {0.0f, 0.0f, 1.0f, 0.0f},
		             {82642.8984f, 3977.64844f, 28358.457f, 1.0f},
		         });

		CHECK_EQ(frames[2]->time, 0.0f);
		CHECK_EQ(frames[2]->roll_angle, 0.0f);
		CHECK_EQ(frames[2]->fov_scale, 1.0f);
		CHECK_EQ(frames[2]->motion_type, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[2]->motion_type_fov, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[2]->motion_type_roll, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[2]->motion_type_time_scale, phoenix::camera_motion::smooth);
		CHECK_EQ(frames[2]->tension, 0.0f);
		CHECK_EQ(frames[2]->cam_bias, 0.0f);
		CHECK_EQ(frames[2]->continuity, 0.0f);
		CHECK_EQ(frames[2]->time_scale, 1.0f);
		CHECK_FALSE(frames[2]->time_fixed);
		CHECK_EQ(frames[2]->original_pose,
		         glm::mat4x4 {
		             {1.0f, 0.0f, 0.0f, 0.0f},
		             {0.0f, 1.0f, 0.0f, 0.0f},
		             {0.0f, 0.0f, 1.0f, 0.0f},
		             {82642.8984f, 3899.0f, 29397.9844f, 1.0f},
		         });

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobAnimate(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCVobAnimate.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::animate>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobAnimate:zCVob");

		phoenix::vobs::animate::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK(vob->start_on);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneVobFarPlane(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCZoneVobFarPlane.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::zone_far_plane>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneVobFarPlane:zCVob");

		phoenix::vobs::zone_far_plane::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->vob_far_plane_z, 6500.0f);
		CHECK_EQ(vob->inner_range_percentage, 0.699999988f);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneZFog(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCZoneZFog.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::zone_fog>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneZFog:zCVob");

		phoenix::vobs::zone_fog::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->range_center, 16000.0f);
		CHECK_EQ(vob->inner_range_percentage, 0.699999988f);
		CHECK_EQ(vob->color, glm::u8vec4 {120, 120, 120, 255});
		CHECK_FALSE(vob->fade_out_sky);
		CHECK_FALSE(vob->override_color);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCItem(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCItem.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::item>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCItem:zCVob");

		phoenix::vobs::item::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->instance, "ITPL_BLUEPLANT");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCTrigger(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCTrigger.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTrigger:zCVob");

		phoenix::vobs::trigger::parse(*vob, *ar, phoenix::game_version::gothic_1);

		CHECK_EQ(vob->target, "");
		CHECK_EQ(vob->flags, 3);
		CHECK_EQ(vob->filter_flags, 0);
		CHECK_EQ(vob->vob_target, "");
		CHECK_EQ(vob->max_activation_count, -1);
		CHECK_EQ(vob->retrigger_delay_sec, 0.0f);
		CHECK_EQ(vob->damage_threshold, 0.0f);
		CHECK_EQ(vob->fire_delay_sec, 0.0f);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMOB(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCMOB.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMOB:zCVob");

		phoenix::vobs::mob::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->name, "MOBNAME_GRAVE_18");
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

	TEST_CASE("oCMobInter(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCMobInter.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_inter>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_inter::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->state, 1);
		CHECK_EQ(vob->target, "");
		CHECK_EQ(vob->item, "");
		CHECK_EQ(vob->condition_function, "");
		CHECK_EQ(vob->on_state_change_function, "PRAYIDOL");
		CHECK_FALSE(vob->rewind);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobFire(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCMobFire.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_fire>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobFire:oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_fire::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->slot, "BIP01 FIRE");
		CHECK_EQ(vob->vob_tree, "FIRETREE_LAMP.ZEN");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobContainer(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCMobContainer.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_container>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobContainer:oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_container::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_FALSE(vob->locked);
		CHECK_EQ(vob->key, "");
		CHECK_EQ(vob->pick_string, "");
		CHECK_EQ(vob->contents, "ItMi_Gold:35");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobDoor(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCMobDoor.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::mob_door>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobDoor:oCMobInter:oCMOB:zCVob");

		phoenix::vobs::mob_door::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_FALSE(vob->locked);
		CHECK_EQ(vob->key, "");
		CHECK_EQ(vob->pick_string, "");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCPFXControler(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCPFXControler.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::pfx_controller>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCPFXControler:zCVob");

		phoenix::vobs::pfx_controller::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->pfx_name, "STARGATE_EDGES.PFX");
		CHECK_FALSE(vob->kill_when_done);
		CHECK_FALSE(vob->initially_running);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	static const std::vector<float> G2_LIGHT_RANGE_ANIMATION_SCALE {};
	static const std::vector<glm::u8vec4> G2_LIGHT_COLOR_ANIMATION_LIST {};

	TEST_CASE("zCVobLight(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCVobLight.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::light>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobLight:zCVob");

		phoenix::vobs::light::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->preset, "NW_STANDART_FIRE_STATIC");
		CHECK_EQ(vob->light_type, phoenix::light_mode::point);
		CHECK_EQ(vob->range, 400.0f);
		CHECK_EQ(vob->color, glm::u8vec4 {100, 71, 60, 255});
		CHECK_EQ(vob->cone_angle, 0.0f);
		CHECK(vob->is_static);
		CHECK_EQ(vob->quality, phoenix::light_quality::low);
		CHECK_EQ(vob->lensflare_fx, "");
		CHECK_FALSE(vob->on);
		CHECK_EQ(vob->range_animation_scale, G2_LIGHT_RANGE_ANIMATION_SCALE);
		CHECK_EQ(vob->range_animation_fps, 0.0f);
		CHECK(vob->range_animation_smooth);
		CHECK_EQ(vob->color_animation_list, G2_LIGHT_COLOR_ANIMATION_LIST);
		CHECK_EQ(vob->color_animation_fps, 0.0f);
		CHECK(vob->color_animation_smooth);
		CHECK(vob->can_move);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSound(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCVobSound.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::sound>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSound:zCVob");

		phoenix::vobs::sound::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->volume, 80.0f);
		CHECK_EQ(vob->mode, phoenix::sound_mode::random);
		CHECK_EQ(vob->random_delay, 30.0f);
		CHECK_EQ(vob->random_delay_var, 20.0f);
		CHECK(vob->initially_playing);
		CHECK_FALSE(vob->ambient3d);
		CHECK_FALSE(vob->obstruction);
		CHECK_EQ(vob->cone_angle, 0.0f);
		CHECK_EQ(vob->volume_type, phoenix::sound_trigger_volume::spherical);
		CHECK_EQ(vob->radius, 3000.0f);
		CHECK_EQ(vob->sound_name, "OW_CROW");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSoundDaytime(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCVobSoundDaytime.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::sound_daytime>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSoundDaytime:zCVobSound:zCVob");

		phoenix::vobs::sound_daytime::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->start_time, 5.0f);
		CHECK_EQ(vob->end_time, 21.0f);
		CHECK_EQ(vob->sound_name2, "InsectsFrogs_Night");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCZoneMusic(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCZoneMusic.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::zone_music>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCZoneMusic:zCVob");

		phoenix::vobs::zone_music::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK(vob->enabled);
		CHECK_EQ(vob->priority, 1);
		CHECK_FALSE(vob->ellipsoid);
		CHECK_EQ(vob->reverb, -3.2190001f);
		CHECK_EQ(vob->volume, 1.0f);
		CHECK(vob->loop);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCMessageFilter(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCMessageFilter.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::message_filter>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCMessageFilter:zCVob");

		phoenix::vobs::message_filter::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->target, "EVT_ADDON_TROLLPORTAL_CAMERA_01");
		CHECK_EQ(vob->on_trigger, phoenix::message_filter_action::untrigger);
		CHECK_EQ(vob->on_untrigger, phoenix::message_filter_action::untrigger);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	static const std::vector<std::string> G2_CODE_MASTER_SLAVES {
	    "EVT_ORNAMENT_SWITCH_BIGFARM_01",
	    "EVT_ORNAMENT_SWITCH_BIGFARM_02",
	    "EVT_ORNAMENT_SWITCH_BIGFARM_03",
	};

	TEST_CASE("zCCodeMaster(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCCodeMaster.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::code_master>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCCodeMaster:zCVob");

		phoenix::vobs::code_master::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->target, "EVT_ORNAMENT_TRIGGER_BIGFARM_01");
		CHECK_FALSE(vob->ordered);
		CHECK_FALSE(vob->first_false_is_failure);
		CHECK_EQ(vob->failure_target, "");
		CHECK_FALSE(vob->untriggered_cancels);
		CHECK_EQ(vob->slaves, G2_CODE_MASTER_SLAVES);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	static const std::vector<phoenix::vobs::trigger_list::target> G2_TRIGGER_LIST_TARGETS {
	    {"EVT_ADDON_TROLLPORTAL_MASTERTRIGGERLIST_ALPHA_01", 0.0f},
	    {"EVT_ADDON_TROLLPORTAL_TRIGGERSCRIPT_01", 0.0f},
	};

	TEST_CASE("zCTriggerList(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCTriggerList.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_list>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerList:zCTrigger:zCVob");

		phoenix::vobs::trigger_list::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->mode, phoenix::trigger_batch_mode::all);
		CHECK_EQ(vob->targets, G2_TRIGGER_LIST_TARGETS);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTriggerScript(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCTriggerScript.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_script>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerScript:zCTrigger:zCVob");

		phoenix::vobs::trigger_script::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->function, "EVT_CAVALORNSGOBBOS_FUNC");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	static const std::vector<phoenix::animation_sample> G1_MOVER_KEYFRAMES {
	    {glm::vec3 {29785.9609f, 5140.81982f, -16279.8477f},
	     glm::quat {0.999809802f, -0.000760567724f, 0.0174517576f, 0.00869333092f}},
	    {glm::vec3 {29785.9609f, 5720.81982f, -16279.8477f},
	     glm::quat {0.999809802f, -0.000760567724f, 0.0174517576f, 0.00869333092f}},
	};

	TEST_CASE("zCMover(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCMover.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_mover>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCMover:zCTrigger:zCVob");

		phoenix::vobs::trigger_mover::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->behavior, phoenix::mover_behavior::toggle);
		CHECK_EQ(vob->touch_blocker_damage, 0.0f);
		CHECK_EQ(vob->stay_open_time_sec, 2.0f);
		CHECK_FALSE(vob->locked);
		CHECK_FALSE(vob->auto_link);
		CHECK_FALSE(vob->auto_rotate);
		CHECK_EQ(vob->speed, 0.0500000007f);
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

	TEST_CASE("oCTriggerChangeLevel(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCTriggerChangeLevel.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_change_level>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerChangeLevel:zCTrigger:zCVob");

		phoenix::vobs::trigger_change_level::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->level_name, "ADDON\\ADDONWORLD.ZEN");
		CHECK_EQ(vob->start_vob, "START_ADDON");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCTriggerWorldStart(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCTriggerWorldStart.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_world_start>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerWorldStart:zCVob");

		phoenix::vobs::trigger_world_start::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->target, "EVT_TROLL_GRAVE_MOVER_01");
		CHECK(vob->fire_once);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCTriggerUntouch(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCTriggerUntouch.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::trigger_untouch>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerUntouch:zCVob");

		phoenix::vobs::trigger_untouch::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->target, "EVT_TROLL_GRAVE_TRIGGERLIST_01");

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCEarthquake(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/zCEarthquake.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::earthquake>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCEarthquake:zCVob");

		phoenix::vobs::earthquake::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->radius, 1000.0f);
		CHECK_EQ(vob->duration, 5.0f);
		CHECK_EQ(vob->amplitude, glm::vec3 {2.0f, 10.0f, 2.0f});

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTouchDamage(parse:g2)") {
		auto buf = phoenix::buffer::mmap("./samples/G2/VOb/oCTouchDamage.zen");
		auto ar = phoenix::archive_reader::open(buf);
		auto vob = std::make_unique<phoenix::vobs::touch_damage>();

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTouchDamage:zCTouchDamage:zCVob");

		phoenix::vobs::touch_damage::parse(*vob, *ar, phoenix::game_version::gothic_2);

		CHECK_EQ(vob->damage, 1000.0f);
		CHECK(vob->barrier);
		CHECK_FALSE(vob->blunt);
		CHECK(vob->edge);
		CHECK_FALSE(vob->fire);
		CHECK_FALSE(vob->fly);
		CHECK_FALSE(vob->magic);
		CHECK_FALSE(vob->point);
		CHECK_FALSE(vob->fall);
		CHECK_EQ(vob->repeat_delay_sec, 0.0f);
		CHECK_EQ(vob->volume_scale, 1.0f);
		CHECK_EQ(vob->collision, phoenix::collision_type::box);

		CHECK_FALSE(vob->is_save_game());
		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCMoverControler(parse:g2)") {
		// TODO: Stub
	}

	TEST_CASE("oCNpc(parse:g2)") {
		// TODO: Stub
	}
}
