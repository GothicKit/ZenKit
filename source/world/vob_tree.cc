// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/detail/error.hh>
#include <phoenix/world/vob_tree.hh>

#include <fmt/format.h>
#include <unordered_map>

namespace phoenix {
	static std::unordered_map<std::string, vob_type> _vob_type_map = {
	    {"zCVob", vob_type::zCVob},
	    {"zCVobLevelCompo:zCVob", vob_type::zCVobLevelCompo},
	    {"oCItem:zCVob", vob_type::oCItem},
	    {"oCMOB:zCVob", vob_type::oCMOB},
	    {"oCMobInter:oCMOB:zCVob", vob_type::oCMobInter},
	    {"oCMobBed:oCMobInter:oCMOB:zCVob", vob_type::oCMobBed},
	    {"oCMobFire:oCMobInter:oCMOB:zCVob", vob_type::oCMobFire},
	    {"oCMobLadder:oCMobInter:oCMOB:zCVob", vob_type::oCMobLadder},
	    {"oCMobSwitch:oCMobInter:oCMOB:zCVob", vob_type::oCMobSwitch},
	    {"oCMobWheel:oCMobInter:oCMOB:zCVob", vob_type::oCMobWheel},
	    {"oCMobContainer:oCMobInter:oCMOB:zCVob", vob_type::oCMobContainer},
	    {"oCMobDoor:oCMobInter:oCMOB:zCVob", vob_type::oCMobDoor},
	    {"zCPFXControler:zCVob", vob_type::zCPFXController},
	    {"zCVobAnimate:zCVob", vob_type::zCVobAnimate},
	    {"zCVobLensFlare:zCVob", vob_type::zCVobLensFlare},
	    {"zCVobLight:zCVob", vob_type::zCVobLight},
	    {"zCVobSpot:zCVob", vob_type::zCVobSpot},
	    {"zCVobStartpoint:zCVob", vob_type::zCVobStartpoint},
	    {"zCVobSound:zCVob", vob_type::zCVobSound},
	    {"zCVobSoundDaytime:zCVobSound:zCVob", vob_type::zCVobSoundDaytime},
	    {"oCZoneMusic:zCVob", vob_type::oCZoneMusic},
	    {"oCZoneMusicDefault:oCZoneMusic:zCVob", vob_type::oCZoneMusicDefault},
	    {"zCZoneZFog:zCVob", vob_type::zCZoneZFog},
	    {"zCZoneZFogDefault:zCZoneZFog:zCVob", vob_type::zCZoneZFogDefault},
	    {"zCZoneVobFarPlane:zCVob", vob_type::zCZoneVobFarPlane},
	    {"zCZoneVobFarPlaneDefault:zCZoneVobFarPlane:zCVob", vob_type::zCZoneVobFarPlaneDefault},
	    {"zCMessageFilter:zCVob", vob_type::zCMessageFilter},
	    {"zCCodeMaster:zCVob", vob_type::zCCodeMaster},
	    {"zCTrigger:zCVob", vob_type::zCTrigger},
	    {"zCTriggerList:zCTrigger:zCVob", vob_type::zCTriggerList},
	    {"oCTriggerScript:zCTrigger:zCVob", vob_type::oCTriggerScript},
	    {"zCMover:zCTrigger:zCVob", vob_type::zCMover},
	    {"oCTriggerChangeLevel:zCTrigger:zCVob", vob_type::oCTriggerChangeLevel},
	    {"zCTriggerWorldStart:zCVob", vob_type::zCTriggerWorldStart},
	    {"zCTriggerUntouch:zCVob", vob_type::zCTriggerUntouch},
	    {"zCCSCamera:zCVob", vob_type::zCCSCamera},
	    {"zCCamTrj_KeyFrame:zCVob", vob_type::zCCamTrj_KeyFrame},
	    {"oCTouchDamage:zCTouchDamage:zCVob", vob_type::oCTouchDamage},
	    {"zCEarthquake:zCVob", vob_type::zCEarthquake},
	    {"zCMoverControler:zCVob", vob_type::zCMoverController},
	    {"zCVobScreenFX:zCVob", vob_type::zCVobScreenFX},
	    {"zCVobStair:zCVob", vob_type::zCVobStair},
	    {"oCCSTrigger:zCTrigger:zCVob", vob_type::oCCSTrigger},
	    {"\xA7", vob_type::unknown}, // some sort of padding object, probably. seems to be always empty
	};

