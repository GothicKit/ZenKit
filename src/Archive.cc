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
	    {"zCWayNet", ObjectType::zCWayNet},
	    {"zCWaypoint", ObjectType::zCWaypoint},
	    {"zCMaterial", ObjectType::zCMaterial},
	};

	static std::unordered_map<ObjectType, std::string> const CLASS_NAMES = {
	    {ObjectType::zCVob, "zCVob"},
	    {ObjectType::zCVobLevelCompo, "zCVobLevelCompo:zCVob"},
	    {ObjectType::oCItem, "oCItem:zCVob"},
	    {ObjectType::oCNpc, "oCNpc:zCVob"},
	    {ObjectType::oCMOB, "oCMOB:zCVob"},
	    {ObjectType::oCMobInter, "oCMobInter:oCMOB:zCVob"},
	    {ObjectType::oCMobBed, "oCMobBed:oCMobInter:oCMOB:zCVob"},
	    {ObjectType::oCMobFire, "oCMobFire:oCMobInter:oCMOB:zCVob"},
	    {ObjectType::oCMobLadder, "oCMobLadder:oCMobInter:oCMOB:zCVob"},
	    {ObjectType::oCMobSwitch, "oCMobSwitch:oCMobInter:oCMOB:zCVob"},
	    {ObjectType::oCMobWheel, "oCMobWheel:oCMobInter:oCMOB:zCVob"},
	    {ObjectType::oCMobContainer, "oCMobContainer:oCMobInter:oCMOB:zCVob"},
	    {ObjectType::oCMobDoor, "oCMobDoor:oCMobInter:oCMOB:zCVob"},
	    {ObjectType::zCPFXController, "zCPFXControler:zCVob"},
	    {ObjectType::zCVobAnimate, "zCVobAnimate:zCVob"},
	    {ObjectType::zCVobLensFlare, "zCVobLensFlare:zCVob"},
	    {ObjectType::zCVobLight, "zCVobLight:zCVob"},
	    {ObjectType::zCVobSpot, "zCVobSpot:zCVob"},
	    {ObjectType::zCVobStartpoint, "zCVobStartpoint:zCVob"},
	    {ObjectType::zCVobSound, "zCVobSound:zCVob"},
	    {ObjectType::zCVobSoundDaytime, "zCVobSoundDaytime:zCVobSound:zCVob"},
	    {ObjectType::oCZoneMusic, "oCZoneMusic:zCVob"},
	    {ObjectType::oCZoneMusicDefault, "oCZoneMusicDefault:oCZoneMusic:zCVob"},
	    {ObjectType::zCZoneZFog, "zCZoneZFog:zCVob"},
	    {ObjectType::zCZoneZFogDefault, "zCZoneZFogDefault:zCZoneZFog:zCVob"},
	    {ObjectType::zCZoneVobFarPlane, "zCZoneVobFarPlane:zCVob"},
	    {ObjectType::zCZoneVobFarPlaneDefault, "zCZoneVobFarPlaneDefault:zCZoneVobFarPlane:zCVob"},
	    {ObjectType::zCMessageFilter, "zCMessageFilter:zCVob"},
	    {ObjectType::zCCodeMaster, "zCCodeMaster:zCVob"},
	    {ObjectType::zCTrigger, "zCTrigger:zCVob"},
	    {ObjectType::zCTriggerList, "zCTriggerList:zCTrigger:zCVob"},
	    {ObjectType::oCTriggerScript, "oCTriggerScript:zCTrigger:zCVob"},
	    {ObjectType::zCMover, "zCMover:zCTrigger:zCVob"},
	    {ObjectType::oCTriggerChangeLevel, "oCTriggerChangeLevel:zCTrigger:zCVob"},
	    {ObjectType::zCTriggerWorldStart, "zCTriggerWorldStart:zCVob"},
	    {ObjectType::zCTriggerUntouch, "zCTriggerUntouch:zCVob"},
	    {ObjectType::zCCSCamera, "zCCSCamera:zCVob"},
	    {ObjectType::zCCamTrj_KeyFrame, "zCCamTrj_KeyFrame:zCVob"},
	    {ObjectType::oCTouchDamage, "oCTouchDamage:zCTouchDamage:zCVob"},
	    {ObjectType::zCEarthquake, "zCEarthquake:zCVob"},
	    {ObjectType::zCMoverController, "zCMoverControler:zCVob"},
	    {ObjectType::zCVobScreenFX, "zCVobScreenFX:zCVob"},
	    {ObjectType::zCVobStair, "zCVobStair:zCVob"},
	    {ObjectType::oCCSTrigger, "oCCSTrigger:zCTrigger:zCVob"},
	    {ObjectType::oCNpcTalent, "oCNpcTalent"},
	    {ObjectType::zCEventManager, "zCEventManager"},
	    {ObjectType::zCDecal, "zCDecal"},
	    {ObjectType::zCMesh, "zCMesh"},
	    {ObjectType::zCProgMeshProto, "zCProgMeshProto"},
	    {ObjectType::zCParticleFX, "zCParticleFX"},
	    {ObjectType::zCAICamera, "zCAICamera"},
	    {ObjectType::zCModel, "zCModel"},
	    {ObjectType::zCMorphMesh, "zCMorphMesh"},
	    {ObjectType::oCAIHuman, "oCAIHuman:oCAniCtrl_Human:zCAIPlayer"},
	    {ObjectType::oCAIVobMove, "oCAIVobMove"},
	    {ObjectType::oCCSPlayer, "oCCSPlayer:zCCSPlayer"},
	    {ObjectType::zCSkyControler_Outdoor, "zCSkyControler_Outdoor"},
	    {ObjectType::zCWayNet, "zCWayNet"},
	    {ObjectType::zCWaypoint, "zCWaypoint"},
	    {ObjectType::zCMaterial, "zCMaterial"},
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
		auto read = Read::from(&in);

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

		std::shared_ptr<Object> syn;
		switch (type) {
		case ObjectType::oCNpcTalent:
			syn = std::make_shared<VNpc::Talent>();
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
			syn = std::make_shared<VLevel>();
			break;
		case ObjectType::zCVobStartpoint:
			syn = std::make_shared<VStartPoint>();
			break;
		case ObjectType::zCVobStair:
			syn = std::make_shared<VStair>();
			break;
		case ObjectType::zCVobSpot:
			syn = std::make_shared<VSpot>();
			break;
		case ObjectType::zCVob:
			syn = std::make_shared<VirtualObject>();
			break;
		case ObjectType::zCVobScreenFX:
			syn = std::make_shared<VScreenEffect>();
			break;
		case ObjectType::zCCSCamera:
			syn = std::make_shared<VCutsceneCamera>();
			break;
		case ObjectType::zCCamTrj_KeyFrame:
			syn = std::make_shared<VCameraTrajectoryFrame>();
			break;
		case ObjectType::zCVobAnimate:
			syn = std::make_shared<VAnimate>();
			break;
		case ObjectType::zCZoneVobFarPlane:
			syn = std::make_shared<VZoneFarPlane>();
			break;
		case ObjectType::zCZoneVobFarPlaneDefault:
			syn = std::make_shared<VZoneFarPlaneDefault>();
			break;
		case ObjectType::zCZoneZFogDefault:
			syn = std::make_shared<VZoneFogDefault>();
			break;
		case ObjectType::zCZoneZFog:
			syn = std::make_shared<VZoneFog>();
			break;
		case ObjectType::zCVobLensFlare:
			syn = std::make_shared<VLensFlare>();
			break;
		case ObjectType::oCItem:
			syn = std::make_shared<VItem>();
			break;
		case ObjectType::zCTrigger:
			syn = std::make_shared<VTrigger>();
			break;
		case ObjectType::oCCSTrigger:
			syn = std::make_shared<VCutsceneTrigger>();
			break;
		case ObjectType::oCMOB:
			syn = std::make_shared<VMovableObject>();
			break;
		case ObjectType::oCMobInter:
			syn = std::make_shared<VInteractiveObject>();
			break;
		case ObjectType::oCMobLadder:
			syn = std::make_shared<VLadder>();
			break;
		case ObjectType::oCMobSwitch:
			syn = std::make_shared<VSwitch>();
			break;
		case ObjectType::oCMobWheel:
			syn = std::make_shared<VWheel>();
			break;
		case ObjectType::oCMobBed:
			syn = std::make_shared<VBed>();
			break;
		case ObjectType::oCMobFire:
			syn = std::make_shared<VFire>();
			break;
		case ObjectType::oCMobContainer:
			syn = std::make_shared<VContainer>();
			break;
		case ObjectType::oCMobDoor:
			syn = std::make_shared<VDoor>();
			break;
		case ObjectType::zCPFXController:
			syn = std::make_shared<VParticleEffectController>();
			break;
		case ObjectType::zCVobLight:
			syn = std::make_shared<VLight>();
			break;
		case ObjectType::zCVobSound:
			syn = std::make_shared<VSound>();
			break;
		case ObjectType::zCVobSoundDaytime:
			syn = std::make_shared<VSoundDaytime>();
			break;
		case ObjectType::oCZoneMusic:
			syn = std::make_shared<VZoneMusic>();
			break;
		case ObjectType::oCZoneMusicDefault:
			syn = std::make_shared<VZoneMusicDefault>();
			break;
		case ObjectType::zCMessageFilter:
			syn = std::make_shared<VMessageFilter>();
			break;
		case ObjectType::zCCodeMaster:
			syn = std::make_shared<VCodeMaster>();
			break;
		case ObjectType::zCTriggerList:
			syn = std::make_shared<VTriggerList>();
			break;
		case ObjectType::oCTriggerScript:
			syn = std::make_shared<VTriggerScript>();
			break;
		case ObjectType::zCMover:
			syn = std::make_shared<VMover>();
			break;
		case ObjectType::oCTriggerChangeLevel:
			syn = std::make_shared<VTriggerChangeLevel>();
			break;
		case ObjectType::zCTriggerWorldStart:
			syn = std::make_shared<VTriggerWorldStart>();
			break;
		case ObjectType::oCTouchDamage:
			syn = std::make_shared<VTouchDamage>();
			break;
		case ObjectType::zCTriggerUntouch:
			syn = std::make_shared<VTriggerUntouch>();
			break;
		case ObjectType::zCEarthquake:
			syn = std::make_shared<VEarthquake>();
			break;
		case ObjectType::zCMoverController:
			syn = std::make_shared<VMoverController>();
			break;
		case ObjectType::oCNpc:
			syn = std::make_shared<VNpc>();
			break;
		case ObjectType::zCMaterial:
			syn = std::make_shared<Material>();
			break;
#ifdef ZK_FUTURE
		case ObjectType::zCWayNet:
			syn = std::make_shared<WayNet>();
			break;
		case ObjectType::zCWaypoint:
			syn = std::make_shared<WayPoint>();
			break;
#endif
		default:
			ZKLOGE("ReadArchive", "Unknown object type: %s", obj.class_name.c_str());
			break;
		}

		if (syn != nullptr) {
			// TODO(lmichaelis): The VOb type/id assignment is a hacky workaround! Create separate types!
			if (is_vobject(type)) {
				VirtualObjectType vtype = static_cast<VirtualObjectType>(type);
				reinterpret_cast<VirtualObject*>(syn.get())->type = vtype;
				reinterpret_cast<VirtualObject*>(syn.get())->id = obj.index;
			}

			_m_cache.insert_or_assign(obj.index, syn);
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

	std::unique_ptr<WriteArchive> WriteArchive::to(Write* w, ArchiveFormat format) {
		switch (format) {
		case ArchiveFormat::BINARY:
			return std::make_unique<WriteArchiveBinary>(w);
		case ArchiveFormat::BINSAFE:
			return std::make_unique<WriteArchiveBinsafe>(w);
		case ArchiveFormat::ASCII:
			return std::make_unique<WriteArchiveAscii>(w);
		}

		return nullptr;
	}

	std::unique_ptr<WriteArchive> WriteArchive::to_save(Write* w, ArchiveFormat format) {
		auto ar = to(w, format);
		ar->_m_save = true;
		return ar;
	}

	void WriteArchive::write_object(std::shared_ptr<Object> obj, GameVersion version) {
		this->write_object("%", obj, version);
	}

	void WriteArchive::write_object(std::string_view name, std::shared_ptr<Object> obj, GameVersion version) {
		auto it = _m_cache.find(obj.get());
		if (it != _m_cache.end()) {
			this->write_ref(name, it->second);
			return;
		}

		this->write_object(name, obj.get(), version);
	}

	void WriteArchive::write_object(std::string_view name, Object const* obj, GameVersion version) {
		std::string_view class_name = CLASS_NAMES.at(obj->get_object_type());
		uint16_t obj_version = obj->get_version_identifier(version);

		auto index = this->write_object_begin(name, class_name, obj_version);
		_m_cache.insert_or_assign(obj, index);

		obj->save(*this, version);
		this->write_object_end();
	}
} // namespace zenkit
