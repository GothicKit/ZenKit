// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/world/vob_tree.hh>

#include <phoenix/vobs/camera.hh>
#include <phoenix/vobs/light.hh>
#include <phoenix/vobs/misc.hh>
#include <phoenix/vobs/mob.hh>
#include <phoenix/vobs/sound.hh>
#include <phoenix/vobs/trigger.hh>
#include <phoenix/vobs/vob.hh>
#include <phoenix/vobs/zone.hh>

#include <fmt/format.h>
#include <unordered_map>

namespace phoenix {
	static std::unordered_map<std::string, vob_type> vob_type_map = {
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

	std::unique_ptr<vob> parse_vob_tree(archive_reader_ref& in, game_version version) {
		std::vector<std::unique_ptr<vob>> vobs {};

		archive_object obj;
		if (!in->read_object_begin(obj)) {
			throw parser_error("vob_tree: expected object where there was none");
		}

		vob_type type;

		if (const auto& it = vob_type_map.find(obj.class_name); it != vob_type_map.end()) {
			type = it->second;
		} else {
			throw parser_error(fmt::format("vob_tree: expected a '*:zCVob' object but got '{}'", obj.class_name));
		}

		std::unique_ptr<vob> object;

		switch (type) {
		case vob_type::zCCamTrj_KeyFrame:
		case vob_type::zCVobLevelCompo:
		case vob_type::zCVobStartpoint:
		case vob_type::zCVobScreenFX:
		case vob_type::zCVobStair:
		case vob_type::zCVobSpot:
		case vob_type::zCVob:
			object = std::make_unique<vob>();
			vob::parse(*object, in, version);
			break;
		case vob_type::zCCSCamera:
			object = std::make_unique<vobs::cs_camera>();
			vobs::cs_camera::parse(dynamic_cast<vobs::cs_camera&>(*object), in, version);
			break;
		case vob_type::zCVobAnimate:
			object = std::make_unique<vobs::animate>();
			vobs::animate::parse(dynamic_cast<vobs::animate&>(*object), in, version);
			break;
		case vob_type::zCZoneVobFarPlane:
		case vob_type::zCZoneVobFarPlaneDefault:
			object = std::make_unique<vobs::zone_far_plane>();
			vobs::zone_far_plane::parse(dynamic_cast<vobs::zone_far_plane&>(*object), in, version);
			break;
		case vob_type::zCZoneZFogDefault:
		case vob_type::zCZoneZFog:
			object = std::make_unique<vobs::zone_fog>();
			vobs::zone_fog::parse(dynamic_cast<vobs::zone_fog&>(*object), in, version);
			break;
		case vob_type::zCVobLensFlare:
			object = std::make_unique<vobs::lens_flare>();
			vobs::lens_flare::parse(dynamic_cast<vobs::lens_flare&>(*object), in, version);
			break;
		case vob_type::oCItem:
			object = std::make_unique<vobs::item>();
			vobs::item::parse(dynamic_cast<vobs::item&>(*object), in, version);
			break;
		case vob_type::zCTrigger:
		case vob_type::oCCSTrigger:
			object = std::make_unique<vobs::trigger>();
			vobs::trigger::parse(dynamic_cast<vobs::trigger&>(*object), in, version);
			break;
		case vob_type::oCMOB:
			object = std::make_unique<vobs::mob>();
			vobs::mob::parse(dynamic_cast<vobs::mob&>(*object), in, version);
			break;
		case vob_type::oCMobInter:
		case vob_type::oCMobLadder:
		case vob_type::oCMobSwitch:
		case vob_type::oCMobWheel:
		case vob_type::oCMobBed:
			object = std::make_unique<vobs::mob_inter>();
			vobs::mob_inter::parse(dynamic_cast<vobs::mob_inter&>(*object), in, version);
			break;
		case vob_type::oCMobFire:
			object = std::make_unique<vobs::mob_fire>();
			vobs::mob_fire::parse(dynamic_cast<vobs::mob_fire&>(*object), in, version);
			break;
		case vob_type::oCMobContainer:
			object = std::make_unique<vobs::mob_container>();
			vobs::mob_container::parse(dynamic_cast<vobs::mob_container&>(*object), in, version);
			break;
		case vob_type::oCMobDoor:
			object = std::make_unique<vobs::mob_door>();
			vobs::mob_door::parse(dynamic_cast<vobs::mob_door&>(*object), in, version);
			break;
		case vob_type::zCPFXController:
			object = std::make_unique<vobs::pfx_controller>();
			vobs::pfx_controller::parse(dynamic_cast<vobs::pfx_controller&>(*object), in, version);
			break;
		case vob_type::zCVobLight:
			object = std::make_unique<vobs::light>();
			vobs::light::parse((vobs::light&) *object, in, version);
			break;
		case vob_type::zCVobSound:
			object = std::make_unique<vobs::sound>();
			vobs::sound::parse(dynamic_cast<vobs::sound&>(*object), in, version);
			break;
		case vob_type::zCVobSoundDaytime:
			object = std::make_unique<vobs::sound_daytime>();
			vobs::sound_daytime::parse(dynamic_cast<vobs::sound_daytime&>(*object), in, version);
			break;
		case vob_type::oCZoneMusic:
		case vob_type::oCZoneMusicDefault:
			object = std::make_unique<vobs::zone_music>();
			vobs::zone_music::parse(dynamic_cast<vobs::zone_music&>(*object), in, version);
			break;
		case vob_type::zCMessageFilter:
			object = std::make_unique<vobs::message_filter>();
			vobs::message_filter::parse(dynamic_cast<vobs::message_filter&>(*object), in, version);
			break;
		case vob_type::zCCodeMaster:
			object = std::make_unique<vobs::code_master>();
			vobs::code_master::parse(dynamic_cast<vobs::code_master&>(*object), in, version);
			break;
		case vob_type::zCTriggerList:
			object = std::make_unique<vobs::trigger_list>();
			vobs::trigger_list::parse(dynamic_cast<vobs::trigger_list&>(*object), in, version);
			break;
		case vob_type::oCTriggerScript:
			object = std::make_unique<vobs::trigger_script>();
			vobs::trigger_script::parse(dynamic_cast<vobs::trigger_script&>(*object), in, version);
			break;
		case vob_type::zCMover:
			object = std::make_unique<vobs::trigger_mover>();
			vobs::trigger_mover::parse(dynamic_cast<vobs::trigger_mover&>(*object), in, version);
			break;
		case vob_type::oCTriggerChangeLevel:
			object = std::make_unique<vobs::trigger_change_level>();
			vobs::trigger_change_level::parse(dynamic_cast<vobs::trigger_change_level&>(*object), in, version);
			break;
		case vob_type::zCTriggerWorldStart:
			object = std::make_unique<vobs::trigger_world_start>();
			vobs::trigger_world_start::parse(dynamic_cast<vobs::trigger_world_start&>(*object), in, version);
			break;
		case vob_type::oCTouchDamage:
			object = std::make_unique<vobs::touch_damage>();
			vobs::touch_damage::parse(dynamic_cast<vobs::touch_damage&>(*object), in, version);
			break;
		case vob_type::zCTriggerUntouch:
			object = std::make_unique<vobs::trigger_untouch>();
			vobs::trigger_untouch::parse(dynamic_cast<vobs::trigger_untouch&>(*object), in, version);
			break;
		case vob_type::zCEarthquake:
			object = std::make_unique<vobs::earthquake>();
			vobs::earthquake::parse(dynamic_cast<vobs::earthquake&>(*object), in, version);
			break;
		case vob_type::zCMoverController:
			object = std::make_unique<vobs::mover_controller>();
			vobs::mover_controller::parse(dynamic_cast<vobs::mover_controller&>(*object), in, version);
			break;
		case vob_type::unknown:
			break;
		}

		if (!in->read_object_end()) {
			fmt::print(stderr, "warning: not all data consumed of vob '{}'\n", obj.class_name);
			in->skip_object(true);
		}

		auto child_count = in->read_int();
		if (object == nullptr) {
			std::function<void(int)> skip;
			skip = [&skip, &in](int count) {
				for (int i = 0; i < count; ++i) {
					in->skip_object(false);

					auto child_count = in->read_int();
					skip(child_count);
				}
			};

			skip(child_count);
			return nullptr;
		}

		object->children.reserve(child_count);
		object->id = obj.index;
		object->type = type;

		for (int i = 0; i < child_count; ++i) {
			auto child = parse_vob_tree(in, version);
			if (child == nullptr)
				continue;

			object->children.push_back(std::move(child));
		}

		return object;
	}

} // namespace phoenix