	static std::unordered_map<std::string, visual_type> _visual_type_map = {
	    {"zCDecal", visual_type::zCDecal},
	    {"zCMesh", visual_type::zCMesh},
	    {"zCProgMeshProto", visual_type::zCProgMeshProto},
	    {"zCParticleFX", visual_type::zCParticleFX},
	    {"zCModel", visual_type::zCModel},
	    {"zCMorphMesh", visual_type::zCMorphMesh},
	    {"\xA7", visual_type::unknown},
	};

	namespace vob {
		void base::parse(vob::base& vob, archive_reader_ref& in, game_version version) {
			auto packed = in->read_int() != 0; // pack
			bool has_visual_object = true;
			bool has_ai_object = true;

			if (packed) {
				auto bin = in->read_raw_bytes(); // dataRaw

				vob.bbox = bounding_box::parse(bin);
				vob.position = bin.get_vec3();
				vob.rotation = bin.get_mat3x3();

				std::uint8_t bit0 = bin.get();
				std::uint16_t bit1;

				if (version == game_version::gothic_1) {
					bit1 = bin.get();
				} else {
					bit1 = bin.get_ushort();
				}

				vob.show_visual = static_cast<bool>((bit0 & 0b00000001) >> 0);
				vob.camera_alignment = (bit0 & 0b00000110) >> 1;
				vob.cd_static = static_cast<bool>((bit0 & 0b00001000) >> 3);
				vob.cd_dynamic = static_cast<bool>((bit0 & 0b00010000) >> 4);
				vob.vob_static = static_cast<bool>((bit0 & 0b00100000) >> 5);
				vob.dynamic_shadows = (bit0 & 0b11000000) >> 6;

				bool has_preset_name = static_cast<bool>((bit1 & 0b000000000000001u) >> 0u);
				bool has_vob_name = static_cast<bool>((bit1 & 0b000000000000010u) >> 1u);
				bool has_visual_name = static_cast<bool>((bit1 & 0b000000000000100u) >> 2u);
				has_visual_object = static_cast<bool>((bit1 & 0b000000000001000u) >> 3u);
				has_ai_object = static_cast<bool>((bit1 & 0b000000000010000u) >> 4u);
				// bool has_event_man_object = static_cast<bool>((bit1 & 0b000000000100000u) >> 5u);
				vob.physics_enabled = static_cast<bool>((bit1 & 0b000000001000000u) >> 6u);

				if (version == game_version::gothic_2) {
					vob.anim_mode = static_cast<animation_mode>(bit1 & 0b000000110000000u >> 7u);
					vob.bias = static_cast<std::int32_t>((bit1 & 0b011111000000000u) >> 9u);
					vob.ambient = static_cast<bool>((bit1 & 0b100000000000000u) >> 14u);

					vob.anim_strength = bin.get_float();
					vob.far_clip_scale = bin.get_float();
				}

				if (has_preset_name) {
					vob.preset_name = in->read_string(); // presetName
				}

				if (has_vob_name) {
					vob.vob_name = in->read_string(); // vobName
				}

				if (has_visual_name) {
					vob.visual_name = in->read_string(); // visual
				}
			} else {
				vob.preset_name = in->read_string();
				vob.bbox = in->read_bbox(); // bbox3DWS

				vob.rotation = in->read_mat3x3(); // trafoOSToWSRot
				vob.position = in->read_vec3();   // trafoOSToWSPos

				vob.vob_name = in->read_string();       // vobName
				vob.visual_name = in->read_string();    // visual
				vob.show_visual = in->read_bool();      // showVisual
				vob.camera_alignment = in->read_enum(); // visualCamAlign

				if (version == game_version::gothic_1) {
					vob.cd_static = in->read_bool();       // cdStatic
					vob.cd_dynamic = in->read_bool();      // cdDyn
					vob.vob_static = in->read_bool();      // staticVob
					vob.dynamic_shadows = in->read_enum(); // dynShadow
				} else {
					vob.anim_mode = static_cast<animation_mode>(in->read_enum()); // visualAniMode
					vob.anim_strength = in->read_float();                         // visualAniModeStrength
					vob.far_clip_scale = in->read_float();                        // vobFarClipZScale
					vob.cd_static = in->read_bool();                              // cdStatic
					vob.cd_dynamic = in->read_bool();                             // cdDyn
					vob.vob_static = in->read_bool();                             // staticVob
					vob.dynamic_shadows = in->read_enum();                        // dynShadow
					vob.bias = in->read_int();                                    // zbias
					vob.ambient = in->read_bool();                                // isAmbient
				}
			}

			if (has_visual_object) {
				archive_object visual {};
				in->read_object_begin(visual);
				vob.associated_visual_type = _visual_type_map[visual.class_name];

				if (vob.associated_visual_type == visual_type::zCDecal) {
					decal::parse(vob.visual_decal, in, version);
				}

				if (!in->read_object_end()) {
					fmt::print(stderr, "warning: vob_tree: visual \"{}\" not fully parsed\n", visual.class_name);
					in->skip_object(true);
				}
			}

			if (has_ai_object) {
				in->skip_object(false);
			}
		}

