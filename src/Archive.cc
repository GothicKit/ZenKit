// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include "zenkit/Material.hh"
#include "zenkit/vobs/Camera.hh"
#include "zenkit/vobs/Light.hh"
#include "zenkit/vobs/Misc.hh"
#include "zenkit/vobs/MovableObject.hh"
#include "zenkit/vobs/Sound.hh"
#include "zenkit/vobs/Trigger.hh"
#include "zenkit/vobs/VirtualObject.hh"
#include "zenkit/vobs/Zone.hh"

#include "Internal.hh"

#include "archive/ArchiveAscii.hh"
#include "archive/ArchiveBinary.hh"
#include "archive/ArchiveBinsafe.hh"

#include "phoenix/buffer.hh"
#include "zenkit/World.hh"

#include <iostream>

namespace zenkit {
	static std::unordered_map<std::string, ObjectType> const OBJECTS = {
	    {"zCVob", ObjectType::zCVob},
	    {"zCVobLevelCompo:zCVob", ObjectType::zCVobLevelCompo},
	    {"oCItem:zCVob", ObjectType::oCItem},
	    {"oCNpc:zCVob", ObjectType::oCNpc},
	    {"oCMOB:zCVob", ObjectType::oCMOB},
	    {"oCMobInter:oCMOB:zCVob", ObjectType::oCMobInter},
	    {"oCMobBed:oCMobInter:oCMOB:zCVob", ObjectType::oCMobBed},
	    {"oCMobFire:oCMobInter:oCMOB:zCVob", ObjectType::oCMobFire},
	    {"oCMobLadder:oCMobInter:oCMOB:zCVob", ObjectType::oCMobLadder},
	    {"oCMobSwitch:oCMobInter:oCMOB:zCVob", ObjectType::oCMobSwitch},
	    {"oCMobWheel:oCMobInter:oCMOB:zCVob", ObjectType::oCMobWheel},
	    {"oCMobContainer:oCMobInter:oCMOB:zCVob", ObjectType::oCMobContainer},
	    {"oCMobDoor:oCMobInter:oCMOB:zCVob", ObjectType::oCMobDoor},
	    {"zCPFXControler:zCVob", ObjectType::zCPFXController},
	    {"zCVobAnimate:zCVob", ObjectType::zCVobAnimate},
	    {"zCVobLensFlare:zCVob", ObjectType::zCVobLensFlare},
	    {"zCVobLight:zCVob", ObjectType::zCVobLight},
	    {"zCVobSpot:zCVob", ObjectType::zCVobSpot},
	    {"zCVobStartpoint:zCVob", ObjectType::zCVobStartpoint},
	    {"zCVobSound:zCVob", ObjectType::zCVobSound},
	    {"zCVobSoundDaytime:zCVobSound:zCVob", ObjectType::zCVobSoundDaytime},
	    {"oCZoneMusic:zCVob", ObjectType::oCZoneMusic},
	    {"oCZoneMusicDefault:oCZoneMusic:zCVob", ObjectType::oCZoneMusicDefault},
	    {"zCZoneZFog:zCVob", ObjectType::zCZoneZFog},
	    {"zCZoneZFogDefault:zCZoneZFog:zCVob", ObjectType::zCZoneZFogDefault},
	    {"zCZoneVobFarPlane:zCVob", ObjectType::zCZoneVobFarPlane},
	    {"zCZoneVobFarPlaneDefault:zCZoneVobFarPlane:zCVob", ObjectType::zCZoneVobFarPlaneDefault},
	    {"zCMessageFilter:zCVob", ObjectType::zCMessageFilter},
	    {"zCCodeMaster:zCVob", ObjectType::zCCodeMaster},
	    {"zCTrigger:zCVob", ObjectType::zCTrigger},
	    {"zCTriggerList:zCTrigger:zCVob", ObjectType::zCTriggerList},
	    {"oCTriggerScript:zCTrigger:zCVob", ObjectType::oCTriggerScript},
	    {"zCMover:zCTrigger:zCVob", ObjectType::zCMover},
	    {"oCTriggerChangeLevel:zCTrigger:zCVob", ObjectType::oCTriggerChangeLevel},
	    {"zCTriggerWorldStart:zCVob", ObjectType::zCTriggerWorldStart},
	    {"zCTriggerUntouch:zCVob", ObjectType::zCTriggerUntouch},
	    {"zCCSCamera:zCVob", ObjectType::zCCSCamera},
	    {"zCCamTrj_KeyFrame:zCVob", ObjectType::zCCamTrj_KeyFrame},
	    {"oCTouchDamage:zCTouchDamage:zCVob", ObjectType::oCTouchDamage},
	    {"zCEarthquake:zCVob", ObjectType::zCEarthquake},
	    {"zCMoverControler:zCVob", ObjectType::zCMoverController},
	    {"zCVobScreenFX:zCVob", ObjectType::zCVobScreenFX},
	    {"zCVobStair:zCVob", ObjectType::zCVobStair},
	    {"oCCSTrigger:zCTrigger:zCVob", ObjectType::oCCSTrigger},
	    {"oCNpcTalent", ObjectType::oCNpcTalent},
	    {"zCEventManager", ObjectType::zCEventManager},
	    {"zCDecal", ObjectType::zCDecal},
	    {"zCMesh", ObjectType::zCMesh},
	    {"zCProgMeshProto", ObjectType::zCProgMeshProto},
	    {"zCParticleFX", ObjectType::zCParticleFX},
	    {"zCAICamera", ObjectType::zCAICamera},
	    {"zCModel", ObjectType::zCModel},
	    {"zCMorphMesh", ObjectType::zCMorphMesh},
	    {"oCAIHuman:oCAniCtrl_Human:zCAIPlayer", ObjectType::oCAIHuman},
	    {"oCAIVobMove", ObjectType::oCAIVobMove},
	    {"oCCSPlayer:zCCSPlayer", ObjectType::oCCSPlayer},
	    {"zCSkyControler_Outdoor", ObjectType::zCSkyControler_Outdoor},
	};

