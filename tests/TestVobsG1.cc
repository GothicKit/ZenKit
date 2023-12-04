// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/Archive.hh>
#include <zenkit/ModelAnimation.hh>
#include <zenkit/vobs/Light.hh>
#include <zenkit/vobs/Misc.hh>
#include <zenkit/vobs/MovableObject.hh>
#include <zenkit/vobs/Sound.hh>
#include <zenkit/vobs/Trigger.hh>
#include <zenkit/vobs/VirtualObject.hh>
#include <zenkit/vobs/Zone.hh>

TEST_SUITE("vobs") {
	static zenkit::ArchiveObject obj;

	TEST_CASE("zCVob(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCVob.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVob");

		zenkit::VirtualObject vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.bbox.min, glm::vec3 {-18966.623f, -236.707687f, 4373.23486f});
		CHECK_EQ(vob.bbox.max, glm::vec3 {-18772.623f, -42.7076874f, 4567.23486f});
		CHECK_EQ(vob.position, glm::vec3 {-18869.623f, -139.707687f, 4470.23486f});
		CHECK_EQ(vob.rotation,
		         glm::mat3x3 {
		             {1, 0, 0},
		             {0, 1, 0},
		             {0, 0, 1},
		         });
		CHECK(vob.show_visual);
		CHECK_EQ(vob.sprite_camera_facing_mode, zenkit::SpriteAlignment::NONE);
		CHECK_FALSE(vob.cd_static);
		CHECK_FALSE(vob.cd_dynamic);
		CHECK_FALSE(vob.vob_static);
		CHECK_EQ(vob.dynamic_shadows, zenkit::ShadowType::NONE);
		CHECK_FALSE(vob.physics_enabled);
		CHECK_EQ(vob.anim_mode, zenkit::AnimationType::NONE);
		CHECK_EQ(vob.bias, 0);
		CHECK_FALSE(vob.ambient);
		CHECK_EQ(vob.anim_strength, 0.0f);
		CHECK_EQ(vob.far_clip_scale, 2.0f);
		CHECK_EQ(vob.preset_name, "");
		CHECK_EQ(vob.vob_name, "");
		CHECK_EQ(vob.visual->name, "FIRE.pfx");
		CHECK_EQ(vob.visual->type, zenkit::VisualType::PARTICLE_EFFECT);
		CHECK_EQ(vob.visual_decal, std::nullopt);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobAnimate(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCVobAnimate.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobAnimate:zCVob");

		zenkit::VAnimate vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_FALSE(vob.start_on);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneVobFarPlane(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCZoneVobFarPlane.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneVobFarPlane:zCVob");

		zenkit::VZoneFarPlane vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.vob_far_plane_z, 9000.0f);
		CHECK_EQ(vob.inner_range_percentage, 0.699999988f);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCZoneZFog(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCZoneZFog.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCZoneZFog:zCVob");

		zenkit::VZoneFog vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.range_center, 8000.0f);
		CHECK_EQ(vob.inner_range_percentage, 0.850000024f);
		CHECK_EQ(vob.color, glm::u8vec4 {120, 120, 120, 20});
		CHECK_FALSE(vob.fade_out_sky);
		CHECK_FALSE(vob.override_color);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobLensFlare(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCVobLensFlare.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobLensFlare:zCVob");

		zenkit::VLensFlare vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.fx, "TORCHFX01");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCItem(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCItem.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCItem:zCVob");

		zenkit::VItem vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.instance, "ITMW_1H_AXE_01");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCCSTrigger(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCCSTrigger.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCCSTrigger:zCTrigger:zCVob");

		zenkit::VTrigger vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.target, "AMB_PSI_CS003.CS");
		CHECK_EQ(vob.flags, 3);
		CHECK_EQ(vob.filter_flags, 23);
		CHECK_EQ(vob.vob_target, "");
		CHECK_EQ(vob.max_activation_count, -1);
		CHECK_EQ(vob.retrigger_delay_sec, 0.0f);
		CHECK_EQ(vob.damage_threshold, 0.0f);
		CHECK_EQ(vob.fire_delay_sec, 0.0f);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMOB(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCMOB.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMOB:zCVob");

		zenkit::VMovableObject vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.name, "");
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

	TEST_CASE("oCMobInter(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCMobInter.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobInter:oCMOB:zCVob");

		zenkit::VInteractiveObject vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.state, 1);
		CHECK_EQ(vob.target, "OW_ORC_MAINGATE_01");
		CHECK_EQ(vob.item, "");
		CHECK_EQ(vob.condition_function, "MC_ORCCITY_GATE");
		CHECK_EQ(vob.on_state_change_function, "");
		CHECK_FALSE(vob.rewind);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobFire(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCMobFire.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobFire:oCMobInter:oCMOB:zCVob");

		zenkit::VFire vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.slot, "BIP01 FIRE");
		CHECK_EQ(vob.vob_tree, "FIRETREE_MEDIUM.ZEN");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobContainer(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCMobContainer.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobContainer:oCMobInter:oCMOB:zCVob");

		zenkit::VContainer vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_FALSE(vob.locked);
		CHECK_EQ(vob.key, "");
		CHECK_EQ(vob.pick_string, "");
		CHECK_EQ(vob.contents, "");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCMobDoor(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCMobDoor.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCMobDoor:oCMobInter:oCMOB:zCVob");

		zenkit::VDoor vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_FALSE(vob.locked);
		CHECK_EQ(vob.key, "");
		CHECK_EQ(vob.pick_string, "");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCPFXControler(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCPFXControler.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCPFXControler:zCVob");

		zenkit::VParticleEffectController vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.pfx_name, "CS_miltenfog.PFX");
		CHECK(vob.kill_when_done);
		CHECK(vob.initially_running);

		CHECK(ar->read_object_end());
	}

	static std::vector<float> const G1_LIGHT_RANGE_ANIMATION_SCALE {};
	static std::vector<glm::u8vec4> const G1_LIGHT_COLOR_ANIMATION_LIST {
	    glm::u8vec4 {211, 147, 107, 255}, glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {211, 147, 107, 255},
	    glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {225, 197, 100, 255}, glm::u8vec4 {223, 173, 117, 255},
	    glm::u8vec4 {227, 209, 106, 255}, glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {211, 147, 107, 255},
	    glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {225, 197, 100, 255}, glm::u8vec4 {227, 209, 106, 255},
	    glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {211, 147, 107, 255}, glm::u8vec4 {225, 197, 100, 255},
	    glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {225, 197, 100, 255}, glm::u8vec4 {211, 147, 107, 255},
	    glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {227, 209, 106, 255}, glm::u8vec4 {225, 197, 100, 255},
	    glm::u8vec4 {211, 147, 107, 255}, glm::u8vec4 {225, 197, 100, 255}, glm::u8vec4 {223, 173, 117, 255},
	    glm::u8vec4 {225, 197, 100, 255}, glm::u8vec4 {227, 209, 106, 255}, glm::u8vec4 {223, 173, 117, 255},
	    glm::u8vec4 {211, 147, 107, 255}, glm::u8vec4 {223, 173, 117, 255}, glm::u8vec4 {211, 147, 107, 255},
	    glm::u8vec4 {225, 197, 100, 255}, glm::u8vec4 {227, 209, 106, 255}, glm::u8vec4 {223, 173, 117, 255},
	};

	TEST_CASE("zCVobLight(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCVobLight.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobLight:zCVob");

		zenkit::VLight vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.preset, "");
		CHECK_EQ(vob.light_type, zenkit::LightType::POINT);
		CHECK_EQ(vob.range, 2000.0f);
		CHECK_EQ(vob.color, glm::u8vec4 {223, 173, 117, 255});
		CHECK_EQ(vob.cone_angle, 0.0f);
		CHECK_FALSE(vob.is_static);
		CHECK_EQ(vob.quality, zenkit::LightQuality::LOW);
		CHECK_EQ(vob.lensflare_fx, "");
		CHECK(vob.on);
		CHECK_EQ(vob.range_animation_scale, G1_LIGHT_RANGE_ANIMATION_SCALE);
		CHECK_EQ(vob.range_animation_fps, 0.0f);
		CHECK(vob.range_animation_smooth);
		CHECK_EQ(vob.color_animation_list, G1_LIGHT_COLOR_ANIMATION_LIST);
		CHECK_EQ(vob.color_animation_fps, 11.0000067f);
		CHECK_FALSE(vob.color_animation_smooth);
		CHECK(vob.can_move);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSound(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCVobSound.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSound:zCVob");

		zenkit::VSound vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.volume, 100.0f);
		CHECK_EQ(vob.mode, zenkit::SoundMode::LOOP);
		CHECK_EQ(vob.random_delay, 5.0f);
		CHECK_EQ(vob.random_delay_var, 2.0f);
		CHECK(vob.initially_playing);
		CHECK_FALSE(vob.ambient3d);
		CHECK_FALSE(vob.obstruction);
		CHECK_EQ(vob.cone_angle, 0.0f);
		CHECK_EQ(vob.volume_type, zenkit::SoundTriggerVolumeType::SPHERICAL);
		CHECK_EQ(vob.radius, 1500.0f);
		CHECK_EQ(vob.sound_name, "FIRE_MEDIUM");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("zCVobSoundDaytime(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCVobSoundDaytime.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCVobSoundDaytime:zCVobSound:zCVob");

		zenkit::VSoundDaytime vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.start_time, 8.0f);
		CHECK_EQ(vob.end_time, 18.0f);
		CHECK_EQ(vob.sound_name2, "INSECTS_AND_NIGHTINGALES");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCZoneMusic(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCZoneMusic.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCZoneMusic:zCVob");

		zenkit::VZoneMusic vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK(vob.enabled);
		CHECK_EQ(vob.priority, 1);
		CHECK_FALSE(vob.ellipsoid);
		CHECK_EQ(vob.reverb, -9.0f);
		CHECK_EQ(vob.volume, 1.0f);
		CHECK(vob.loop);

		CHECK(ar->read_object_end());
	}

	static std::vector<zenkit::VTriggerList::Target> const G1_TRIGGER_LIST_TARGETS {
	    {"EVT_CASTLE_PLATE", 0.0f},
	    {"EVT_CASTLE_FLOOR_5", 6.0f},
	    {"EVT_CASTLE_FLOOR_4", 2.0f},
	    {"EVT_CASTLE_FLOOR_3", 1.0f},
	    {"EVT_CASTLE_FLOOR_2", 0.0f},
	    {"EVT_CASTLE_FLOOR_1", 4.0f},
	};

	TEST_CASE("zCTriggerList(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCTriggerList.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCTriggerList:zCTrigger:zCVob");

		zenkit::VTriggerList vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.mode, zenkit::TriggerBatchMode::ALL);
		CHECK_EQ(vob.targets, G1_TRIGGER_LIST_TARGETS);

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCTriggerScript(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCTriggerScript.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerScript:zCTrigger:zCVob");

		zenkit::VTriggerScript vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.function, "ON_NC_GATE_TRIGGER");

		CHECK(ar->read_object_end());
	}

	static std::vector<zenkit::AnimationSample> const G1_MOVER_KEYFRAMES {
	    {glm::vec3 {-23325.1992f, 3438.91333f, -21834.9473f},
	     glm::quat {0.105035283f, 0.091305837f, 0.747364759f, 0.649674594f}},
	    {glm::vec3 {-23325.1543f, 3438.91333f, -21844.3672f},
	     glm::quat {-0.00970827043f, 0.138834357f, -0.0690778494f, 0.98785609f}},
	};

	TEST_CASE("zCMover(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/zCMover.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "zCMover:zCTrigger:zCVob");

		zenkit::VMover vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.behavior, zenkit::MoverBehavior::TRIGGER_CONTROL);
		CHECK_EQ(vob.touch_blocker_damage, 0.0f);
		CHECK_EQ(vob.stay_open_time_sec, 2.0f);
		CHECK_FALSE(vob.locked);
		CHECK_FALSE(vob.auto_link);
		CHECK_FALSE(vob.auto_rotate);
		CHECK_EQ(vob.speed, 0.00200023991f);
		CHECK_EQ(vob.lerp_mode, zenkit::MoverLerpType::CURVE);
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

	TEST_CASE("oCTriggerChangeLevel(GOTHIC1)") {
		auto buf = zenkit::Read::from("./samples/G1/VOb/oCTriggerChangeLevel.zen");
		auto ar = zenkit::ReadArchive::from(buf.get());

		CHECK(ar->read_object_begin(obj));
		CHECK_EQ(obj.class_name, "oCTriggerChangeLevel:zCTrigger:zCVob");

		zenkit::VTriggerChangeLevel vob {};
		vob.load(*ar, zenkit::GameVersion::GOTHIC_1);

		CHECK_EQ(vob.level_name, "ORCTEMPEL.ZEN");
		CHECK_EQ(vob.start_vob, "ENTRANCE_ORCTEMPLE_SURFACE");

		CHECK(ar->read_object_end());
	}

	TEST_CASE("oCNpc(GOTHIC1)") {
		// TODO: Stub
	}
}