		void light_preset::parse(light_preset& vob, archive_reader_ref& in, game_version version) {
			vob.preset = in->read_string();       // lightPresetInUse
			vob.type = in->read_enum();           // lightType
			vob.range = in->read_float();         // range
			vob.color = in->read_color();         // color
			vob.cone_angle = in->read_float();    // spotConeAngle
			vob.is_static = in->read_bool();      // lightStatic
			vob.quality = in->read_enum();        // lightQuality
			vob.lensflare_fx = in->read_string(); // lensflareFX

			if (!vob.is_static) {
				vob.on = in->read_bool();                      // turnedOn
				auto range_ani_scale = in->read_string();      // rangeAniScale
				vob.range_animation_fps = in->read_float();    // rangeAniFPS
				vob.range_animation_smooth = in->read_bool();  // rangeAniSmooth
				auto color_animation_list = in->read_string(); // colorAniList
				vob.color_animation_fps = in->read_float();    // colorAniFPS
				vob.color_animation_smooth = in->read_bool();  // colorAniSmooth

				std::istringstream ranges {range_ani_scale};
				float value;
				while (!ranges.eof()) {
					ranges >> value;
					vob.range_animation_scale.push_back(value);
				}

				std::istringstream colors {color_animation_list};
				colors.setf(std::ios::skipws);

				uint8_t r, g, b;
				char br = ' ';
				while (!colors.eof()) {
					colors >> br >> r >> g >> b >> br;
					vob.color_animation_list.emplace_back(r, g, b, 0);
				}

				if (version == game_version::gothic_2) {
					vob.can_move = in->read_bool(); // canMove
				}
			}
		}

		void light::parse(light& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);

			vob.preset_name = in->read_string();  // lightPresetInUse
			vob.type = in->read_enum();           // lightType
			vob.range = in->read_float();         // range
			vob.color = in->read_color();         // color
			vob.cone_angle = in->read_float();    // spotConeAngle
			vob.is_static = in->read_bool();      // lightStatic
			vob.quality = in->read_enum();        // lightQuality
			vob.lensflare_fx = in->read_string(); // lensflareFX