	ReadArchive::ReadArchive(ArchiveHeader head, Read* r) : header(std::move(head)), read(r) {}

	ReadArchive::ReadArchive(ArchiveHeader head, Read* r, std::unique_ptr<Read> owned)
	    : header(std::move(head)), read(r), _m_owned(std::move(owned)) {}

	void ArchiveHeader::load(Read* r) {
		try {
			if (r->read_line(true) != "ZenGin Archive") {
				ZKLOGE("ReadArchive", "Invalid Header");
				throw zenkit::ParserError {"ReadArchive", "magic missing"};
			}

			std::string ver = r->read_line(true);
			if (ver.find("ver ") != 0) {
				throw zenkit::ParserError {"ReadArchive", "ver field missing"};
			}

			this->version = std::stoi(ver.substr(ver.find(' ') + 1));
			if (this->version != 1) {
				throw zenkit::ParserError {"ReadArchive", "Unsupported format version: " + ver};
			}

			this->archiver = r->read_line(true);

			auto fmt = r->read_line(true);
			if (fmt == "ASCII") {
				this->format = ArchiveFormat::ASCII;
			} else if (fmt == "BINARY") {
				this->format = ArchiveFormat::BINARY;
			} else if (fmt == "BIN_SAFE") {
				this->format = ArchiveFormat::BINSAFE;
			}

			std::string save_game = r->read_line(true);
			if (save_game.find("saveGame ") != 0) {
				throw zenkit::ParserError {"ReadArchive", "saveGame field missing"};
			}
			this->save = std::stoi(save_game.substr(save_game.find(' ') + 1)) != 0;

			std::string optional = r->read_line(true);
			if (optional.find("date ") == 0) {
				this->date = optional.substr(optional.find(' ') + 1);
				optional = r->read_line(true);
			}

			if (optional.find("user ") == 0) {
				this->user = optional.substr(optional.find(' ') + 1);
				optional = r->read_line(true);
			}

			if (optional != "END") {
				throw zenkit::ParserError {"ReadArchive", "first END missing"};
			}
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive", e, "reading int"};
		}
	}

