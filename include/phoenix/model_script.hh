// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>

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

		enum class event_fight_mode {
			fist,
			one_handed,
			two_handed,
			bow,
			crossbow,
			magic,
			none,
		};

		enum animation_flags : uint8_t {
			af_none = 0,
			af_move = 1,
			af_rotate = 2,
			af_queue = 4,
			af_fly = 8,
			af_idle = 16,
		};

		enum class animation_direction : uint8_t {
			forward = 0,
			backward = 1,
		};

		/**
		 * @brief The `meshAndTree` tag
		 * @remark MDS syntax: `meshAndTree(<string> [DONT_USE_MESH])`
		 */
		struct skeleton {
			std::string name;
			bool disable_mesh {false};
		};

		/**
		 * @brief The `modelTag` tag
		 * @remark MDS syntax: `modelTag(<string> <string>)`
		 */
		struct model_tag {
			std::string bone;
		};

		/**
		 * @brief The `*eventTag` tag
		 * @remark MDS syntax: `*eventTag(<int> <string> [<string>] [<string>] [ATTACH])`
		 */
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

		/**
		 * @brief The `*eventPFX` tag
		 * @remark MDS syntax: `*eventPFX(<int> [<int>] <string> <string> [ATTACH])`
		 */
		struct event_pfx {
			std::int32_t frame;
			std::int32_t index {0};
			std::string name;
			std::string position;
			bool attached {false};
		};

		/**
		 * @brief The `*eventCamTremor` tag
		 * @remark MDS syntax: `*eventCamTremor(<int> <int> <int> <int> <int>)`
		 */
		struct event_camera_tremor {
			std::int32_t frame {0};
			std::int32_t field1 {0};
			std::int32_t field2 {0};
			std::int32_t field3 {0};
			std::int32_t field4 {0};
		};

		/**
		 * @brief The `*eventPFXStop` tag
		 * @remark MDS syntax: `*eventPFXStop(<int> <int>)`
		 */
		struct event_pfx_stop {
			std::int32_t frame;
			std::int32_t index;
		};

		/**
		 * @brief The `*eventSFX` tag
		 * @remark MDS syntax: `*eventSFX(<int> <string> [R:<float>] [EMPTY_SLOT])`
		 */
		struct event_sfx {
			std::int32_t frame;
			std::string name;
			float range {1000.0f};
			bool empty_slot {false};
		};

		/**
		 * @brief The `*eventSFXGrnd` tag
		 * @remark MDS syntax: `*eventSFXGrnd(<int> <string>)`
		 */
		struct event_sfx_ground {
			std::int32_t frame;
			std::string name;
			float range {1000.0f};
			bool empty_slot {false};
		};

		/**
		 * @brief The `*eventMMStartAni` tag
		 * @remark MDS syntax: `*eventMMStartAni(<int> <string> [<string>])`
		 */
		struct event_morph_animate {
			std::int32_t frame;
			std::string animation;
			std::string node {};
		};

		/**
		 * @brief The `aniAlias` tag
		 * @remark MDS syntax: `ani(<string> <int> <string> <float> <float> <flags> <string> <F|R> <int> <int>
		 *         [FPS:<float>] [CVS:<float>])`
		 */
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

		/**
		 * @brief The `aniAlias` tag
		 * @remark MDS syntax: `aniAlias(<string> <int> <string> <float> <float> <flags> <string> [<F|R>])`
		 */
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

		/**
		 * @brief The `aniBlend` tag
		 * @remark MDS syntax: `aniBlend(<string> [<int>] <string> [<float> <float>])`
		 */
		struct animation_blending {
			std::string name;
			std::string next;
			float blend_in {0};
			float blend_out {0};
		};

		/**
		 * @brief The `aniComb` tag
		 * @remark MDS syntax: `aniComb(<string> <int> <string> <float> <float> <flags> <string> <int>)`
		 */
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

	/**
	 * @brief Represents a model script containing animation settings.
	 *
	 * Parses ZenGin model scripts. The reference implementation can be found on GitHub:
	 * https://github.com/Try/ZenLib/blob/master/zenload/modelScriptParser.cpp
	 *
	 * Thanks to the original author, Andre Taulien, nigleb, noahndertaler and ousnius as well
	 * as Try for additional work on their ZenLib fork!
	 *
	 * @see https://github.com/ataulien/ZenLib
	 * @see https://github.com/Try/ZenLib
	 */
	class model_script {
	public:
		/**
		 * @brief Parses a model script (MDS) file from the given buffer.
		 * @param buf A buffer to parse from.
		 * @return The parsed model script.
		 */
		[[nodiscard]] static model_script parse(buffer& buf);

		/**
		 * @brief Parses a model script (MDS) file from the given buffer.
		 * @param buf A buffer to parse from.
		 * @return The parsed model script.
		 */
		[[nodiscard]] inline static model_script parse(buffer&& buf) {
			return parse(buf);
		}

		/**
		 * @brief Parses a binary model script (MSB) file from the given buffer.
		 * @param buf A buffer to parse from.
		 * @return The parsed model script.
		 */
		static model_script parse_binary(buffer& buf);

	public:
		mds::skeleton skeleton;
		std::vector<std::string> meshes;
		std::vector<std::string> disabled_animations;
		std::vector<mds::animation_combination> combinations;
		std::vector<mds::animation_blending> blends;
		std::vector<mds::animation_alias> aliases;
		std::vector<mds::model_tag> model_tags;
		std::vector<mds::animation> animations;
	};
} // namespace phoenix
