// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>

namespace phoenix {
	namespace mds {
		enum class event_tag_type {
			unknown,
			create_item,
			insert_item,
			remove_item,
			destroy_item,
			place_item,
			exchange_item,
			fight_mode,
			place_munition,
			remove_munition,
			draw_sound,
			undraw_sound,
			swap_mesh,
			draw_torch,
			inventory_torch,
			drop_torch,
			hit_limb,
			hit_direction,
			dam_multiply,
			par_frame,
			opt_frame,
			hit_end,
			window,
		};

		/// \brief A set of fight stances the player can take.
		enum class event_fight_mode {
			fist,       ///< The player fights with his fists.
			one_handed, ///< The player wields a one-handed weapon.
			two_handed, ///< The player wields a two-handed weapon.
			bow,        ///< The player wields a bow.
			crossbow,   ///< The player wields a crossbow.
			magic,      ///< The player casts a magic spell.
			none,       ///< The player is not in a fighting stance.
			invalid,    ///< A fight mode which acts as an `unset` marker. Added for OpenGothic compatibility.
		};

		enum animation_flags : uint8_t {
			af_none = 0,
			af_move = 1,
			af_rotate = 2,
			af_queue = 4,
			af_fly = 8,
			af_idle = 16,
		};

		/// \brief The way the animation is to be played.
		enum class animation_direction : uint8_t {
			forward = 0,  ///< The animation samples are played from first to last.
			backward = 1, ///< The animation samples are played from last to first.
		};

		/// \brief The `meshAndTree` tag
		/// \remark MDS syntax: `meshAndTree(<string> [DONT_USE_MESH])`
		struct skeleton {
			std::string name;
			bool disable_mesh {false};
		};

		/// \brief The `modelTag` tag
		/// \remark MDS syntax: `modelTag(<string> <string>)`
		struct model_tag {
			std::string bone;
		};

		/// \brief The `*eventTag` tag
		/// \remark MDS syntax: `*eventTag(<int> <string> [<string>] [<string>] [ATTACH])`
		struct event_tag {
			std::int32_t frame;
			event_tag_type type;
			std::string slot {};
			std::string slot2 {};
			std::string item {};
			std::vector<int32_t> frames {};
			event_fight_mode fight_mode {event_fight_mode::none};
			bool attached {false};
		};

		/// \brief The `*eventPFX` tag
		/// \remark MDS syntax: `*eventPFX(<int> [<int>] <string> <string> [ATTACH])`
		struct event_pfx {
			std::int32_t frame;
			std::int32_t index {0};
			std::string name;
			std::string position;
			bool attached {false};
		};

		/// \brief The `*eventCamTremor` tag
		/// \remark MDS syntax: `*eventCamTremor(<int> <int> <int> <int> <int>)`
		struct event_camera_tremor {
			std::int32_t frame {0};
			std::int32_t field1 {0};
			std::int32_t field2 {0};
			std::int32_t field3 {0};
			std::int32_t field4 {0};
		};

		/// \brief The `*eventPFXStop` tag
		/// \remark MDS syntax: `*eventPFXStop(<int> <int>)`
		struct event_pfx_stop {
			std::int32_t frame;
			std::int32_t index;
		};

		/// \brief The `*eventSFX` tag
		/// \remark MDS syntax: `*eventSFX(<int> <string> [R:<float>] [EMPTY_SLOT])`
		struct event_sfx {
			std::int32_t frame;
			std::string name;
			float range {1000.0f};
			bool empty_slot {false};
		};

		/// \brief The `*eventSFXGrnd` tag
		/// \remark MDS syntax: `*eventSFXGrnd(<int> <string>)`
		struct event_sfx_ground {
			std::int32_t frame;
			std::string name;
			float range {1000.0f};
			bool empty_slot {false};
		};

		/// \brief The `*eventMMStartAni` tag
		/// \remark MDS syntax: `*eventMMStartAni(<int> <string> [<string>])`
		struct event_morph_animate {
			std::int32_t frame;
			std::string animation;
			std::string node {};
		};

		/// \brief The `aniAlias` tag
		/// \remark MDS syntax: `ani(<string> <int> <string> <float> <float> <flags> <string> <F|R> <int> <int>
		///         [FPS:<float>] [CVS:<float>])`
		struct animation {
			std::string name;
			std::uint32_t layer;
			std::string next;
			float blend_in;
			float blend_out;
			animation_flags flags;
			std::string model;
			animation_direction direction;
			std::int32_t first_frame;
			std::int32_t last_frame;
			float fps;
			float speed;
			float collision_volume_scale;

			std::vector<event_tag> events {};
			std::vector<event_pfx> pfx {};
			std::vector<event_pfx_stop> pfx_stop {};
			std::vector<event_sfx> sfx {};
			std::vector<event_sfx_ground> sfx_ground {};
			std::vector<event_morph_animate> morph {};
			std::vector<event_camera_tremor> tremors {};
		};

		/// \brief The `aniAlias` tag
		/// \remark MDS syntax: `aniAlias(<string> <int> <string> <float> <float> <flags> <string> [<F|R>])`
		struct animation_alias {
			std::string name;
			std::uint32_t layer;
			std::string next;
			float blend_in;
			float blend_out;
			animation_flags flags;
			std::string alias;
			animation_direction direction;
		};

		/// \brief The `aniBlend` tag
		/// \remark MDS syntax: `aniBlend(<string> [<int>] <string> [<float> <float>])`
		struct animation_blending {
			std::string name;
			std::string next;
			float blend_in {0};
			float blend_out {0};
		};

		/// \brief The `aniComb` tag
		/// \remark MDS syntax: `aniComb(<string> <int> <string> <float> <float> <flags> <string> <int>)`
		struct animation_combination {
			std::string name;
			std::uint32_t layer;
			std::string next;
			float blend_in;
			float blend_out;
			animation_flags flags;
			std::string model;
			std::int32_t last_frame;
		};

		animation_flags animation_flags_from_string(std::string_view str);
	} // namespace mds

	/// \brief Represents a *ZenGin* model script.
	///
	/// <p>Model scripts contain animations related to a model and actions the animation controller should take during
	/// or after an animation plays (such as playing a sound).</p>
	class model_script {
	public:
		/// \brief Parses a model script from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model script.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] static model_script parse(buffer& buf);

		/// \brief Parses a model script from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model script.
		/// \throws parser_error if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] inline static model_script parse(buffer&& buf) {
			return model_script::parse(buf);
		}

		/// \brief Parses a compiled model script from the data in the given buffer.
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed model script.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws parser_error if parsing fails.
		/// \see #parse_binary(buffer&&)
		[[nodiscard]] static model_script parse_binary(buffer& buf);

		// \brief Parses a compiled model script from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed model script.
		/// \throws parser_error if parsing fails.
		/// \see #parse_binary(buffer&)
		[[nodiscard]] inline static model_script parse_binary(buffer&& buf) {
			return model_script::parse_binary(buf);
		}

	public:
		/// \brief The model skeleton this model script was made for.
		mds::skeleton skeleton;

		/// \brief A list of meshes which can be used with this model script.
		std::vector<std::string> meshes;

		/// \brief A list of animation names which are disabled.
		std::vector<std::string> disabled_animations;

		std::vector<mds::animation_combination> combinations;
		std::vector<mds::animation_blending> blends;
		std::vector<mds::animation_alias> aliases;
		std::vector<mds::model_tag> model_tags;
		std::vector<mds::animation> animations;
	};
} // namespace phoenix
