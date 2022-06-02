// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>

#include <optional>

namespace phoenix {
	enum class model_script_animation_flags : uint8_t {
		none = 0,
		move = 1,
		rotate = 2,
		queue = 4,
		fly = 8,
		idle = 16,
	};

	inline model_script_animation_flags operator|(model_script_animation_flags a, model_script_animation_flags b) {
		return static_cast<model_script_animation_flags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	inline model_script_animation_flags operator&(model_script_animation_flags a, model_script_animation_flags b) {
		return static_cast<model_script_animation_flags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}

	enum class model_script_animation_direction : uint8_t {
		forward,
		backward,
	};

	enum class model_script_event_type {
		none,
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
		window
	};

	enum class model_script_fight_mode {
		fist,
		one_handed,
		two_handed,
		bow,
		crossbow,
		magic,
		none
	};

	struct model_script_sound_effect {
		std::int32_t frame;
		std::string name;
		float range;
		bool empty_slot;
	};

	struct model_script_particle_effect {
		std::int32_t frame;
		std::int32_t index;
		std::string name;
		std::string position;
		bool attached;
	};

	struct model_script_particle_effect_stop {
		std::int32_t frame;
		std::int32_t index;
	};

	struct model_script_morph_animation {
		std::int32_t frame;
		std::string animation;
		std::string node;
	};

	struct model_script_event {
		std::int32_t frame;
		model_script_event_type type;
		std::string slot;
		std::string slot2;
		std::string item;
		std::vector<int32_t> frames;
		model_script_fight_mode fight_mode {model_script_fight_mode::none};
	};

	struct model_script_animation {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		model_script_animation_flags flags;
		std::string model;
		model_script_animation_direction direction;
		std::int32_t first_frame;
		std::int32_t last_frame;
		float fps;
		float speed;
		float collision_volume_scale;

		std::vector<model_script_sound_effect> sfx;
		std::vector<model_script_particle_effect> pfx;
		std::vector<model_script_particle_effect_stop> pfx_stop;
		std::vector<model_script_morph_animation> morph_animations;
		std::vector<model_script_event> event_tags;

	};

	struct model_script_animation_alias {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		model_script_animation_flags flags;
		std::string alias;
		model_script_animation_direction direction;
	};

	struct model_script_animation_blend {
		std::string name;
		std::string next;
		float blend_in;
		float blend_out;
	};

	struct model_script_animation_combination {
		std::string name;
		std::uint32_t layer;
		std::string next;
		float blend_in;
		float blend_out;
		model_script_animation_flags flags;
		std::string model;
		std::int32_t last_frame;
	};

	struct model_script_mesh_and_tree {
		std::string name;
		bool disable_mesh;
	};

	model_script_animation_flags animation_flags_from_string(std::string_view str);
	model_script_event_type event_type_from_string(const std::string& str);

	class model_script {
	public:
		/**
		 * @brief Parses a binary model script (MSB) file from the given buffer.
		 * @param buf A buffer to parse from.
		 * @return The parsed model script.
		 */
		static model_script parse_binary(buffer& buf);

		model_script_mesh_and_tree mesh_and_tree {};
		std::vector<std::string> meshes;
		std::vector<model_script_event> model_tags;
		std::vector<model_script_animation> animations;

		std::vector<model_script_animation_alias> aliases;
		std::vector<model_script_animation_blend> blends;
		std::vector<model_script_animation_combination> combinations;
		std::vector<std::string> disable;
	};

}
