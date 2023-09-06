// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "../Api.hh"
#include <phoenix/archive.hh>
#include <phoenix/material.hh>
#include <phoenix/phoenix.hh>

namespace phoenix {
	/// \brief All possible VOb types.
	/// \summary Mostly copied from [ZenLib](https://github.com/Try/ZenLib).
	enum class vob_type : std::uint8_t {
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
	enum class shadow_mode : std::uint8_t {
		none = 0, ///< The VOb does not cast any shadow.
		blob = 1, ///< The VOb casts a basic dark circle at its base.
	};

	/// \brief Ways a VOb is seen in the game world.
	enum class visual_type : std::uint8_t {
		decal = 0,           ///< The VOb presents as a decal.
		mesh = 1,            ///< The VOb presents a phoenix::mesh.
		proto_mesh = 2,      ///< The VOb presents a phoenix::proto_mesh.
		particle_system = 3, ///< The VOb presents as a particle system.
		ai_camera = 4,       ///< The VOb is a game-controlled camera.
		model = 5,           ///< The VOb presents a phoenix::model.
		morph_mesh = 6,      ///< The VOb presents a phoenix::morph_mesh.
		unknown = 7,         ///< The VOb presents an unknown visual or no visual at all.
	};

	/// \brief Ways the camera may behave with a VOb.
	enum class sprite_alignment : std::uint8_t {
		none = 0, ///< The sprite is not affected by the camera's position.
		yaw = 1,  ///< The sprite rotates with the camera's yaw axis.
		full = 2, ///< The sprite rotates with camera fully.
	};

	/// \brief Ways the camera may behave with a VOb. Same as sprite_alignment
	/// \deprecated Use sprite_alignment instead.
	using camera_lock_mode [[deprecated("use phoenix::sprite_alignment")]] = sprite_alignment;

	/// \brief Types of wavy animation.
	enum class animation_mode : std::uint8_t {
		none = 0, ///< No wave animation.
		wind = 1,
		wind2 = 2,
	};

	/// \brief Decal visual configuration for VObs.
	struct decal {
		std::string name {};
		glm::vec2 dimension {};
		glm::vec2 offset {};
		bool two_sided {};
		alpha_function alpha_func {};
		float texture_anim_fps {};
		std::uint8_t alpha_weight {};
		bool ignore_daylight {};

		/// \brief Parses a decal the given *ZenGin* archive.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \return The parsed decal.
		/// \throws parser_error if parsing fails.
		PHOENIX_API static decal parse(archive_reader& ctx, game_version version);
	};

	/// \brief The base class for all VObs.
	///
	/// <p>Contains parameters all VObs have, like their position, bounding box and model.</p>
	struct vob {
		struct save_state {
			uint8_t sleep_mode;
			float next_on_timer;
		};

		vob_type type; ///< The type of this VOb.
		uint32_t id;   ///< The index of this VOb in the archive it was read from.

		bounding_box bbox {};
		glm::vec3 position {};
		glm::mat3x3 rotation {};
		bool show_visual {};

		union {
			[[deprecated("use phoenix::vob::sprite_camera_facing_mode")]] sprite_alignment camera_alignment;
			sprite_alignment sprite_camera_facing_mode;
		};

		bool cd_static {};
		bool cd_dynamic {};
		bool vob_static {};
		shadow_mode dynamic_shadows {};
		bool physics_enabled {};
		animation_mode anim_mode {};
		std::int32_t bias {};
		bool ambient {};
		float anim_strength {};
		float far_clip_scale {};

		std::string preset_name {};
		std::string vob_name {};
		std::string visual_name {};

		visual_type associated_visual_type {};
		std::optional<decal> visual_decal {};

		/// \brief Contains extra data about the item in the context of a saved game.
		std::optional<save_state> saved;

		/// \brief The children of this VOb.
		std::vector<std::unique_ptr<vob>> children {};

		/// \brief Default virtual destructor.
		virtual ~vob() = default;

		/// \return `true` if this VOb is from a save-game and `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_save_game() const noexcept {
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
		/// \throws parser_error if parsing fails.
		PHOENIX_API static void parse(vob& obj, archive_reader& ctx, game_version version);
	};
} // namespace phoenix
