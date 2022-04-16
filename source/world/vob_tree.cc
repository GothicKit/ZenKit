// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
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
	    {"\xA7", vob_type::unknown},  // some sort of padding object, probably. seems to be always empty
	};

	namespace vob {
		void base::parse(vob::base& vob, archive_reader_ref& in, game_version version) {
			auto packed = in->read_int() != 0;

			if (packed) {
				vector_reader bin {in->read_raw_bytes()};

				vob.bbox[0] = bin.read_vec3();
				vob.bbox[1] = bin.read_vec3();
				vob.position = bin.read_vec3();
				vob.rotation[0] = bin.read_vec3();
				vob.rotation[1] = bin.read_vec3();
				vob.rotation[2] = bin.read_vec3();

				u8 bit0 = bin.read_u8();
				u16 bit1;

				if (version == game_version::gothic_1) {
					bit1 = bin.read_u8();
				} else {
					bit1 = bin.read_u16();
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
				bool has_visual_object = static_cast<bool>((bit1 & 0b000000000001000u) >> 3u);
				bool has_ai_object = static_cast<bool>((bit1 & 0b000000000010000u) >> 4u);
				// bool has_event_man_object = static_cast<bool>((bit1 & 0b000000000100000u) >> 5u);
				vob.physics_enabled = static_cast<bool>((bit1 & 0b000000001000000u) >> 6u);

				if (version == game_version::gothic_2) {
					vob.animation_mode = (bit1 & 0b000000110000000u >> 7u);
					vob.bias = static_cast<s32>((bit1 & 0b011111000000000u) >> 9u);
					vob.ambient = static_cast<bool>((bit1 & 0b100000000000000u) >> 14u);

					vob.animation_strength = bin.read_f32();
					vob.far_clip_scale = bin.read_f32();
				}

				if (has_preset_name) {
					vob.preset_name = in->read_string();
				}

				if (has_vob_name) {
					vob.vob_name = in->read_string();
				}

				if (has_visual_name) {
					vob.visual_name = in->read_string();
				}

				if (has_visual_object) {
					in->skip_object(false);
				}

				if (has_ai_object) {
					in->skip_object(false);
				}
			} else {
				vob.preset_name = in->read_string();

				auto [min, max] = in->read_bbox();
				vob.bbox[0] = min;
				vob.bbox[1] = max;

				vob.rotation = in->read_mat3x3();
				vob.position = in->read_vec3();

				vob.vob_name = in->read_string();
				vob.visual_name = in->read_string();
				vob.show_visual = in->read_bool();
				vob.camera_alignment = in->read_enum();

				if (version == game_version::gothic_1) {
					vob.cd_static = in->read_bool();
					vob.cd_dynamic = in->read_bool();
					vob.vob_static = in->read_bool();
					vob.dynamic_shadows = in->read_enum();
					// vob._m_animation_mode = in->read_byte();
				} else {
					vob.animation_mode = in->read_enum();
					vob.animation_strength = in->read_float();
					vob.far_clip_scale = in->read_float();
					vob.cd_static = in->read_bool();
					vob.cd_dynamic = in->read_bool();
					vob.vob_static = in->read_bool();
					vob.dynamic_shadows = in->read_enum();
					vob.bias = in->read_int();
					vob.ambient = in->read_bool();
				}

				// visual
				in->skip_object(false);

				// ai
				in->skip_object(false);
			}
		}

		void light::parse(light& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);

			vob.preset_name = in->read_string();
			vob.type = in->read_enum();
			vob.range = in->read_float();
			vob.colour = in->read_color();
			vob.cone_angle = in->read_float();
			vob.is_static = in->read_bool();
			vob.quality = in->read_enum();
			vob.lensflare_fx = in->read_string();

			if (!vob.is_static) {
				vob.on = in->read_bool();
				vob.range_animation_scale = in->read_string();
				vob.range_animation_fps = in->read_float();
				vob.range_animation_smooth = in->read_bool();
				vob.color_animation_list = in->read_string();
				vob.color_animation_fps = in->read_float();
				vob.color_animation_smooth = in->read_bool();

				if (version == game_version::gothic_2) {
					vob.can_move = in->read_bool();
				}
			}
		}

		void sound::parse(sound& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.volume = in->read_float();
			vob.mode = in->read_enum();
			vob.random_delay = in->read_float();
			vob.random_delay_var = in->read_float();
			vob.initially_playing = in->read_bool();
			vob.ambient3d = in->read_bool();
			vob.obstruction = in->read_bool();
			vob.cone_angle = in->read_float();
			vob.volume_type = in->read_enum();
			vob.radius = in->read_float();
			vob.name = in->read_string();
		}

		void zone_music::parse(zone_music& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.enabled = in->read_bool();
			vob.priority = in->read_int();
			vob.ellipsoid = in->read_bool();
			vob.reverb = in->read_float();
			vob.volume = in->read_float();
			vob.loop = in->read_bool();
		}

		void code_master::parse(code_master& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.target = in->read_string();
			vob.ordered = in->read_bool();
			vob.first_false_is_failure = in->read_bool();
			vob.failure_target = in->read_string();
			vob.untriggered_cancels = in->read_bool();

			auto slave_count = in->read_byte();
			for (int i = 0; i < slave_count; ++i) {
				vob.slaves.emplace_back(in->read_string());
			}
		}

		void trigger::parse(trigger& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.target = in->read_string();
			vob.flags = in->read_raw_bytes()[0];
			vob.filter_flags = in->read_raw_bytes()[0];
			vob.vob_target = in->read_string();
			vob.max_activation_count = in->read_int();
			vob.retrigger_delay_sec = in->read_float();
			vob.damage_threshold = in->read_float();
			vob.fire_delay_sec = in->read_float();
		}

		void touch_damage::parse(touch_damage& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.damage = in->read_float();
			vob.barrier = in->read_bool();
			vob.blunt = in->read_bool();
			vob.edge = in->read_bool();
			vob.fire = in->read_bool();
			vob.fly = in->read_bool();
			vob.magic = in->read_bool();
			vob.point = in->read_bool();
			vob.fall = in->read_bool();
			vob.repeat_delay_sec = in->read_float();
			vob.volume_scale = in->read_float();
			vob.damage_type = in->read_enum();
		}

		void mob::parse(mob& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.name = in->read_string();
			vob.hp = in->read_int();
			vob.damage = in->read_int();
			vob.movable = in->read_bool();
			vob.takable = in->read_bool();
			vob.focus_override = in->read_bool();
			vob.material = in->read_enum();
			vob.visual_destroyed = in->read_string();
			vob.owner = in->read_string();
			vob.owner_guild = in->read_string();
			vob.destroyed = in->read_bool();
		}

		void mob_inter::parse(mob_inter& vob, archive_reader_ref& in, game_version version) {
			mob::parse(vob, in, version);
			vob.state = in->read_int();
			vob.target = in->read_string();
			vob.item = in->read_string();
			vob.condition_function = in->read_string();
			vob.on_state_change_function = in->read_string();
			vob.rewind = in->read_bool();
		}

		void mob_container::parse(mob_container& vob, archive_reader_ref& in, game_version version) {
			mob_inter::parse(vob, in, version);
			vob.locked = in->read_bool();
			vob.key = in->read_string();
			vob.pick_string = in->read_string();
			vob.contents = in->read_string();
		}

		void trigger_mover::parse(trigger_mover& vob, archive_reader_ref& in, game_version version) {
			trigger::parse(vob, in, version);
			vob.behavior = in->read_enum();
			vob.touch_blocker_damage = in->read_float();
			vob.stay_open_time_sec = in->read_float();
			vob.locked = in->read_bool();
			vob.auto_link = in->read_bool();

			if (version == game_version::gothic_2) {
				vob.auto_rotate = in->read_bool();
			}

			auto keyframe_count = in->read_word();
			if (keyframe_count > 0) {
				vob.speed = in->read_float();
				vob.lerp_mode = in->read_enum();
				vob.speed_mode = in->read_enum();

				vector_reader sample_reader {in->read_raw_bytes()};

				for (int i = 0; i < keyframe_count; ++i) {
					vob.keyframes.push_back(animation_sample {sample_reader.read_vec3(), sample_reader.read_vec4()});
				}
			}

			vob.sfx_open_start = in->read_string();
			vob.sfx_open_end = in->read_string();
			vob.sfx_transitioning = in->read_string();
			vob.sfx_close_start = in->read_string();
			vob.sfx_close_end = in->read_string();
			vob.sfx_lock = in->read_string();
			vob.sfx_unlock = in->read_string();
			vob.sfx_use_locked = in->read_string();
		}

		void camera_trj_frame::parse(camera_trj_frame& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.field1 = in->read_float();
		    vob.field2 = in->read_float();
		    vob.field3 = in->read_float();
		    vob.field4 = in->read_enum();
		    vob.field5 = in->read_enum();
		    vob.field6 = in->read_enum();
		    vob.field7 = in->read_enum();
		    vob.field8 = in->read_float();
		    vob.field9 = in->read_float();
		    vob.field10 = in->read_float();
		    vob.field11 = in->read_float();
		    vob.field12 = in->read_bool();

			vob.blob = in->read_raw_bytes();
		}

		void cs_camera::parse(cs_camera& vob, archive_reader_ref& in, game_version version) {
			base::parse(vob, in, version);
			vob.field1 = in->read_enum();
			vob.field2 = in->read_enum();
			vob.field3 = in->read_enum();
			vob.field4 = in->read_enum();
			vob.field5 = in->read_bool();
			vob.field6 = in->read_bool();
			vob.field7 = in->read_bool();
			vob.field8 = in->read_bool();
			vob.field9 = in->read_bool();
			vob.field10 = in->read_float();
			vob.field11 = in->read_string();
			vob.field12 = in->read_bool();
			vob.field13 = in->read_bool();
			vob.field14 = in->read_float();
			vob.field15 = in->read_int();
			vob.field16 = in->read_int();

			archive_object frame_obj {};
			while (in->read_object_begin(frame_obj)) {
				camera_trj_frame frame {};
				camera_trj_frame::parse(frame, in, version);
				vob.frames.push_back(frame);

				if (!in->read_object_end()) {
					fmt::print(stderr, "warning: not all data consumed of 'zCCamTrj_KeyFrame'");
					in->skip_object(true);
				}
			}
		}
	} // namespace vob

	vob_tree vob_tree::parse(archive_reader_ref& in, game_version version) {
		vob_tree vob {};

		archive_object obj;
		if (!in->read_object_begin(obj)) {
			throw parser_error("vob_tree: expected object where there was none");
		}

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
		case vob_type::oCItem:
		case vob_type::zCVobLensFlare:
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
} // namespace phoenix
