// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"
#include "zenkit/Material.hh"
#include "zenkit/Misc.hh"

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace zenkit {
	class ReadArchive;

	/// \brief All possible VOb types.
	/// \summary Mostly copied from [ZenLib](https://github.com/Try/ZenLib).
	enum class VobType : std::uint8_t {
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
	};

	/// \brief Ways a VOb can cast shadows.
	enum class ShadowType : std::uint8_t {
		NONE = 0, ///< The VOb does not cast any shadow.
		BLOB = 1, ///< The VOb casts a basic dark circle at its base.

		// Deprecated entries.
		none ZKREM("renamed to ShadowType::NONE") = NONE,
		blob ZKREM("renamed to ShadowType::BLOB") = BLOB,
	};

	/// \brief Ways a VOb is seen in the game world.
	enum class VisualType : std::uint8_t {
		DECAL = 0,                 ///< The VOb presents as a decal.
		MESH = 1,                  ///< The VOb presents a phoenix::Mesh.
		MULTI_RESOLUTION_MESH = 2, ///< The VOb presents a phoenix::MultiResolutionMesh.
		PARTICLE_EFFECT = 3,       ///< The VOb presents as a particle system.
		AI_CAMERA = 4,             ///< The VOb is a game-controlled camera.
		MODEL = 5,                 ///< The VOb presents a phoenix::Model.
		MORPH_MESH = 6,            ///< The VOb presents a phoenix::MorphMesh.
		UNKNOWN = 7,               ///< The VOb presents an unknown visual or no visual at all.

		// Deprecated entries.
		decal ZKREM("renamed to VisualType::DECAL") = DECAL,
		mesh ZKREM("renamed to VisualType::MESH") = MESH,
		proto_mesh ZKREM("renamed to VisualType::MULTI_RESOLUTION_MESH") = MULTI_RESOLUTION_MESH,
		particle_system ZKREM("renamed to VisualType::PARTICLE_EFFECT") = PARTICLE_EFFECT,
		ai_camera ZKREM("renamed to VisualType::AI_CAMERA") = AI_CAMERA,
		model ZKREM("renamed to VisualType::MODEL") = MODEL,
		morph_mesh ZKREM("renamed to VisualType::MORPH_MESH") = MORPH_MESH,
		unknown ZKREM("renamed to VisualType::UNKNOWN") = UNKNOWN,
	};

	/// \brief Ways the camera may behave with a VOb.
	enum class SpriteAlignment : std::uint8_t {
		NONE = 0, ///< The sprite is not affected by the camera's position.
		YAW = 1,  ///< The sprite rotates with the camera's yaw axis.
		FULL = 2, ///< The sprite rotates with camera fully.

		// Deprecated entries.
		none ZKREM("renamed to SpriteAlignment::NONE") = NONE,
		yaw ZKREM("renamed to SpriteAlignment::YAW") = YAW,
		full ZKREM("renamed to SpriteAlignment::FULL") = FULL,
	};

	/// \brief Types of wavy animation.
	enum class AnimationType : std::uint8_t {
		NONE = 0, ///< No wave animation.
		WIND = 1,
		WIND_ALT = 2,

		// Deprecated entries.
		none ZKREM("renamed to AnimationType::NONE") = NONE,
		wind ZKREM("renamed to AnimationType::WIND") = WIND,
		wind2 ZKREM("renamed to AnimationType::WIND_ALT") = WIND_ALT,
	};

	/// \brief Decal visual configuration for VObs.
	struct Decal {
		std::string name {};
		glm::vec2 dimension {};
		glm::vec2 offset {};
		bool two_sided {};
		AlphaFunction alpha_func {};
		float texture_anim_fps {};
		std::uint8_t alpha_weight {};
		bool ignore_daylight {};

		/// \brief Parses a decal the given *ZenGin* archive.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \return The parsed decal.
		/// \throws ParserError if parsing fails.
		ZKREM("use ::load()") ZKAPI static Decal parse(ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version);
	};

	struct RigidBody {
		glm::vec3 vel;
		std::uint8_t mode;
		bool gravity_enabled;
		float gravity_scale;
		glm::vec3 slide_direction;

		ZKAPI void load(ReadArchive& r, GameVersion version);
	};

	struct EventManager {
		bool cleared;
		bool active;

		ZKAPI void load(ReadArchive& r, GameVersion version);
	};

	/// \brief The base class for all VObs.
	///
	/// <p>Contains parameters all VObs have, like their position, bounding box and model.</p>
	struct VirtualObject {
		struct SaveState {
			uint8_t sleep_mode;
			float next_on_timer;
			std::optional<RigidBody> rigid_body {};
		};

		using save_state ZKREM("renamed to SaveState") = SaveState;

		VobType type; ///< The type of this VOb.
		uint32_t id;  ///< The index of this VOb in the archive it was read from.

		AxisAlignedBoundingBox bbox {};
		glm::vec3 position {};
		glm::mat3x3 rotation {};
		bool show_visual {};
		SpriteAlignment sprite_camera_facing_mode {SpriteAlignment::NONE};
		bool cd_static {};
		bool cd_dynamic {};
		bool vob_static {};
		ShadowType dynamic_shadows {};
		bool physics_enabled {};
		AnimationType anim_mode {};
		std::int32_t bias {};
		bool ambient {};
		float anim_strength {};
		float far_clip_scale {};

		std::string preset_name {};
		std::string vob_name {};
		std::string visual_name {};

		VisualType associated_visual_type {};
		std::optional<Decal> visual_decal {};

		/// \brief Contains extra data about the item in the context of a saved game.
		std::optional<SaveState> saved {};

		/// \brief The children of this VOb.
		std::vector<std::unique_ptr<VirtualObject>> children {};

		/// \brief Default virtual destructor.
		virtual ~VirtualObject() = default;

		/// \return `true` if this VOb is from a save-game and `false` if not.
		[[nodiscard]] ZKAPI inline bool is_save_game() const noexcept {
			return saved.has_value();
		}

		/// \brief Parses a base VOb from the given *ZenGin* archive.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		ZKREM("use ::load()") ZKAPI static void parse(VirtualObject& obj, ReadArchive& ctx, GameVersion version);

		ZKAPI virtual void load(ReadArchive& r, GameVersion version);
	};
} // namespace zenkit
