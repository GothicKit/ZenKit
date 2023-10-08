// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/world/VobTree.hh"
#include "zenkit/Archive.hh"
#include "zenkit/vobs/Camera.hh"
#include "zenkit/vobs/Light.hh"
#include "zenkit/vobs/Misc.hh"
#include "zenkit/vobs/MovableObject.hh"
#include "zenkit/vobs/Sound.hh"
#include "zenkit/vobs/Trigger.hh"
#include "zenkit/vobs/VirtualObject.hh"
#include "zenkit/vobs/Zone.hh"

#include "../Internal.hh"

#include <unordered_map>

namespace zenkit {
	static std::unordered_map<std::string, VobType> TYPES = {
	    {"zCVob", VobType::zCVob},
	    {"zCVobLevelCompo:zCVob", VobType::zCVobLevelCompo},
	    {"oCItem:zCVob", VobType::oCItem},
	    {"oCNpc:zCVob", VobType::oCNpc},
	    {"oCMOB:zCVob", VobType::oCMOB},
	    {"oCMobInter:oCMOB:zCVob", VobType::oCMobInter},
	    {"oCMobBed:oCMobInter:oCMOB:zCVob", VobType::oCMobBed},
	    {"oCMobFire:oCMobInter:oCMOB:zCVob", VobType::oCMobFire},
	    {"oCMobLadder:oCMobInter:oCMOB:zCVob", VobType::oCMobLadder},
	    {"oCMobSwitch:oCMobInter:oCMOB:zCVob", VobType::oCMobSwitch},
	    {"oCMobWheel:oCMobInter:oCMOB:zCVob", VobType::oCMobWheel},
	    {"oCMobContainer:oCMobInter:oCMOB:zCVob", VobType::oCMobContainer},
	    {"oCMobDoor:oCMobInter:oCMOB:zCVob", VobType::oCMobDoor},
	    {"zCPFXControler:zCVob", VobType::zCPFXController},
	    {"zCVobAnimate:zCVob", VobType::zCVobAnimate},
	    {"zCVobLensFlare:zCVob", VobType::zCVobLensFlare},
	    {"zCVobLight:zCVob", VobType::zCVobLight},
	    {"zCVobSpot:zCVob", VobType::zCVobSpot},
	    {"zCVobStartpoint:zCVob", VobType::zCVobStartpoint},
	    {"zCVobSound:zCVob", VobType::zCVobSound},
	    {"zCVobSoundDaytime:zCVobSound:zCVob", VobType::zCVobSoundDaytime},
	    {"oCZoneMusic:zCVob", VobType::oCZoneMusic},
	    {"oCZoneMusicDefault:oCZoneMusic:zCVob", VobType::oCZoneMusicDefault},
	    {"zCZoneZFog:zCVob", VobType::zCZoneZFog},
	    {"zCZoneZFogDefault:zCZoneZFog:zCVob", VobType::zCZoneZFogDefault},
	    {"zCZoneVobFarPlane:zCVob", VobType::zCZoneVobFarPlane},
	    {"zCZoneVobFarPlaneDefault:zCZoneVobFarPlane:zCVob", VobType::zCZoneVobFarPlaneDefault},
	    {"zCMessageFilter:zCVob", VobType::zCMessageFilter},
	    {"zCCodeMaster:zCVob", VobType::zCCodeMaster},
	    {"zCTrigger:zCVob", VobType::zCTrigger},
	    {"zCTriggerList:zCTrigger:zCVob", VobType::zCTriggerList},
	    {"oCTriggerScript:zCTrigger:zCVob", VobType::oCTriggerScript},
	    {"zCMover:zCTrigger:zCVob", VobType::zCMover},
	    {"oCTriggerChangeLevel:zCTrigger:zCVob", VobType::oCTriggerChangeLevel},
	    {"zCTriggerWorldStart:zCVob", VobType::zCTriggerWorldStart},
	    {"zCTriggerUntouch:zCVob", VobType::zCTriggerUntouch},
	    {"zCCSCamera:zCVob", VobType::zCCSCamera},
	    {"zCCamTrj_KeyFrame:zCVob", VobType::zCCamTrj_KeyFrame},
	    {"oCTouchDamage:zCTouchDamage:zCVob", VobType::oCTouchDamage},
	    {"zCEarthquake:zCVob", VobType::zCEarthquake},
	    {"zCMoverControler:zCVob", VobType::zCMoverController},
	    {"zCVobScreenFX:zCVob", VobType::zCVobScreenFX},
	    {"zCVobStair:zCVob", VobType::zCVobStair},
	    {"oCCSTrigger:zCTrigger:zCVob", VobType::oCCSTrigger},
	    {"\xA7", VobType::ignored}, // some sort of padding object, probably. seems to be always empty
	};