	std::unique_ptr<ReadArchive> ReadArchive::open(phoenix::buffer& in) {
		auto read = zenkit::Read::from(&in);

		ArchiveHeader header {};
		header.load(read.get());

		std::unique_ptr<ReadArchive> reader;
		if (header.format == ArchiveFormat::ASCII) {
			reader = std::make_unique<ReadArchiveAscii>(std::move(header), read.get(), std::move(read));
		} else if (header.format == ArchiveFormat::BINARY) {
			reader = std::make_unique<ReadArchiveBinary>(std::move(header), read.get(), std::move(read));
		} else if (header.format == ArchiveFormat::BINSAFE) {
			reader = std::make_unique<ReadArchiveBinsafe>(std::move(header), read.get(), std::move(read));
		} else {
			throw zenkit::ParserError {"ReadArchive",
			                           "format '" + std::to_string(static_cast<uint32_t>(header.format)) +
			                               "' is not supported"};
		}

		reader->read_header();
		return reader;
	}

	std::unique_ptr<ReadArchive> ReadArchive::from(Read* r) {
		ArchiveHeader header {};
		header.load(r);

		std::unique_ptr<ReadArchive> reader;
		if (header.format == ArchiveFormat::ASCII) {
			reader = std::make_unique<ReadArchiveAscii>(std::move(header), r);
		} else if (header.format == ArchiveFormat::BINARY) {
			reader = std::make_unique<ReadArchiveBinary>(std::move(header), r);
		} else if (header.format == ArchiveFormat::BINSAFE) {
			reader = std::make_unique<ReadArchiveBinsafe>(std::move(header), r);
		} else {
			throw ParserError {"ReadArchive",
			                   "format '" + std::to_string(static_cast<uint32_t>(header.format)) +
			                       "' is not supported"};
		}

		reader->read_header();
		return reader;
	}