			if (!vob.is_static) {
				vob.on = in->read_bool();                      // turnedOn
				auto range_ani_scale = in->read_string();      // rangeAniScale
				vob.range_animation_fps = in->read_float();    // rangeAniFPS
				vob.range_animation_smooth = in->read_bool();  // rangeAniSmooth
				auto color_animation_list = in->read_string(); // colorAniList
				vob.color_animation_fps = in->read_float();    // colorAniFPS
				vob.color_animation_smooth = in->read_bool();  // colorAniSmooth

				std::istringstream ranges {range_ani_scale};
				float value;
				while (!ranges.eof()) {
					ranges >> value;
					vob.range_animation_scale.push_back(value);
				}

				std::istringstream colors {color_animation_list};
				colors.setf(std::ios::skipws);

				uint8_t r, g, b;
				char br = ' ';
				while (!colors.eof()) {
					colors >> br >> r >> g >> b >> br;
					vob.color_animation_list.emplace_back(r, g, b, 0);
				}

				if (version == game_version::gothic_2) {
					vob.can_move = in->read_bool(); // canMove
				}
			}
		}

		void sound::parse(sound& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.volume = in->read_float();                                     // sndVolume
			vob.mode = static_cast<sound_mode>(in->read_enum());               // sndMode
			vob.random_delay = in->read_float();                               // sndRandDelay
			vob.random_delay_var = in->read_float();                           // sndRandDelayVar
			vob.initially_playing = in->read_bool();                           // sndStartOn
			vob.ambient3d = in->read_bool();                                   // sndAmbient3D
			vob.obstruction = in->read_bool();                                 // sndObstruction
			vob.cone_angle = in->read_float();                                 // sndConeAngle
			vob.volume_type = static_cast<sound_volume_type>(in->read_enum()); // sndVolType
			vob.radius = in->read_float();                                     // sndRadius
			vob.name = in->read_string();                                      // sndName
		}

		void zone_music::parse(zone_music& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.enabled = in->read_bool();   // enabled
			vob.priority = in->read_int();   // priority
			vob.ellipsoid = in->read_bool(); // ellipsoid
			vob.reverb = in->read_float();   // reverbLevel
			vob.volume = in->read_float();   // volumeLevel
			vob.loop = in->read_bool();      // loop
		}

		void code_master::parse(code_master& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.target = in->read_string();               // triggerTarget
			vob.ordered = in->read_bool();                // orderRelevant
			vob.first_false_is_failure = in->read_bool(); // firstFalseIsFailure
			vob.failure_target = in->read_string();       // triggerTargetFailure
			vob.untriggered_cancels = in->read_bool();    // untriggerCancels

			auto slave_count = in->read_byte(); // numSlaves
			for (int i = 0; i < slave_count; ++i) {
				vob.slaves.emplace_back(in->read_string()); // slaveVobName[i]
			}
		}

		void trigger::parse(trigger& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.target = in->read_string();                // triggerTarget
			vob.flags = in->read_raw_bytes().get();        // flags
			vob.filter_flags = in->read_raw_bytes().get(); // filterFlags
			vob.vob_target = in->read_string();            // respondToVobName
			vob.max_activation_count = in->read_int();     // numCanBeActivated
			vob.retrigger_delay_sec = in->read_float();    // retriggerWaitSec
			vob.damage_threshold = in->read_float();       // damageThreshold
			vob.fire_delay_sec = in->read_float();         // fireDelaySec
		}

		void touch_damage::parse(touch_damage& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.damage = in->read_float();           // damage
			vob.barrier = in->read_bool();           // Barrier
			vob.blunt = in->read_bool();             // Blunt
			vob.edge = in->read_bool();              // Edge
			vob.fire = in->read_bool();              // Fire
			vob.fly = in->read_bool();               // Fly
			vob.magic = in->read_bool();             // Magic
			vob.point = in->read_bool();             // Point
			vob.fall = in->read_bool();              // Fall
			vob.repeat_delay_sec = in->read_float(); // damageRepeatDelaySec
			vob.volume_scale = in->read_float();     // damageVolDownScale
			vob.damage_type = in->read_enum();       // damageCollType
		}

		void mob::parse(mob& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.name = in->read_string();             // focusName
			vob.hp = in->read_int();                  // hitpoints
			vob.damage = in->read_int();              // damage
			vob.movable = in->read_bool();            // moveable
			vob.takable = in->read_bool();            // takeable
			vob.focus_override = in->read_bool();     // focusOverride
			vob.material = in->read_enum();           // soundMaterial
			vob.visual_destroyed = in->read_string(); // visualDestroyed
			vob.owner = in->read_string();            // owner
			vob.owner_guild = in->read_string();      // ownerGuild
			vob.destroyed = in->read_bool();          // isDestroyed
		}

