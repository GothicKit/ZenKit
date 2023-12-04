// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"
#include "zenkit/Material.hh"
#include "zenkit/Misc.hh"
#include "zenkit/Object.hh"

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace zenkit {
	class ReadArchive;
	struct VNpc;

	/// \brief Ways VObjects can cast shadows.
	enum class ShadowType : std::uint8_t {
		NONE = 0, ///< The VObject does not cast any shadow.
		BLOB = 1, ///< The VObject casts a basic dark circle at its base.

		// Deprecated entries.
		none ZKREM("renamed to ShadowType::NONE") = NONE,
		blob ZKREM("renamed to ShadowType::BLOB") = BLOB,
	};

	/// \brief Ways a VOb is seen in the game world.
	enum class VisualType : std::uint8_t {
		DECAL = 0,                 ///< The VOb presents as a decal.
		MESH = 1,                  ///< The VOb presents a zenkit::Mesh.
		MULTI_RESOLUTION_MESH = 2, ///< The VOb presents a zenkit::MultiResolutionMesh.
		PARTICLE_EFFECT = 3,       ///< The VOb presents as a particle system.
		AI_CAMERA = 4,             ///< The VOb is a game-controlled camera.
		MODEL = 5,                 ///< The VOb presents a zenkit::Model.
		MORPH_MESH = 6,            ///< The VOb presents a zenkit::MorphMesh.
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

	/// \brief Behavior of a VObject's rotation in relation to the camera.
	enum class SpriteAlignment : std::uint8_t {
		NONE = 0, ///< The sprite is not affected by the camera's rotation.
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

		/// \brief String wind animation.
		///
		/// <p>Indicates that the object should be animated as if shifting in strong wind. Used mostly for
		/// animating grass and other small foliage.</p>
		///
		/// \see http://www.gothic-library.ru/publ/class_zcvob/1-1-0-467#visualAniMode
		WIND = 1,

		/// \brief Light wind animation.
		///
		/// <p>Indicates that the object should be animated as if shifting in light wind. Used mostly for
		/// animating trees.</p>
		///
		/// \see http://www.gothic-library.ru/publ/class_zcvob/1-1-0-467#visualAniMode
		WIND_ALT = 2,

		// Deprecated entries.
		none ZKREM("renamed to AnimationType::NONE") = NONE,
		wind ZKREM("renamed to AnimationType::WIND") = WIND,
		wind2 ZKREM("renamed to AnimationType::WIND_ALT") = WIND_ALT,
	};

	struct Visual : Object {
		/// \brief The type of this visual.
		VisualType type = VisualType::UNKNOWN;

		/// \brief The name of this visual, a decal mesh or particle effect.
		///
		/// <p>This field conains the name of the source file of the visual. A mesh visual, for example, will end in
		/// `.3DS` since that is the original file format of the mesh.</p>
		std::string name;
	};

	struct VisualMesh final : Visual {
		ZK_OBJECT(ObjectType::zCMesh);
	};

	struct VisualMultiResolutionMesh final : Visual {
		ZK_OBJECT(ObjectType::zCProgMeshProto);
	};

	struct VisualParticleEffect final : Visual {
		ZK_OBJECT(ObjectType::zCParticleFX);
	};

	struct VisualCamera final : Visual {
		ZK_OBJECT(ObjectType::zCAICamera);
	};

	struct VisualModel final : Visual {
		ZK_OBJECT(ObjectType::zCModel);
	};

	struct VisualMorphMesh final : Visual {
		ZK_OBJECT(ObjectType::zCMorphMesh);
	};

	/// \brief Decal visual configuration for VObs.
	struct VisualDecal final : Visual {
		ZK_OBJECT(ObjectType::zCDecal);

	public:
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
		ZKREM("use ::load()") ZKAPI static VisualDecal parse(ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
	};

	struct RigidBody {
		glm::vec3 vel;
		std::uint8_t mode;
		bool gravity_enabled;
		float gravity_scale;
		glm::vec3 slide_direction;

		ZKAPI void load(ReadArchive& r, GameVersion version);
	};

	struct EventManager final : Object {
		ZK_OBJECT(ObjectType::zCEventManager);

	private:
		bool cleared = false;
		bool active = false;

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
	};

	struct VirtualObject;

	struct Ai : Object {};

	struct AiHuman final : Ai {
		ZK_OBJECT(ObjectType::oCAIHuman);

	public:
		int water_level;
		float floor_y;
		float water_y;
		float ceil_y;
		float feet_y;
		float head_y;
		float fall_dist_y;
		float fall_start_y;
		std::shared_ptr<VNpc> npc;
		int walk_mode;
		int weapon_mode;
		int wmode_ast;
		int wmode_select;
		bool change_weapon;
		int action_mode;

		void load(ReadArchive& r, GameVersion version) override;
	};

	struct AiMove final : Ai {
		ZK_OBJECT(ObjectType::oCAIVobMove);

	public:
		std::shared_ptr<VirtualObject> vob;
		std::shared_ptr<VNpc> owner;

		void load(ReadArchive& r, GameVersion version) override;
	};

	enum class VirtualObjectType {
		UNKNOWN = -1,
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
	};

	/// \brief The base class of all ZenGin virtual objects.
	///
	/// <p>Contains general VObject settings like its position, rotation, visual and much more. Known fields have been
	/// documented with information about their purpose and additional information may be obtained from the unified
	/// ZenGin documentation at https://zk.gothickit.dev.</p>
	///
	/// <p>ZenGin virtual objects inherit from the base zenkit::Object. Copy and move operations have been
	/// intentionally deleted, because VObject can reference each other through shared pointers. Moving VObjects
	/// while this is the case would break parsing.</p>
	///
	/// \note Virtual objects should not be loaded manually. They are embedded into world archives and are automatically
	///       loaded when the world is loaded.
	struct VirtualObject : Object {
		ZK_OBJECT(ObjectType::zCVob);

	public:
		VirtualObject() = default;

		/// \brief Implicit copy-constructor. Disabled for performance reasons.
		VirtualObject(VirtualObject const&) = delete;

		/// \brief Move-constructor. Disabled for memory safety with shared references.
		VirtualObject(VirtualObject&&) = default;

		/// \brief Default virtual destructor.
		~VirtualObject() override = default;

		/// \brief The type of this VObject.
		///
		/// <p>This field holds the type of the virtual object which determines its function in the game world. There
		/// are lots of virtual objects which are diffentiated by this field. See the type hierarchy diagram of
		/// zenkit::VirtualObject for an overview of availabe VObject types.</p>
		VirtualObjectType type = VirtualObjectType::UNKNOWN;

		/// \brief The index of this VObject in the archive it was read from.
		/// \deprecated This value is only relevant with the specific world archive the object was loaded from.
		///             Essentially, it exposes internal state of the archive parser to the outside. It should no longer
		///             be used since it is not a real unique identifier of any single object and references internal
		///             parser state. If you need to check, if two VObjects are equal, you should do a simple pointer
		///             compare on the shared pointer it's wrapped in.
		uint32_t id ZKREM("scheduled for removal; refer to documentation") = 0;

		/// \brief The bounding box of this VObject.
		AxisAlignedBoundingBox bbox = AxisAlignedBoundingBox::zero();

		/// \brief The position of this VObject in virtual space.
		glm::vec3 position = glm::vec3 {0};

		/// \brief The rotation of this VObject in virtual space.
		glm::mat3x3 rotation = glm::identity<glm::mat3x3>();

		/// \brief Indicates whether this VObject has should display its associated visual.
		bool show_visual = false;

		/// \brief Indicates how this VObject should be aligned in relation to the camera.
		///
		/// <p>The value of this field indicates, if and how this VObject should align itself with the camera. This may
		/// be used with grass or flowers which only consist of a 2-dimensional sprite to have it always face the
		/// camera, for example. See zenkit::SpriteAlignment for additional details.</p>
		SpriteAlignment sprite_camera_facing_mode = SpriteAlignment::NONE;

		/// \brief Indicates whether this VObject should collide with other VObjects.
		///
		/// <p>This is used for placing the object in the ZenGin map editor, the <i>Spacer</i>, where it will prevent
		/// the VObject being placed wither other VObjects. This setting is irrelevant for runtime collision
		/// detection</p>
		bool cd_static = true;

		/// \brief Indicates whether this VObject should collide with dynamic objects.
		///
		/// <p>For this purpose, dynamic objects are the player, NPCs and items. If this flag is set, implementations
		/// should apply collision detection to this VObject.</p>
		bool cd_dynamic = true;

		/// \brief Indicates whether this VObject should be included during static lighting calculations.
		///
		/// <p>These lighting calculations are done at compile-time and will bake VObjects with this flag into the
		/// light-maps available from the world mesh. If set to `true`, this VObject may be excluded from dynamic
		/// lighting if the light-maps are used.</p>
		bool vob_static = false;

		/// \brief The type of dynamic shadow to be used.
		///
		/// <p>See zenkit::ShadowType for information about available shadow types.</p>
		ShadowType dynamic_shadows = ShadowType::NONE;

		/// \brief Indicates whether this VObject has enabled physics.
		///
		/// <p>This field represents internal state of the ZenGin which is written to disk only for a certain world
		/// format type. It is not possible to set this field ZenGin's world editor, the <i>Spacer</i>, for example.
		/// This makes this field unreliable, since it is only available for some VObjects.</p>
		bool physics_enabled = true;

		/// \brief The type of wind animation to apply to the VObject.
		AnimationType anim_mode = AnimationType::NONE;

		/// \brief The depth-bias for this VObject.
		///
		/// <p>This value is passed directly to Direct3D when rendering the associated visual of this VObject. Sadly,
		/// because documentation is not really available for the very old Direct3D version used by the ZenGin, the
		/// exact behavior it would have resulted in are unknown.</p>
		std::int32_t bias = 0;

		/// \brief Indicates that this VObject is <i>ambient</i>.
		///
		/// <p>It looks like this VObject setting was a system used duing development when access to the game's source
		/// code was available. Basically, the global variable `zCWorld::s_bAmbientVobsEnabled` could be used to hide or
		/// show VObjects which have the `isAmbient` flag set. In release builds, this variable is always set to `true`,
		/// thus the `isAmbient` flag does not have any percievable effect on the game.</p>
		/// <p>It follows, that this field should be ignored by most implementations.</p>
		bool ambient = false;

		/// \brief Indicates the strength of the animation set through #anim_mode.
		///
		/// <p>Essentialy, this controls the strength of the wind to be animated using the given #anim_mode. This value
		/// is ignored if #anim_mode is AnimationType::NONE</p>
		float anim_strength = 0;
		float far_clip_scale = 2.0;

		std::string preset_name;

		/// \brief The name of this VObject.
		///
		/// <p>The name of this VObject. VObject names are not necessarily unique but are sometimes helpful for
		/// identfying them in ZenGin's original map editor, the <i>Spacer</i>.</p>
		std::string vob_name;

		std::string visual_name ZKREM("use VirtualObject::visual::name instead") {};
		VisualType associated_visual_type ZKREM("use VirtualObject::visual::type instead") {};
		std::optional<VisualDecal> visual_decal ZKREM("use VirtualObject::visual instead") {};

		/// \brief The visual data associated with this VObject.
		///
		/// <p>All supported visuals except the zenkit::VisualDecal do not contain any additional information. To
		/// determine the type of attached visual, simply access zenkit::VisualDecal::type</p>
		std::shared_ptr<Visual> visual = nullptr;

		std::shared_ptr<Ai> ai = nullptr;

		std::uint8_t sleep_mode = 0;
		float next_on_timer = 0;
		std::optional<RigidBody> rigid_body {};

		/// \brief The children of this VOb.
		std::vector<std::shared_ptr<VirtualObject>> children {};

		ZKREM("use ::load()") ZKAPI static void parse(VirtualObject& obj, ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
	};

	struct VSpot final : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobSpot);
	};

	struct VStair final : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobStair);
	};

	struct VStartPoint final : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobStartpoint);
	};

	struct VLevel final : VirtualObject {
		ZK_OBJECT(ObjectType::zCVobLevelCompo);
	};
} // namespace zenkit
