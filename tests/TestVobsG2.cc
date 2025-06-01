// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/Archive.hh>
#include <zenkit/ModelAnimation.hh>
#include <zenkit/vobs/Camera.hh>
#include <zenkit/vobs/Light.hh>
#include <zenkit/vobs/Misc.hh>
#include <zenkit/vobs/MovableObject.hh>
#include <zenkit/vobs/Sound.hh>
#include <zenkit/vobs/Trigger.hh>
#include <zenkit/vobs/VirtualObject.hh>
#include <zenkit/vobs/Zone.hh>

TEST_SUITE("vobs") {
	static zenkit::ArchiveObject obj;

	TEST_CASE("zCVob(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCVob.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVob");

		zenkit::VirtualObject vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.bbox.min, zenkit::Vec3 {30897.1035f, 4760.24951f, -14865.5723f});
		CHECK_EQ(vob.bbox.max, zenkit::Vec3 {30929.8301f, 4836.17529f, -14817.3135f});
		CHECK_EQ(vob.position, zenkit::Vec3 {30913.4668f, 4798.9751f, -14841.4434f});
		CHECK_EQ(vob.rotation,
		         zenkit::Mat3 {
		             {0.920505285f, 0, -0.390731275f},
		             {0, 1, 0},
		             {0.390731275f, 0, 0.920505285f},
		         });
		CHECK(vob.show_visual);
		CHECK_EQ(vob.sprite_camera_facing_mode, zenkit::SpriteAlignment::NONE);
		CHECK_FALSE(vob.cd_static);
		CHECK_FALSE(vob.cd_dynamic);
		CHECK(vob.vob_static);
		CHECK_EQ(vob.dynamic_shadows, zenkit::ShadowType::NONE);
		CHECK_FALSE(vob.physics_enabled);
		CHECK_EQ(vob.anim_mode, zenkit::AnimationType::NONE);
		CHECK_EQ(vob.bias, 0);
		CHECK_FALSE(vob.ambient);
		CHECK_EQ(vob.anim_strength, 0.0f);
		CHECK_EQ(vob.far_clip_scale, 1.0f);
		CHECK_EQ(vob.preset_name, "");
		CHECK_EQ(vob.vob_name, "");
		CHECK_EQ(vob.visual->name, "OW_MISC_WALL_TORCH_01.3DS");
		CHECK_EQ(vob.visual->type, zenkit::VisualType::MULTI_RESOLUTION_MESH);
		CHECK_EQ(vob.visual_decal, std::nullopt);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCCSCamera(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCCSCamera.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCCSCamera:zCVob");

		zenkit::VCutsceneCamera vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.trajectory_for, zenkit::CameraCoordinateReference::WORLD);
		CHECK_EQ(vob.target_trajectory_for, zenkit::CameraCoordinateReference::WORLD);
		CHECK_EQ(vob.loop_mode, zenkit::CameraLoop::NONE);
		CHECK_EQ(vob.lerp_mode, zenkit::CameraLerpType::PATH);
		CHECK_FALSE(vob.ignore_for_vob_rotation);
		CHECK_FALSE(vob.ignore_for_vob_rotation_target);
		CHECK_FALSE(vob.adapt);
		CHECK_FALSE(vob.ease_first);
		CHECK_FALSE(vob.ease_last);
		CHECK_EQ(vob.total_duration, 20.0f);
		CHECK_EQ(vob.auto_focus_vob, "");
		CHECK_FALSE(vob.auto_player_movable);
		CHECK_FALSE(vob.auto_untrigger_last);
		CHECK_EQ(vob.auto_untrigger_last_delay, 0.0f);
		CHECK_EQ(vob.position_count, 2);
		CHECK_EQ(vob.target_count, 1);

		auto const& frames = vob.trajectory_frames;
		CHECK_EQ(frames[0]->time, 0.0f);
		CHECK_EQ(frames[0]->roll_angle, 0.0f);
		CHECK_EQ(frames[0]->fov_scale, 1.0f);
		CHECK_EQ(frames[0]->motion_type, zenkit::CameraMotion::SLOW);
		CHECK_EQ(frames[0]->motion_type_fov, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(frames[0]->motion_type_roll, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(frames[0]->motion_type_time_scale, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(frames[0]->tension, 0.0f);
		CHECK_EQ(frames[0]->cam_bias, 0.0f);
		CHECK_EQ(frames[0]->continuity, 0.0f);
		CHECK_EQ(frames[0]->time_scale, 1.0f);
		CHECK_FALSE(frames[0]->time_fixed);
		CHECK_EQ(frames[0]->original_pose,
		         zenkit::Mat4 {
		             {0.202226311f, 3.00647909e-11f, -0.979338825f, 0},
		             {0.00805913191f, 0.999966145f, 0.00166415179f, 0.0f},
		             {0.979305685f, -0.00822915602f, 0.202219456f, 0.0f},
		             {81815.7656f, 3905.95044f, 29227.1875f, 1.0f},
		         });

		CHECK_EQ(frames[1]->time, 20.0f);
		CHECK_EQ(frames[1]->roll_angle, 0.0f);
		CHECK_EQ(frames[1]->fov_scale, 1.0f);
		CHECK_EQ(frames[1]->motion_type, zenkit::CameraMotion::SLOW);
		CHECK_EQ(frames[1]->motion_type_fov, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(frames[1]->motion_type_roll, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(frames[1]->motion_type_time_scale, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(frames[1]->tension, 0.0f);
		CHECK_EQ(frames[1]->cam_bias, 0.0f);
		CHECK_EQ(frames[1]->continuity, 0.0f);
		CHECK_EQ(frames[1]->time_scale, 1.0f);
		CHECK_FALSE(frames[1]->time_fixed);
		CHECK_EQ(frames[1]->original_pose,
		         zenkit::Mat4 {
		             {1.0f, 0.0f, -0.0f, 0.0f},
		             {-0.0f, 1.0f, -0.0f, 0.0f},
		             {0.0f, 0.0f, 1.0f, 0.0f},
		             {82642.8984f, 3977.64844f, 28358.457f, 1.0f},
		         });

		auto const& target_frames = vob.target_frames;
		CHECK_EQ(target_frames[0]->time, 0.0f);
		CHECK_EQ(target_frames[0]->roll_angle, 0.0f);
		CHECK_EQ(target_frames[0]->fov_scale, 1.0f);
		CHECK_EQ(target_frames[0]->motion_type, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(target_frames[0]->motion_type_fov, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(target_frames[0]->motion_type_roll, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(target_frames[0]->motion_type_time_scale, zenkit::CameraMotion::SMOOTH);
		CHECK_EQ(target_frames[0]->tension, 0.0f);
		CHECK_EQ(target_frames[0]->cam_bias, 0.0f);
		CHECK_EQ(target_frames[0]->continuity, 0.0f);
		CHECK_EQ(target_frames[0]->time_scale, 1.0f);
		CHECK_FALSE(target_frames[0]->time_fixed);
		CHECK_EQ(target_frames[0]->original_pose,
		         zenkit::Mat4 {
		             {1.0f, 0.0f, 0.0f, 0.0f},
		             {0.0f, 1.0f, 0.0f, 0.0f},
		             {0.0f, 0.0f, 1.0f, 0.0f},
		             {82642.8984f, 3899.0f, 29397.9844f, 1.0f},
		         });

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobAnimate(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCVobAnimate.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobAnimate:zCVob");

		zenkit::VAnimate vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK(vob.start_on);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneVobFarPlane(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCZoneVobFarPlane.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneVobFarPlane:zCVob");

		zenkit::VZoneFarPlane vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.vob_far_plane_z, 6500.0f);
		CHECK_EQ(vob.inner_range_percentage, 0.699999988f);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneZFog(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCZoneZFog.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneZFog:zCVob");

		zenkit::VZoneFog vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.range_center, 16000.0f);
		CHECK_EQ(vob.inner_range_percentage, 0.699999988f);
		CHECK_EQ(vob.color, zenkit::Color {120, 120, 120, 255});
		CHECK_FALSE(vob.fade_out_sky);
		CHECK_FALSE(vob.override_color);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCItem(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCItem.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCItem:zCVob");

		zenkit::VItem vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.instance, "ITPL_BLUEPLANT");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCTrigger(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCTrigger.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTrigger:zCVob");

		zenkit::VTrigger vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.target, "");
		CHECK_EQ(vob.flags, 3);
		CHECK_EQ(vob.filter_flags, 0);
		CHECK_EQ(vob.vob_target, "");
		CHECK_EQ(vob.max_activation_count, -1);
		CHECK_EQ(vob.retrigger_delay_sec, 0.0f);
		CHECK_EQ(vob.damage_threshold, 0.0f);
		CHECK_EQ(vob.fire_delay_sec, 0.0f);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMOB(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCMOB.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMOB:zCVob");

		zenkit::VMovableObject vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.name, "MOBNAME_GRAVE_18");
		CHECK_EQ(vob.hp, 10);
		CHECK_EQ(vob.damage, 0);
		CHECK_FALSE(vob.movable);
		CHECK_FALSE(vob.takable);
		CHECK_FALSE(vob.focus_override);
		CHECK_EQ(vob.material, zenkit::SoundMaterialType::WOOD);
		CHECK_EQ(vob.visual_destroyed, "");
		CHECK_EQ(vob.owner, "");
		CHECK_EQ(vob.owner_guild, "");
		CHECK_FALSE(vob.destroyed);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobInter(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCMobInter.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobInter:oCMOB:zCVob");

		zenkit::VInteractiveObject vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.state, 1);
		CHECK_EQ(vob.target, "");
		CHECK_EQ(vob.item, "");
		CHECK_EQ(vob.condition_function, "");
		CHECK_EQ(vob.on_state_change_function, "PRAYIDOL");
		CHECK_FALSE(vob.rewind);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobFire(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCMobFire.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobFire:oCMobInter:oCMOB:zCVob");

		zenkit::VFire vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.slot, "BIP01 FIRE");
		CHECK_EQ(vob.vob_tree, "FIRETREE_LAMP.ZEN");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobContainer(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCMobContainer.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobContainer:oCMobInter:oCMOB:zCVob");

		zenkit::VContainer vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_FALSE(vob.locked);
		CHECK_EQ(vob.key, "");
		CHECK_EQ(vob.pick_string, "");
		CHECK_EQ(vob.contents, "ItMi_Gold:35");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobDoor(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCMobDoor.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobDoor:oCMobInter:oCMOB:zCVob");

		zenkit::VDoor vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_FALSE(vob.locked);
		CHECK_EQ(vob.key, "");
		CHECK_EQ(vob.pick_string, "");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCPFXControler(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCPFXControler.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCPFXControler:zCVob");

		zenkit::VParticleEffectController vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.pfx_name, "STARGATE_EDGES.PFX");
		CHECK_FALSE(vob.kill_when_done);
		CHECK_FALSE(vob.initially_running);

		CHECK(ar->read_object_end());
	}

	static std::vector<float> const G2_LIGHT_RANGE_ANIMATION_SCALE {};
	static std::vector<zenkit::Color> const G2_LIGHT_COLOR_ANIMATION_LIST {};

	TEST_CASE("zCVobLight(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCVobLight.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobLight:zCVob");

		zenkit::VLight vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.preset, "NW_STANDART_FIRE_STATIC");
		CHECK_EQ(vob.light_type, zenkit::LightType::POINT);
		CHECK_EQ(vob.range, 400.0f);
		CHECK_EQ(vob.color, zenkit::Color {100, 71, 60, 255});
		CHECK_EQ(vob.cone_angle, 0.0f);
		CHECK(vob.is_static);
		CHECK_EQ(vob.quality, zenkit::LightQuality::LOW);
		CHECK_EQ(vob.lensflare_fx, "");
		CHECK_FALSE(vob.on);
		CHECK_EQ(vob.range_animation_scale, G2_LIGHT_RANGE_ANIMATION_SCALE);
		CHECK_EQ(vob.range_animation_fps, 0.0f);
		CHECK(vob.range_animation_smooth);
		CHECK_EQ(vob.color_animation_list, G2_LIGHT_COLOR_ANIMATION_LIST);
		CHECK_EQ(vob.color_animation_fps, 0.0f);
		CHECK(vob.color_animation_smooth);
		CHECK(vob.can_move);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSound(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCVobSound.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSound:zCVob");

		zenkit::VSound vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.volume, 80.0f);
		CHECK_EQ(vob.mode, zenkit::SoundMode::RANDOM);
		CHECK_EQ(vob.random_delay, 30.0f);
		CHECK_EQ(vob.random_delay_var, 20.0f);
		CHECK(vob.initially_playing);
		CHECK_FALSE(vob.ambient3d);
		CHECK_FALSE(vob.obstruction);
		CHECK_EQ(vob.cone_angle, 0.0f);
		CHECK_EQ(vob.volume_type, zenkit::SoundTriggerVolumeType::SPHERICAL);
		CHECK_EQ(vob.radius, 3000.0f);
		CHECK_EQ(vob.sound_name, "OW_CROW");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSoundDaytime(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCVobSoundDaytime.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSoundDaytime:zCVobSound:zCVob");

		zenkit::VSoundDaytime vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.start_time, 5.0f);
		CHECK_EQ(vob.end_time, 21.0f);
		CHECK_EQ(vob.sound_name2, "InsectsFrogs_Night");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCZoneMusic(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCZoneMusic.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCZoneMusic:zCVob");

		zenkit::VZoneMusic vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK(vob.enabled);
		CHECK_EQ(vob.priority, 1);
		CHECK_FALSE(vob.ellipsoid);
		CHECK_EQ(vob.reverb, -3.2190001f);
		CHECK_EQ(vob.volume, 1.0f);
		CHECK(vob.loop);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCMessageFilter(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCMessageFilter.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCMessageFilter:zCVob");

		zenkit::VMessageFilter vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.target, "EVT_ADDON_TROLLPORTAL_CAMERA_01");
		CHECK_EQ(vob.on_trigger, zenkit::MessageFilterAction::UNTRIGGER);
		CHECK_EQ(vob.on_untrigger, zenkit::MessageFilterAction::UNTRIGGER);

		CHECK(ar->read_object_end());
	}

	static std::vector<std::string> const G2_CODE_MASTER_SLAVES {
	    "EVT_ORNAMENT_SWITCH_BIGFARM_01",
	    "EVT_ORNAMENT_SWITCH_BIGFARM_02",
	    "EVT_ORNAMENT_SWITCH_BIGFARM_03",
	};

	TEST_CASE("zCCodeMaster(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCCodeMaster.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCCodeMaster:zCVob");

		zenkit::VCodeMaster vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.target, "EVT_ORNAMENT_TRIGGER_BIGFARM_01");
		CHECK_FALSE(vob.ordered);
		CHECK_FALSE(vob.first_false_is_failure);
		CHECK_EQ(vob.failure_target, "");
		CHECK_FALSE(vob.untriggered_cancels);
		CHECK_EQ(vob.slaves, G2_CODE_MASTER_SLAVES);

		CHECK(ar->read_object_end());
	}

	static std::vector<zenkit::VTriggerList::Target> const G2_TRIGGER_LIST_TARGETS {
	    {"EVT_ADDON_TROLLPORTAL_MASTERTRIGGERLIST_ALPHA_01", 0.0f},
	    {"EVT_ADDON_TROLLPORTAL_TRIGGERSCRIPT_01", 0.0f},
	};

	TEST_CASE("zCTriggerList(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCTriggerList.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerList:zCTrigger:zCVob");

		zenkit::VTriggerList vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.mode, zenkit::TriggerBatchMode::ALL);
		CHECK_EQ(vob.targets, G2_TRIGGER_LIST_TARGETS);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTriggerScript(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCTriggerScript.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerScript:zCTrigger:zCVob");

		zenkit::VTriggerScript vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.function, "EVT_CAVALORNSGOBBOS_FUNC");

		CHECK(ar->read_object_end());
	}

	static std::vector<zenkit::AnimationSample> const G1_MOVER_KEYFRAMES {
	    {zenkit::Vec3 {29785.9609f, 5140.81982f, -16279.8477f},
	     zenkit::Quat {0.999809802f, -0.000760567724f, 0.0174517576f, 0.00869333092f}},
	    {zenkit::Vec3 {29785.9609f, 5720.81982f, -16279.8477f},
	     zenkit::Quat {0.999809802f, -0.000760567724f, 0.0174517576f, 0.00869333092f}},
	};

	TEST_CASE("zCMover(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCMover.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCMover:zCTrigger:zCVob");

		zenkit::VMover vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.behavior, zenkit::MoverBehavior::TOGGLE);
		CHECK_EQ(vob.touch_blocker_damage, 0.0f);
		CHECK_EQ(vob.stay_open_time_sec, 2.0f);
		CHECK_FALSE(vob.locked);
		CHECK_FALSE(vob.auto_link);
		CHECK_FALSE(vob.auto_rotate);
		CHECK_EQ(vob.speed, 0.0500000007f);
		CHECK_EQ(vob.lerp_mode, zenkit::MoverLerpType::LINEAR);
		CHECK_EQ(vob.speed_mode, zenkit::MoverSpeedType::SLOW_START_END);
		CHECK_EQ(vob.keyframes, G1_MOVER_KEYFRAMES);
		CHECK_EQ(vob.sfx_open_start, "GATE_START");
		CHECK_EQ(vob.sfx_open_end, "GATE_STOP");
		CHECK_EQ(vob.sfx_transitioning, "GATE_LOOP");
		CHECK_EQ(vob.sfx_close_start, "GATE_START");
		CHECK_EQ(vob.sfx_close_end, "GATE_STOP");
		CHECK_EQ(vob.sfx_lock, "");
		CHECK_EQ(vob.sfx_unlock, "");
		CHECK_EQ(vob.sfx_use_locked, "");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTriggerChangeLevel(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCTriggerChangeLevel.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerChangeLevel:zCTrigger:zCVob");

		zenkit::VTriggerChangeLevel vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.level_name, "ADDON\\ADDONWORLD.ZEN");
		CHECK_EQ(vob.start_vob, "START_ADDON");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCTriggerWorldStart(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCTriggerWorldStart.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerWorldStart:zCVob");

		zenkit::VTriggerWorldStart vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.target, "EVT_TROLL_GRAVE_MOVER_01");
		CHECK(vob.fire_once);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCTriggerUntouch(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCTriggerUntouch.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerUntouch:zCVob");

		zenkit::VTriggerUntouch vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.target, "EVT_TROLL_GRAVE_TRIGGERLIST_01");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCEarthquake(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/zCEarthquake.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCEarthquake:zCVob");

		zenkit::VEarthquake vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.radius, 1000.0f);
		CHECK_EQ(vob.duration, 5.0f);
		CHECK_EQ(vob.amplitude, zenkit::Vec3 {2.0f, 10.0f, 2.0f});

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTouchDamage(GOTHIC2)") {
		auto buf = zenkit::Read::from("./samples/G2/VOb/oCTouchDamage.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTouchDamage:zCTouchDamage:zCVob");

		zenkit::VTouchDamage vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_2);

		CHECK_EQ(vob.damage, 1000.0f);
		CHECK(vob.barrier);
		CHECK_FALSE(vob.blunt);
		CHECK(vob.edge);
		CHECK_FALSE(vob.fire);
		CHECK_FALSE(vob.fly);
		CHECK_FALSE(vob.magic);
		CHECK_FALSE(vob.point);
		CHECK_FALSE(vob.fall);
		CHECK_EQ(vob.repeat_delay_sec, 0.0f);
		CHECK_EQ(vob.volume_scale, 1.0f);
		CHECK_EQ(vob.collision, zenkit::TouchCollisionType::BOX);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCMoverControler(GOTHIC2)") {
		// TODO: Stub
	}

	TEST_CASE("oCNpc(GOTHIC2)") {
		// TODO: Stub
	}
}