	std::unique_ptr<VirtualObject> parse_vob_tree(ReadArchive& in, GameVersion version) {
		std::vector<std::unique_ptr<VirtualObject>> vobs {};

		ArchiveObject obj;
		if (!in.read_object_begin(obj)) {
			throw zenkit::ParserError("vob_tree: expected object where there was none");
		}

		VobType type;

		if (auto const& it = TYPES.find(obj.class_name); it != TYPES.end()) {
			type = it->second;
		} else {
			type = VobType::unknown;
		}

		std::unique_ptr<VirtualObject> object;

		switch (type) {
		case VobType::zCCamTrj_KeyFrame:
		case VobType::zCVobLevelCompo:
		case VobType::zCVobStartpoint:
		case VobType::zCVobStair:
		case VobType::zCVobSpot:
		case VobType::zCVob:
			object = std::make_unique<VirtualObject>();
			break;
		case VobType::zCVobScreenFX:
			object = std::make_unique<vobs::ScreenEffect>();
			break;
		case VobType::zCCSCamera:
			object = std::make_unique<vobs::CutsceneCamera>();
			break;
		case VobType::zCVobAnimate:
			object = std::make_unique<vobs::Animate>();
			break;
		case VobType::zCZoneVobFarPlane:
		case VobType::zCZoneVobFarPlaneDefault:
			object = std::make_unique<vobs::ZoneFarPlane>();
			break;
		case VobType::zCZoneZFogDefault:
		case VobType::zCZoneZFog:
			object = std::make_unique<vobs::ZoneFog>();
			break;
		case VobType::zCVobLensFlare:
			object = std::make_unique<vobs::LensFlare>();
			break;
		case VobType::oCItem:
			object = std::make_unique<vobs::Item>();
			break;
		case VobType::zCTrigger:
		case VobType::oCCSTrigger:
			object = std::make_unique<vobs::Trigger>();
			break;
		case VobType::oCMOB:
			object = std::make_unique<vobs::MovableObject>();
			break;
		case VobType::oCMobInter:
		case VobType::oCMobLadder:
		case VobType::oCMobSwitch:
		case VobType::oCMobWheel:
		case VobType::oCMobBed:
			object = std::make_unique<vobs::InteractiveObject>();
			break;
		case VobType::oCMobFire:
			object = std::make_unique<vobs::Fire>();
			break;
		case VobType::oCMobContainer:
			object = std::make_unique<vobs::Container>();
			break;
		case VobType::oCMobDoor:
			object = std::make_unique<vobs::Door>();
			break;
		case VobType::zCPFXController:
			object = std::make_unique<vobs::ParticleEffectController>();
			break;
		case VobType::zCVobLight:
			object = std::make_unique<vobs::Light>();
			break;
		case VobType::zCVobSound:
			object = std::make_unique<vobs::Sound>();
			break;
		case VobType::zCVobSoundDaytime:
			object = std::make_unique<vobs::SoundDaytime>();
			break;
		case VobType::oCZoneMusic:
		case VobType::oCZoneMusicDefault:
			object = std::make_unique<vobs::ZoneMusic>();
			break;
		case VobType::zCMessageFilter:
			object = std::make_unique<vobs::MessageFilter>();
			break;
		case VobType::zCCodeMaster:
			object = std::make_unique<vobs::CodeMaster>();
			break;
		case VobType::zCTriggerList:
			object = std::make_unique<vobs::TriggerList>();
			break;
		case VobType::oCTriggerScript:
			object = std::make_unique<vobs::TriggerScript>();
			break;
		case VobType::zCMover:
			object = std::make_unique<vobs::Mover>();
			break;
		case VobType::oCTriggerChangeLevel:
			object = std::make_unique<vobs::TriggerChangeLevel>();
			break;
		case VobType::zCTriggerWorldStart:
			object = std::make_unique<vobs::TriggerWorldStart>();
			break;
		case VobType::oCTouchDamage:
			object = std::make_unique<vobs::TouchDamage>();
			break;
		case VobType::zCTriggerUntouch:
			object = std::make_unique<vobs::TriggerUntouch>();
			break;
		case VobType::zCEarthquake:
			object = std::make_unique<vobs::Earthquake>();
			break;
		case VobType::zCMoverController:
			object = std::make_unique<vobs::MoverController>();
			break;
		case VobType::oCNpc:
			object = std::make_unique<vobs::Npc>();
			break;
		case VobType::ignored:
			break;
		case VobType::unknown:
			ZKLOGW("VobTree",
			       "Encountered unknown VOb [%s %s %u %u]",
			       obj.object_name.c_str(),
			       obj.class_name.c_str(),
			       obj.version,
			       obj.index);
			break;
		}

		if (object) {
			object->load(in, version);
		}

		if (!in.read_object_end()) {
			ZKLOGW("VobTree", "\"%s\" not fully parsed", obj.class_name.c_str());
			in.skip_object(true);
		}

		auto child_count = static_cast<size_t>(in.read_int());
		if (object == nullptr) {
			std::function<void(size_t)> skip;
			skip = [&skip, &in](size_t count) {
				for (auto i = 0u; i < count; ++i) {
					in.skip_object(false);

					auto num_children = static_cast<size_t>(in.read_int());
					skip(num_children);
				}
			};

			skip(child_count);
			return nullptr;
		}

		object->children.reserve(child_count);
		object->id = obj.index;
		object->type = type;

		for (auto i = 0u; i < child_count; ++i) {
			auto child = parse_vob_tree(in, version);
			if (child == nullptr) continue;

			object->children.push_back(std::move(child));
		}

		return object;
	}
} // namespace zenkit