		void mob_inter::parse(mob_inter& vob, archive_reader_ref& in, game_version version) {
			mob::parse(vob, in, version);
			vob.state = in->read_int();                       // stateNum
			vob.target = in->read_string();                   // triggerTarget
			vob.item = in->read_string();                     // useWithItem
			vob.condition_function = in->read_string();       // conditionFunc
			vob.on_state_change_function = in->read_string(); // onStateFunc
			vob.rewind = in->read_bool();                     // rewind
		}

		void mob_container::parse(mob_container& vob, archive_reader_ref& in, game_version version) {
			mob_inter::parse(vob, in, version);
			vob.locked = in->read_bool();        // locked
			vob.key = in->read_string();         // keyInstance
			vob.pick_string = in->read_string(); // pickLockStr
			vob.contents = in->read_string();    // contains
		}

		void trigger_mover::parse(trigger_mover& vob, archive_reader_ref& in, game_version version) {
			trigger::parse(vob, in, version);
			vob.behavior = static_cast<mover_behavior>(in->read_enum()); // moverBehavior
			vob.touch_blocker_damage = in->read_float();                 // touchBlockerDamage
			vob.stay_open_time_sec = in->read_float();                   // stayOpenTimeSec
			vob.locked = in->read_bool();                                // moverLocked
			vob.auto_link = in->read_bool();                             // autoLinkEnabled

			if (version == game_version::gothic_2) {
				vob.auto_rotate = in->read_bool(); // autoRotate
			}

			auto keyframe_count = in->read_word(); // numKeyframes
			if (keyframe_count > 0) {
				vob.speed = in->read_float();                                    // moveSpeed
				vob.lerp_mode = static_cast<mover_lerp_mode>(in->read_enum());   // posLerpType
				vob.speed_mode = static_cast<mover_speed_mode>(in->read_enum()); // speedType

				auto sample_reader = in->read_raw_bytes(); // keyframes

				for (int i = 0; i < keyframe_count; ++i) {
					vob.keyframes.push_back(animation_sample {sample_reader.get_vec3(), sample_reader.get_vec4()});
				}
			}

			vob.sfx_open_start = in->read_string();    // sfxOpenStart
			vob.sfx_open_end = in->read_string();      // sfxOpenEnd
			vob.sfx_transitioning = in->read_string(); // sfxMoving
			vob.sfx_close_start = in->read_string();   // sfxCloseStart
			vob.sfx_close_end = in->read_string();     // sfxCloseEnd
			vob.sfx_lock = in->read_string();          // sfxLock
			vob.sfx_unlock = in->read_string();        // sfxUnlock
			vob.sfx_use_locked = in->read_string();    // sfxUseLocked
		}

		void camera_trj_frame::parse(camera_trj_frame& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.time = in->read_float();                  // time
			vob.roll_angle = in->read_float();            // angleRollDeg
			vob.fov_scale = in->read_float();             // camFOVScale
			vob.motion_type = in->read_enum();            // motionType
			vob.motion_type_fov = in->read_enum();        // motionTypeFOV
			vob.motion_type_roll = in->read_enum();       // motionTypeRoll
			vob.motion_type_time_scale = in->read_enum(); // motionTypeTimeScale
			vob.tension = in->read_float();               // tension
			vob.cam_bias = in->read_float();              // bias
			vob.continuity = in->read_float();            // continuity
			vob.time_scale = in->read_float();            // timeScale
			vob.time_fixed = in->read_bool();             // timeIsFixed

			auto buf = in->read_raw_bytes();
			vob.original_pose = buf.get_mat4x4(); // originalPose
		}

