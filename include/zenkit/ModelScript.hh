// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <cstdint>
#include <string>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	enum class MdsEventType : uint8_t {
		UNKNOWN = 0,
		ITEM_CREATE = 1,
		ITEM_INSERT = 2,
		ITEM_REMOVE = 3,
		ITEM_DESTROY = 4,
		ITEM_PLACE = 5,
		ITEM_EXCHANGE = 6,
		SET_FIGHT_MODE = 7,
		MUNITION_PLACE = 8,
		MUNITION_REMOVE = 9,
		SOUND_DRAW = 10,
		SOUND_UNDRAW = 11,
		MESH_SWAP = 12,
		TORCH_DRAW = 13,
		TORCH_INVENTORY = 14,
		TORCH_DROP = 15,
		HIT_LIMB = 16,
		HIT_DIRECTION = 17,
		DAMAGE_MULTIPLIER = 18,
		PARRY_FRAME = 19,
		OPTIMAL_FRAME = 20,
		HIT_END = 21,
		COMBO_WINDOW = 22,

		// Deprecated entries.
		unknown ZKREM("renamed to MdsEventType::UNKNOWN") = UNKNOWN,
		create_item ZKREM("renamed to MdsEventType::ITEM_CREATE") = ITEM_CREATE,
		insert_item ZKREM("renamed to MdsEventType::ITEM_INSERT") = ITEM_INSERT,
		remove_item ZKREM("renamed to MdsEventType::ITEM_REMOVE") = ITEM_REMOVE,
		destroy_item ZKREM("renamed to MdsEventType::ITEM_DESTROY") = ITEM_DESTROY,
		place_item ZKREM("renamed to MdsEventType::ITEM_PLACE") = ITEM_PLACE,
		exchange_item ZKREM("renamed to MdsEventType::ITEM_EXCHANGE") = ITEM_EXCHANGE,
		fight_mode ZKREM("renamed to MdsEventType::SET_FIGHT_MODE") = SET_FIGHT_MODE,
		place_munition ZKREM("renamed to MdsEventType::MUNITION_PLACE") = MUNITION_PLACE,
		remove_munition ZKREM("renamed to MdsEventType::MUNITION_REMOVE") = MUNITION_REMOVE,
		draw_sound ZKREM("renamed to MdsEventType::SOUND_DRAW") = SOUND_DRAW,
		undraw_sound ZKREM("renamed to MdsEventType::SOUND_UNDRAW") = SOUND_UNDRAW,
		swap_mesh ZKREM("renamed to MdsEventType::MESH_SWAP") = MESH_SWAP,
		draw_torch ZKREM("renamed to MdsEventType::TORCH_DRAW") = TORCH_DRAW,
		inventory_torch ZKREM("renamed to MdsEventType::TORCH_INVENTORY") = TORCH_INVENTORY,
		drop_torch ZKREM("renamed to MdsEventType::TORCH_DROP") = TORCH_DROP,
		hit_limb ZKREM("renamed to MdsEventType::HIT_LIMB") = HIT_LIMB,
		hit_direction ZKREM("renamed to MdsEventType::HIT_DIRECTION") = HIT_DIRECTION,
		dam_multiply ZKREM("renamed to MdsEventType::DAMAGE_MULTIPLIER") = DAMAGE_MULTIPLIER,
		par_frame ZKREM("renamed to MdsEventType::PARRY_FRAME") = PARRY_FRAME,
		opt_frame ZKREM("renamed to MdsEventType::OPTIMAL_FRAME") = OPTIMAL_FRAME,
		hit_end ZKREM("renamed to MdsEventType::HIT_END") = HIT_END,
		window ZKREM("renamed to MdsEventType::COMBO_WINDOW") = COMBO_WINDOW,
	};

	/// \brief A set of fight stances the player can take.
	enum class MdsFightMode : uint8_t {
		FIST = 0,          ///< The player fights with his fists.
		SINGLE_HANDED = 1, ///< The player wields a one-handed weapon.
		DUAL_HANDED = 2,   ///< The player wields a two-handed weapon.
		BOW = 3,           ///< The player wields a bow.
		CROSSBOW = 4,      ///< The player wields a crossbow.
		MAGIC = 5,         ///< The player casts a magic spell.
		NONE = 6,          ///< The player is not in a fighting stance.
		INVALID = 0xFF,    ///< A fight mode which acts as an `unset` marker. Added for OpenGothic compatibility.

		// Deprecated entries.
		fist ZKREM("renamed to MdsFightMode::FIST") = FIST,
		one_handed ZKREM("renamed to MdsFightMode::SINGLE_HANDED") = SINGLE_HANDED,
		two_handed ZKREM("renamed to MdsFightMode::DUAL_HANDED") = DUAL_HANDED,
		bow ZKREM("renamed to MdsFightMode::BOW") = BOW,
		crossbow ZKREM("renamed to MdsFightMode::CROSSBOW") = CROSSBOW,
		magic ZKREM("renamed to MdsFightMode::MAGIC") = MAGIC,
		none ZKREM("renamed to MdsFightMode::NONE") = NONE,
		invalid ZKREM("renamed to MdsFightMode::INVALID") = INVALID,
	};

	enum class AnimationFlags : uint8_t {
		NONE = 0,
		MOVE = 1,
		ROTATE = 2,
		QUEUE = 4,
		FLY = 8,
		IDLE = 16,
		INPLACE = 32,

		af_none ZKREM("renamed to AnimationFlags::NONE") = NONE,
		af_move ZKREM("renamed to AnimationFlags::MOVE") = MOVE,
		af_rotate ZKREM("renamed to AnimationFlags::ROTATE") = ROTATE,
		af_queue ZKREM("renamed to AnimationFlags::QUEUE") = QUEUE,
		af_fly ZKREM("renamed to AnimationFlags::FLY") = FLY,
		af_idle ZKREM("renamed to AnimationFlags::IDLE") = IDLE,
		af_inplace ZKREM("renamed to AnimationFlags::INPLACE") = INPLACE,
	};

	[[nodiscard]] ZKAPI bool operator&(AnimationFlags a, AnimationFlags b);
	[[nodiscard]] ZKAPI AnimationFlags operator|(AnimationFlags a, AnimationFlags b);
	ZKAPI AnimationFlags& operator|=(AnimationFlags& a, AnimationFlags b);

	/// \brief The way the animation is to be played.
	enum class AnimationDirection : uint8_t {
		FORWARD = 0,  ///< The animation samples are played from first to last.
		BACKWARD = 1, ///< The animation samples are played from last to first.

		// Deprecated entries.
		forward ZKREM("renamed to AnimationDirection::FORWARD") = FORWARD,
		backward ZKREM("renamed to AnimationDirection::BACKWARD") = BACKWARD,
	};

	/// \brief The `meshAndTree` tag
	/// \remark MDS syntax: `meshAndTree(<string> [DONT_USE_MESH])`
	struct MdsSkeleton {
		std::string name;
		bool disable_mesh {false};
	};

	/// \brief The `modelTag` tag
	/// \remark MDS syntax: `modelTag(<string> <string>)`
	struct MdsModelTag {
		std::string bone;
	};

	/// \brief The `*eventTag` tag
	/// \remark MDS syntax: `*eventTag(<int> <string> [<string>] [<string>] [ATTACH])`
	struct MdsEventTag {
		std::int32_t frame;
		MdsEventType type;
		std::string slot {};
		std::string slot2 {};
		std::string item {};
		std::vector<int32_t> frames {};
		MdsFightMode fight_mode {MdsFightMode::NONE};
		bool attached {false};
	};

	/// \brief The `*eventPFX` tag
	/// \remark MDS syntax: `*eventPFX(<int> [<int>] <string> <string> [ATTACH])`
	struct MdsParticleEffect {
		std::int32_t frame;
		std::int32_t index {0};
		std::string name;
		std::string position;
		bool attached {false};
	};

	/// \brief The `*eventCamTremor` tag
	/// \remark MDS syntax: `*eventCamTremor(<int> <int> <int> <int> <int>)`
	struct MdsCameraTremor {
		std::int32_t frame {0};
		std::int32_t field1 {0};
		std::int32_t field2 {0};
		std::int32_t field3 {0};
		std::int32_t field4 {0};
	};

	/// \brief The `*eventPFXStop` tag
	/// \remark MDS syntax: `*eventPFXStop(<int> <int>)`
	struct MdsParticleEffectStop {
		std::int32_t frame;
		std::int32_t index;
	};

	/// \brief The `*eventSFX` tag
	/// \remark MDS syntax: `*eventSFX(<int> <string> [R:<float>] [EMPTY_SLOT])`
	struct MdsSoundEffect {
		std::int32_t frame;
		std::string name;
		float range {1000.0f};
		bool empty_slot {false};
	};

	/// \brief The `*eventSFXGrnd` tag
	/// \remark MDS syntax: `*eventSFXGrnd(<int> <string>)`
	struct MdsSoundEffectGround {
		std::int32_t frame;
		std::string name;
		float range {1000.0f};
		bool empty_slot {false};
	};

	/// \brief The `*eventMMStartAni` tag
	/// \remark MDS syntax: `*eventMMStartAni(<int> <string> [<string>])`
	struct MdsMorphAnimation {
		std::int32_t frame;
		std::string animation;
		std::string node {};
	};

	/// \brief The `aniAlias` tag
	/// \remark MDS syntax: `ani(<string> <int> <string> <float> <float> <flags> <string> <F|R> <int> <int>
	///         [FPS:<float>] [CVS:<float>])`
	struct MdsAnimation {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		AnimationFlags flags {AnimationFlags::NONE};
		std::string model;
		AnimationDirection direction;
		std::int32_t first_frame;
		std::int32_t last_frame;
		float fps;
		float speed;
		float collision_volume_scale;

		std::vector<MdsEventTag> events {};
		std::vector<MdsParticleEffect> pfx {};
		std::vector<MdsParticleEffectStop> pfx_stop {};
		std::vector<MdsSoundEffect> sfx {};
		std::vector<MdsSoundEffectGround> sfx_ground {};
		std::vector<MdsMorphAnimation> morph {};
		std::vector<MdsCameraTremor> tremors {};
	};

	/// \brief The `aniAlias` tag
	/// \remark MDS syntax: `aniAlias(<string> <int> <string> <float> <float> <flags> <string> [<F|R>])`
	struct MdsAnimationAlias {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		AnimationFlags flags {AnimationFlags::NONE};
		std::string alias;
		AnimationDirection direction;
	};

	/// \brief The `aniBlend` tag
	/// \remark MDS syntax: `aniBlend(<string> [<int>] <string> [<float> <float>])`
	struct MdsAnimationBlend {
		std::string name;
		std::string next;
		float blend_in {0};
		float blend_out {0};
	};

	/// \brief The `aniComb` tag
	/// \remark MDS syntax: `aniComb(<string> <int> <string> <float> <float> <flags> <string> <int>)`
	struct MdsAnimationCombine {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		AnimationFlags flags {AnimationFlags::NONE};
		std::string model;
		std::int32_t last_frame;
	};

	/// \brief Represents a *ZenGin* model script.
	///
	/// <p>Model scripts contain animations related to a model and actions the animation controller should take during
	/// or after an animation plays (such as playing a sound).</p>
	class ModelScript {
	public:
		/// \brief Parses a model script from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model script.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static ModelScript parse(phoenix::buffer& buf);

		/// \brief Parses a model script from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model script.
		/// \throws ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static ModelScript parse(phoenix::buffer&& buf);

		ZKAPI void load(Read* r);

	private:
		ZKINT void load_binary(Read* r);
		ZKINT void load_source(Read* r);

	public:
		/// \brief The model skeleton this model script was made for.
		MdsSkeleton skeleton {};

		/// \brief A list of meshes which can be used with this model script.
		std::vector<std::string> meshes {};

		/// \brief A list of animation names which are disabled.
		std::vector<std::string> disabled_animations {};

		std::vector<MdsAnimationCombine> combinations {};
		std::vector<MdsAnimationBlend> blends {};
		std::vector<MdsAnimationAlias> aliases {};
		std::vector<MdsModelTag> model_tags {};
		std::vector<MdsAnimation> animations {};
	};
} // namespace zenkit
