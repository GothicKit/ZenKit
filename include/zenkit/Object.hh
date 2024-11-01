// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "Misc.hh"

#include <cstdint>

namespace zenkit {
	class ReadArchive;
	class WriteArchive;

	enum class ObjectType {
		zCVob = 0,           ///< The base type for all VObs.
		zCVobLevelCompo = 1, ///< A basic VOb used for grouping other VObs.
		oCItem = 2,          ///< A VOb representing an item
		oCNpc = 3,           ///< A VOb representing an NPC
		zCMoverController = 4,
		zCVobScreenFX = 5,
		zCVobStair = 6,
		zCPFXController = 7,
		zCVobAnimate = 8,
		zCVobLensFlare = 9,
		zCVobLight = 10,
		zCVobSpot = 11,
		zCVobStartpoint = 12,
		zCMessageFilter = 13,
		zCCodeMaster = 14,
		zCTriggerWorldStart = 15,
		zCCSCamera = 16,
		zCCamTrj_KeyFrame = 17,
		oCTouchDamage = 18,
		zCTriggerUntouch = 19,
		zCEarthquake = 20,
		oCMOB = 21,                ///< The base VOb type used for dynamic world objects.
		oCMobInter = 22,           ///< The base VOb type used for interactive world objects.
		oCMobBed = 23,             ///< A bed the player can sleep in.
		oCMobFire = 24,            ///< A campfire the player can cook things on.
		oCMobLadder = 25,          ///< A ladder the player can climb.
		oCMobSwitch = 26,          ///< A switch or button the player can use.
		oCMobWheel = 27,           ///< A grindstone the player can sharpen their weapon with.
		oCMobContainer = 28,       ///< A container the player can open.
		oCMobDoor = 29,            ///< A door the player can open.
		zCTrigger = 30,            ///< The base VOb type used for all kinds of triggers.
		zCTriggerList = 31,        ///< A collection of multiple triggers.
		oCTriggerScript = 32,      ///< A trigger for calling a script function.
		oCTriggerChangeLevel = 33, ///< A trigger for changing the game world.
		oCCSTrigger = 34,          ///< A cutscene trigger.
		zCMover = 35,
		zCVobSound = 36,        ///< A VOb which emits a certain sound.
		zCVobSoundDaytime = 37, ///< A VOb which emits a sound only during a specified time.
		oCZoneMusic = 38,       ///< A VOb which plays music from the soundtrack.
		oCZoneMusicDefault = 39,
		zCZoneZFog = 40, ///< A VOb which indicates a foggy area.
		zCZoneZFogDefault = 41,
		zCZoneVobFarPlane = 42,
		zCZoneVobFarPlaneDefault = 43,
		ignored = 44,
		unknown = 45,

		oCNpcTalent,
		zCEventManager,
		zCDecal,
		zCMesh,
		zCProgMeshProto,
		zCParticleFX,
		zCAICamera,
		zCModel,
		zCMorphMesh,
		oCAIHuman,
		oCAIVobMove,
		oCCSPlayer,
		zCSkyControler_Outdoor,
		oCWorld,
		zCWayNet,
		zCWaypoint,
		zCMaterial,
		oCSavegameInfo,
		oCCSManager,
		zCCSPoolItem,
		zCCSBlock,
		zCCutscene,
		zCCutsceneContext,
		oCMsgConversation,
		zCCSAtomicBlock,
		zCCSLib,
	};

	constexpr bool is_vobject(ObjectType type) {
		return type < ObjectType::ignored;
	}

#define ZK_OBJECT(s)                                                                                                   \
public:                                                                                                                \
	static constexpr ObjectType TYPE = s;                                                                              \
	ObjectType get_object_type() const override {                                                                      \
		return s;                                                                                                      \
	}

	class Object ZKAPI {
	public:
		Object() = default;
		virtual ~Object() noexcept = default;

		[[nodiscard]] virtual ObjectType get_object_type() const;
		[[nodiscard]] virtual uint16_t get_version_identifier(GameVersion game) const;

		virtual void load(ReadArchive& r, GameVersion version);
		virtual void save(WriteArchive& w, GameVersion version) const;
	};
} // namespace zenkit
