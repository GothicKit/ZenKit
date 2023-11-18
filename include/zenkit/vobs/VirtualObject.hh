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
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace zenkit::vobs {
	struct Npc;
}
namespace zenkit {
	class ReadArchive;

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
	struct VisualDecal : Object {
		static constexpr ObjectType TYPE = ObjectType::zCDecal;

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

		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}

		ZKAPI void load(ReadArchive& r, GameVersion version);
	};

	struct VisualMesh : Object {
		static constexpr ObjectType TYPE = ObjectType::zCMesh;
		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}
	};

	struct VisualMultiResolutionMesh : Object {
		static constexpr ObjectType TYPE = ObjectType::zCProgMeshProto;
		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}
	};

	struct VisualParticleEffect : Object {
		static constexpr ObjectType TYPE = ObjectType::zCParticleFX;
		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}
	};

	struct VisualCamera : Object {
		static constexpr ObjectType TYPE = ObjectType::zCAICamera;
		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}
	};

	struct VisualModel : Object {
		static constexpr ObjectType TYPE = ObjectType::zCModel;
		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}
	};

	struct VisualMorphMesh : Object {
		static constexpr ObjectType TYPE = ObjectType::zCMorphMesh;
		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}
	};

	struct RigidBody {
		glm::vec3 vel;
		std::uint8_t mode;
		bool gravity_enabled;
		float gravity_scale;
		glm::vec3 slide_direction;

		ZKAPI void load(ReadArchive& r, GameVersion version);
	};

	struct EventManager : Object {
		static constexpr ObjectType TYPE = ObjectType::ignored;

		bool cleared;
		bool active;

		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}

		ZKAPI void load(ReadArchive& r, GameVersion version);
	};

	struct AiHuman : Object {
		static constexpr ObjectType TYPE = ObjectType::zCModel;

		int water_level;
		float floor_y;
		float water_y;
		float ceil_y;
		float feet_y;
		float head_y;
		float fall_dist_y;
		float fall_start_y;
		std::shared_ptr<vobs::Npc> npc;
		int walk_mode;
		int weapon_mode;
		int wmode_ast;
		int wmode_select;
		bool change_weapon;
		int action_mode;

		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}

		void load(ReadArchive& r, GameVersion version) override;
	};

	struct VirtualObject;

	struct AiMove : Object {
		static constexpr ObjectType TYPE = ObjectType::zCModel;

		std::shared_ptr<VirtualObject> vob;
		std::shared_ptr<vobs::Npc> owner;

		[[nodiscard]] ObjectType get_type() const override {
			return TYPE;
		}

		void load(ReadArchive& r, GameVersion version) override;
	};

	/// \brief The base class for all VObs.
	///
	/// <p>Contains parameters all VObs have, like their position, bounding box and model.</p>
	struct VirtualObject : Object {
		VirtualObject() = default;
		VirtualObject(VirtualObject const&) = delete;
		VirtualObject(VirtualObject&&) = default;

		struct SaveState {
			uint8_t sleep_mode;
			float next_on_timer;
			std::optional<RigidBody> rigid_body {};
		};

		using save_state ZKREM("renamed to SaveState") = SaveState;

		ObjectType type; ///< The type of this VOb.
		uint32_t id;     ///< The index of this VOb in the archive it was read from.

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

		ZKREM("use ::visual.get_type() instead") VisualType associated_visual_type {};
		ZKREM("use ::visual instead") std::optional<VisualDecal> visual_decal {};
		std::shared_ptr<Object> visual = nullptr;

		// One of: AiHuman (oCAIHuman), AiMove (oCAIVobMove)
		std::shared_ptr<Object> ai = nullptr;

		/// \brief Contains extra data about the item in the context of a saved game.
		std::optional<SaveState> saved {};

		/// \brief The children of this VOb.
		std::vector<std::shared_ptr<VirtualObject>> children {};

		/// \brief Default virtual destructor.
		~VirtualObject() override = default;

		/// \return `true` if this VOb is from a save-game and `false` if not.
		[[nodiscard]] ZKAPI bool is_save_game() const noexcept {
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

		// TODO(lmichaelis): Return a constant value from this!
		[[nodiscard]] ObjectType get_type() const override {
			return this->type;
		}

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
	};
} // namespace zenkit