	std::shared_ptr<Object> ReadArchive::read_object(GameVersion version) {
		ArchiveObject obj;
		if (!this->read_object_begin(obj)) {
			ZKLOGE("ReadArchive", "Expected object, got entry.");
			return nullptr;
		}

		if (obj.class_name == "\xA7") {
			if (!this->read_object_end()) {
				ZKLOGE("ReadArchive", "Invalid reference object: has children");
				this->skip_object(true);
			}

			auto cached = _m_cache.find(obj.index);
			if (cached == _m_cache.end()) {
				ZKLOGW("ReadArchive", "Unresolved reference: %d", obj.index);
				return nullptr;
			}

			return cached->second;
		}

		if (obj.class_name == "%") {
			// This object is marked as "Empty"
			this->skip_object(true);
			return nullptr;
		}

		auto it = OBJECTS.find(obj.class_name);
		ObjectType type = ObjectType::unknown;
		if (it != OBJECTS.end()) {
			type = it->second;
		}

		// TODO(lmichaelis): The VOb type/id assignment is a hacky workaround! Create separate types!
		std::shared_ptr<Object> syn;
		switch (type) {
		case ObjectType::oCNpcTalent:
			syn = std::make_shared<vobs::Npc::Talent>();
			break;
		case ObjectType::zCEventManager:
			syn = std::make_shared<EventManager>();
			break;
		case ObjectType::zCDecal:
			syn = std::make_shared<VisualDecal>();
			break;
		case ObjectType::zCMesh:
			syn = std::make_shared<VisualMesh>();
			break;
		case ObjectType::zCProgMeshProto:
			syn = std::make_shared<VisualMultiResolutionMesh>();
			break;
		case ObjectType::zCParticleFX:
			syn = std::make_shared<VisualParticleEffect>();
			break;
		case ObjectType::zCAICamera:
			syn = std::make_shared<VisualCamera>();
			break;
		case ObjectType::zCModel:
			syn = std::make_shared<VisualModel>();
			break;
		case ObjectType::zCMorphMesh:
			syn = std::make_shared<VisualMorphMesh>();
			break;
		case ObjectType::oCAIHuman:
			syn = std::make_shared<AiHuman>();
			break;
		case ObjectType::oCAIVobMove:
			syn = std::make_shared<AiMove>();
			break;
		case ObjectType::zCSkyControler_Outdoor:
			syn = std::make_shared<SkyController>();
			break;
		case ObjectType::oCCSPlayer:
			syn = std::make_shared<CutscenePlayer>();
			break;
		case ObjectType::zCVobLevelCompo:
		case ObjectType::zCVobStartpoint:
		case ObjectType::zCVobStair:
		case ObjectType::zCVobSpot:
		case ObjectType::zCVob:
			syn = std::make_shared<VirtualObject>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCVobScreenFX:
			syn = std::make_shared<vobs::ScreenEffect>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCCSCamera:
			syn = std::make_shared<vobs::CutsceneCamera>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCCamTrj_KeyFrame:
			syn = std::make_shared<vobs::CameraTrajectoryFrame>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCVobAnimate:
			syn = std::make_shared<vobs::Animate>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCZoneVobFarPlane:
		case ObjectType::zCZoneVobFarPlaneDefault:
			syn = std::make_shared<vobs::ZoneFarPlane>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCZoneZFogDefault:
		case ObjectType::zCZoneZFog:
			syn = std::make_shared<vobs::ZoneFog>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCVobLensFlare:
			syn = std::make_shared<vobs::LensFlare>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCItem:
			syn = std::make_shared<vobs::Item>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCTrigger:
		case ObjectType::oCCSTrigger:
			syn = std::make_shared<vobs::Trigger>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCMOB:
			syn = std::make_shared<vobs::MovableObject>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCMobInter:
		case ObjectType::oCMobLadder:
		case ObjectType::oCMobSwitch:
		case ObjectType::oCMobWheel:
		case ObjectType::oCMobBed:
			syn = std::make_shared<vobs::InteractiveObject>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCMobFire:
			syn = std::make_shared<vobs::Fire>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCMobContainer:
			syn = std::make_shared<vobs::Container>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCMobDoor:
			syn = std::make_shared<vobs::Door>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCPFXController:
			syn = std::make_shared<vobs::ParticleEffectController>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCVobLight:
			syn = std::make_shared<vobs::Light>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCVobSound:
			syn = std::make_shared<vobs::Sound>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCVobSoundDaytime:
			syn = std::make_shared<vobs::SoundDaytime>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCZoneMusic:
		case ObjectType::oCZoneMusicDefault:
			syn = std::make_shared<vobs::ZoneMusic>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCMessageFilter:
			syn = std::make_shared<vobs::MessageFilter>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCCodeMaster:
			syn = std::make_shared<vobs::CodeMaster>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCTriggerList:
			syn = std::make_shared<vobs::TriggerList>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCTriggerScript:
			syn = std::make_shared<vobs::TriggerScript>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCMover:
			syn = std::make_shared<vobs::Mover>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCTriggerChangeLevel:
			syn = std::make_shared<vobs::TriggerChangeLevel>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCTriggerWorldStart:
			syn = std::make_shared<vobs::TriggerWorldStart>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCTouchDamage:
			syn = std::make_shared<vobs::TouchDamage>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCTriggerUntouch:
			syn = std::make_shared<vobs::TriggerUntouch>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCEarthquake:
			syn = std::make_shared<vobs::Earthquake>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::zCMoverController:
			syn = std::make_shared<vobs::MoverController>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		case ObjectType::oCNpc:
			syn = std::make_shared<vobs::Npc>();
			reinterpret_cast<VirtualObject*>(syn.get())->type = type;
			reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			break;
		default:
			ZKLOGE("ReadArchive", "Unknown object type: %s", obj.class_name.c_str());
			break;
		}

		_m_cache.insert_or_assign(obj.index, syn);

		if (syn != nullptr) {
			syn->load(*this, version);
		}

		if (!this->read_object_end()) {
			ZKLOGW("ReadArchive", "Object not fully loaded: %s", obj.class_name.data());
			this->skip_object(true);
		}

		return syn;
	}

	void ReadArchive::skip_object(bool skip_current) {
		ArchiveObject tmp;
		int32_t level = skip_current ? 1 : 0;

		do {
			if (read_object_begin(tmp)) {
				++level;
			} else if (read_object_end()) {
				--level;
			} else {
				skip_entry();
			}
		} while (level > 0);
	}
} // namespace zenkit