		void cs_camera::parse(cs_camera& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.trajectory_for = in->read_enum();                 // camTrjFOR
			vob.target_trajectory_for = in->read_enum();          // targetTrjFOR
			vob.loop_mode = in->read_enum();                      // loopMode
			vob.lerp_mode = in->read_enum();                      // splLerpMode
			vob.ignore_for_vob_rotation = in->read_bool();        // ignoreFORVobRotCam
			vob.ignore_for_vob_rotation_target = in->read_bool(); // ignoreFORVobRotTarget
			vob.adapt = in->read_bool();                          // adaptToSurroundings
			vob.ease_first = in->read_bool();                     // easeToFirstKey
			vob.ease_last = in->read_bool();                      // easeFromLastKey
			vob.total_duration = in->read_float();                // totalTime
			vob.auto_focus_vob = in->read_string();               // autoCamFocusVobName
			vob.auto_player_movable = in->read_bool();            // autoCamPlayerMovable
			vob.auto_untrigger_last = in->read_bool();            // autoCamUntriggerOnLastKey
			vob.auto_untrigger_last_delay = in->read_float();     // autoCamUntriggerOnLastKeyDelay
			vob.position_count = in->read_int();                  // numPos
			vob.target_count = in->read_int();                    // numTargets

			archive_object frame_obj {};
			while (in->read_object_begin(frame_obj)) {
				if (frame_obj.class_name != "zCCamTrj_KeyFrame:zCVob") {
					fmt::print(stderr, "error: unexpected '{}' in 'zCCSCamera:zCVob'\n", frame_obj.class_name);
					in->skip_object(true);
					continue;
				}

				camera_trj_frame frame {};
				camera_trj_frame::parse(frame, in, version);
				vob.frames.push_back(frame);

				if (!in->read_object_end()) {
					fmt::print(stderr, "warning: not all data consumed of 'zCCamTrj_KeyFrame'\n");
					in->skip_object(true);
				}
			}
		}
	} // namespace vob

	vob_tree vob_tree::_m_empty = vob_tree {vob::base {}};

	vob_tree vob_tree::parse(archive_reader_ref& in, game_version version) {
		vob_tree vob {};

		archive_object obj;
		if (!in->read_object_begin(obj)) {
			throw parser_error("vob_tree: expected object where there was none");
		}

		vob._m_object_id = obj.index;

		if (const auto& it = _vob_type_map.find(obj.class_name); it != _vob_type_map.end()) {
			vob._m_type = it->second;
		} else {
			throw parser_error(fmt::format("vob_tree: expected a '*:zCVob' object but got '{}'", obj.class_name));
		}

		switch (vob._m_type) {
		case vob_type::zCCamTrj_KeyFrame:
		case vob_type::zCVobLevelCompo:
		case vob_type::zCVobStartpoint:
		case vob_type::zCVobScreenFX:
		case vob_type::zCVobStair:
		case vob_type::zCVobSpot:
		case vob_type::zCVob:
			vob._m_content = vob::base {};
			vob::base::parse(std::get<vob::base>(vob._m_content), in, version);
			break;
		case vob_type::zCCSCamera:
			vob._m_content = vob::cs_camera {};
			vob::cs_camera::parse(std::get<vob::cs_camera>(vob._m_content), in, version);
			break;
		case vob_type::zCVobAnimate:
			vob._m_content = vob::animate {};
			vob::animate::parse(std::get<vob::animate>(vob._m_content), in, version);
			break;
		case vob_type::zCZoneVobFarPlane:
		case vob_type::zCZoneVobFarPlaneDefault:
			vob._m_content = vob::zone_far_plane {};
			vob::zone_far_plane::parse(std::get<vob::zone_far_plane>(vob._m_content), in, version);
			break;
		case vob_type::zCZoneZFogDefault:
		case vob_type::zCZoneZFog:
			vob._m_content = vob::zone_fog {};
			vob::zone_fog::parse(std::get<vob::zone_fog>(vob._m_content), in, version);
			break;
		case vob_type::zCVobLensFlare:
			vob._m_content = vob::lens_flare {};
			vob::lens_flare::parse(std::get<vob::lens_flare>(vob._m_content), in, version);
			break;
		case vob_type::oCItem:
			vob._m_content = vob::item {};
			vob::item::parse(std::get<vob::item>(vob._m_content), in, version);
			break;
		case vob_type::zCTrigger:
		case vob_type::oCCSTrigger:
			vob._m_content = vob::trigger {};
			vob::trigger::parse(std::get<vob::trigger>(vob._m_content), in, version);
			break;
		case vob_type::oCMOB:
			vob._m_content = vob::mob {};
			vob::mob::parse(std::get<vob::mob>(vob._m_content), in, version);
			break;
		case vob_type::oCMobInter:
		case vob_type::oCMobLadder:
		case vob_type::oCMobSwitch:
		case vob_type::oCMobWheel:
		case vob_type::oCMobBed:
			vob._m_content = vob::mob_inter {};
			vob::mob_inter::parse(std::get<vob::mob_inter>(vob._m_content), in, version);
			break;
		case vob_type::oCMobFire:
			vob._m_content = vob::mob_fire {};
			vob::mob_fire::parse(std::get<vob::mob_fire>(vob._m_content), in, version);
			break;
		case vob_type::oCMobContainer:
			vob._m_content = vob::mob_container {};
			vob::mob_container::parse(std::get<vob::mob_container>(vob._m_content), in, version);
			break;
		case vob_type::oCMobDoor:
			vob._m_content = vob::mob_door {};
			vob::mob_door::parse(std::get<vob::mob_door>(vob._m_content), in, version);
			break;
		case vob_type::zCPFXController:
			vob._m_content = vob::pfx_controller {};
			vob::pfx_controller::parse(std::get<vob::pfx_controller>(vob._m_content), in, version);
			break;
		case vob_type::zCVobLight:
			vob._m_content = vob::light {};
			vob::light::parse(std::get<vob::light>(vob._m_content), in, version);
			break;
		case vob_type::zCVobSound:
			vob._m_content = vob::sound {};
			vob::sound::parse(std::get<vob::sound>(vob._m_content), in, version);
			break;
		case vob_type::zCVobSoundDaytime:
			vob._m_content = vob::sound_daytime {};
			vob::sound_daytime::parse(std::get<vob::sound_daytime>(vob._m_content), in, version);
			break;
		case vob_type::oCZoneMusic:
		case vob_type::oCZoneMusicDefault:
			vob._m_content = vob::zone_music {};
			vob::zone_music::parse(std::get<vob::zone_music>(vob._m_content), in, version);
			break;
		case vob_type::zCMessageFilter:
			vob._m_content = vob::message_filter {};
			vob::message_filter::parse(std::get<vob::message_filter>(vob._m_content), in, version);
			break;
		case vob_type::zCCodeMaster:
			vob._m_content = vob::code_master {};
			vob::code_master::parse(std::get<vob::code_master>(vob._m_content), in, version);
			break;
		case vob_type::zCTriggerList:
			vob._m_content = vob::trigger_list {};
			vob::trigger_list::parse(std::get<vob::trigger_list>(vob._m_content), in, version);
			break;
		case vob_type::oCTriggerScript:
			vob._m_content = vob::trigger_script {};
			vob::trigger_script::parse(std::get<vob::trigger_script>(vob._m_content), in, version);
			break;
		case vob_type::zCMover:
			vob._m_content = vob::trigger_mover {};
			vob::trigger_mover::parse(std::get<vob::trigger_mover>(vob._m_content), in, version);
			break;
		case vob_type::oCTriggerChangeLevel:
			vob._m_content = vob::trigger_change_level {};
			vob::trigger_change_level::parse(std::get<vob::trigger_change_level>(vob._m_content), in, version);
			break;
		case vob_type::zCTriggerWorldStart:
			vob._m_content = vob::trigger_world_start {};
			vob::trigger_world_start::parse(std::get<vob::trigger_world_start>(vob._m_content), in, version);
			break;
		case vob_type::oCTouchDamage:
			vob._m_content = vob::touch_damage {};
			vob::touch_damage::parse(std::get<vob::touch_damage>(vob._m_content), in, version);
			break;
		case vob_type::zCTriggerUntouch:
			vob._m_content = vob::trigger_untouch {};
			vob::trigger_untouch::parse(std::get<vob::trigger_untouch>(vob._m_content), in, version);
			break;
		case vob_type::zCEarthquake:
			vob._m_content = vob::earthquake {};
			vob::earthquake::parse(std::get<vob::earthquake>(vob._m_content), in, version);
			break;
		case vob_type::zCMoverController:
			vob._m_content = vob::mover_controller {};
			vob::mover_controller::parse(std::get<vob::mover_controller>(vob._m_content), in, version);
			break;
		case vob_type::unknown:
			break;
		}

		if (!in->read_object_end()) {
			fmt::print(stderr, "warning: not all data consumed of vob '{}'\n", obj.class_name);
			in->skip_object(true);
		}

		auto child_count = in->read_int();
		vob._m_children.reserve(child_count);

		for (int i = 0; i < child_count; ++i) {
			vob._m_children.emplace_back(vob_tree::parse(in, version));
		}

		return vob;
	}

	const vob::base& vob_tree::as_base() const {
		switch (_m_type) {
		case vob_type::zCCamTrj_KeyFrame:
		case vob_type::zCVobLevelCompo:
		case vob_type::zCVobStartpoint:
		case vob_type::zCVobScreenFX:
		case vob_type::zCVobStair:
		case vob_type::zCVobSpot:
		case vob_type::zCVob:
			return get<vob::base>();
		case vob_type::zCCSCamera:
			return get<vob::cs_camera>();
		case vob_type::zCVobAnimate:
			return get<vob::animate>();
		case vob_type::zCZoneVobFarPlane:
		case vob_type::zCZoneVobFarPlaneDefault:
			return get<vob::zone_far_plane>();
		case vob_type::zCZoneZFogDefault:
		case vob_type::zCZoneZFog:
			return get<vob::zone_fog>();
		case vob_type::zCVobLensFlare:
			return get<vob::lens_flare>();
		case vob_type::oCItem:
			return get<vob::item>();
		case vob_type::zCTrigger:
		case vob_type::oCCSTrigger:
			return get<vob::trigger>();
		case vob_type::oCMOB:
			return get<vob::mob>();
		case vob_type::oCMobInter:
		case vob_type::oCMobLadder:
		case vob_type::oCMobSwitch:
		case vob_type::oCMobWheel:
		case vob_type::oCMobBed:
			return get<vob::mob_inter>();
		case vob_type::oCMobFire:
			return get<vob::mob_fire>();
		case vob_type::oCMobContainer:
			return get<vob::mob_container>();
		case vob_type::oCMobDoor:
			return get<vob::mob_door>();
		case vob_type::zCPFXController:
			return get<vob::pfx_controller>();
		case vob_type::zCVobLight:
			return get<vob::light>();
		case vob_type::zCVobSound:
			return get<vob::sound>();
		case vob_type::zCVobSoundDaytime:
			return get<vob::sound_daytime>();
		case vob_type::oCZoneMusic:
		case vob_type::oCZoneMusicDefault:
			return get<vob::zone_music>();
		case vob_type::zCMessageFilter:
			return get<vob::message_filter>();
		case vob_type::zCCodeMaster:
			return get<vob::code_master>();
		case vob_type::zCTriggerList:
			return get<vob::trigger_list>();
		case vob_type::oCTriggerScript:
			return get<vob::trigger_script>();
		case vob_type::zCMover:
			return get<vob::trigger_mover>();
		case vob_type::oCTriggerChangeLevel:
			return get<vob::trigger_change_level>();
		case vob_type::zCTriggerWorldStart:
			return get<vob::trigger_world_start>();
		case vob_type::oCTouchDamage:
			return get<vob::touch_damage>();
		case vob_type::zCTriggerUntouch:
			return get<vob::trigger_untouch>();
		case vob_type::zCEarthquake:
			return get<vob::earthquake>();
		case vob_type::zCMoverController:
			return get<vob::mover_controller>();
		default:
			throw std::runtime_error {"cannot retrieve VOb data of unknown VOb"};
		}
	}
} // namespace phoenix
