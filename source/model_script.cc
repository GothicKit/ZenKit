// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/detail/compat.hh>
#include <phoenix/detail/error.hh>
#include <phoenix/mesh.hh>
#include <phoenix/model_script.hh>

#include <fmt/format.h>

#include <sstream>
#include <unordered_map>

namespace phoenix {
	enum class model_script_chunk : uint16_t {
		model_mesh = 0xD000,
		model_script = 0xF000,
		model_script_end = 0xFFFF,
		source = 0xF100,
		model = 0xF200,
		model_end = 0xF2FF,
		mesh_and_tree = 0xF300,
		register_mesh = 0xF400,
		animation_enum = 0xF500,
		animation_enum_end = 0xF5FF,
		// CHUNK_ANI_MAX_FPS           = 0xF510,
		animation = 0xF520,
		animation_alias = 0xF530,
		animation_blend = 0xF540,
		// CHUNK_ANI_SYNC              = 0xF550,
		// CHUNK_ANI_BATCH             = 0xF560,
		animation_combination = 0xF570,
		animation_disable = 0xF580,
		model_tag = 0xF590,
		animation_events = 0xF5A0,
		animation_events_end = 0xF5AF,
		event_sfx = 0xF5A1,
		event_sfx_ground = 0xF5A2,
		event_tag = 0xF5A3,
		event_pfx = 0xF5A4,
		event_pfx_stop = 0xF5A5,
		// CHUNK_EVENT_PFX_GRND        = 0xF5A6,
		// CHUNK_EVENT_SET_MESH        = 0xF5A7,
		// CHUNK_EVENT_SWAP_MESH       = 0xF5A8,
		event_morph_mesh_animation = 0xF5A9,
		// CHUNK_EVENT_CAMTREMOR       = 0xF5AA
		unknown,
	};


	enum class parser_state : uint32_t {
		root = 0,
		model = 1,
		animation_list = 2,
		animation = 3,
		animation_events = 4,
	};

	parser_state operator++(parser_state a) {
		return static_cast<parser_state>(static_cast<uint32_t>(a) + 1);
	}

	parser_state operator--(parser_state a) {
		return static_cast<parser_state>(static_cast<uint32_t>(a) - 1);
	}

	static const std::unordered_map<std::string, model_script_event_type> event_types {
	    {"DEF_CREATE_ITEM", model_script_event_type::create_item},
	    {"DEF_INSERT_ITEM", model_script_event_type::insert_item},
	    {"DEF_REMOVE_ITEM", model_script_event_type::remove_item},
	    {"DEF_DESTROY_ITEM", model_script_event_type::destroy_item},
	    {"DEF_PLACE_ITEM", model_script_event_type::place_item},
	    {"DEF_EXCHANGE_ITEM", model_script_event_type::exchange_item},
	    {"DEF_FIGHTMODE", model_script_event_type::fight_mode},
	    {"DEF_PLACE_MUNITION", model_script_event_type::place_munition},
	    {"DEF_REMOVE_MUNITION", model_script_event_type::remove_munition},
	    {"DEF_DRAWSOUND", model_script_event_type::draw_sound},
	    {"DEF_UNDRAWSOUND", model_script_event_type::undraw_sound},
	    {"DEF_SWAPMESH", model_script_event_type::swap_mesh},
	    {"DEF_DRAWTORCH", model_script_event_type::draw_torch},
	    {"DEF_INV_TORCH", model_script_event_type::inventory_torch},
	    {"DEF_DROP_TORCH", model_script_event_type::drop_torch},
	    {"DEF_HIT_LIMB", model_script_event_type::hit_limb},
	    {"HIT_LIMB", model_script_event_type::hit_limb},
	    {"DEF_HIT_DIR", model_script_event_type::hit_direction},
	    {"DEF_DAM_MULTIPLY", model_script_event_type::dam_multiply},
	    {"DEF_PAR_FRAME", model_script_event_type::par_frame},
	    {"DEF_OPT_FRAME", model_script_event_type::opt_frame},
	    {"DEF_HIT_END", model_script_event_type::hit_end},
	    {"DEF_WINDOW", model_script_event_type::window},
	};

	model_script_animation_flags animation_flags_from_string(std::string_view str) {
		model_script_animation_flags flags = model_script_animation_flags::none;

		for (char c : str) {
			switch (c) {
			case 'M':
				flags = flags | model_script_animation_flags::move;
				break;
			case 'R':
				flags = flags | model_script_animation_flags::rotate;
				break;
			case 'E':
				flags = flags | model_script_animation_flags::queue;
				break;
			case 'F':
				flags = flags | model_script_animation_flags::fly;
				break;
			case 'I':
				flags = flags | model_script_animation_flags::idle;
				break;
			default:
				break;
			}
		}

		return flags;
	}

	model_script_event_type event_type_from_string(const std::string& str) {
		return event_types.at(str);
	}

	model_script model_script::parse(buffer&) {
		model_script script {};
		return script;
	}

	model_script model_script::parse_binary(buffer& buf) {
		model_script script {};
		model_script_chunk type = model_script_chunk::unknown;
		parser_state state = parser_state::root;
		int32_t animation_index = -1;

		do {
			type = static_cast<model_script_chunk>(buf.get_ushort());

			auto length = buf.get_uint();
			auto chunk = buf.extract(length);

			switch (type) {
			case model_script_chunk::model_script:
				(void) chunk.get_uint();      // bool
				(void) chunk.get_line(false); // path
				break;
			case model_script_chunk::source:
				(void) date::parse(chunk);    // date
				(void) chunk.get_line(false); // path
				break;
			case model_script_chunk::model:
				++state;
				break;
			case model_script_chunk::mesh_and_tree: {
				script.mesh_and_tree.disable_mesh = chunk.get_uint() != 0;
				script.mesh_and_tree.name = chunk.get_line(false);
				break;
			}
			case model_script_chunk::register_mesh:
				script.meshes.push_back(chunk.get_line());
				break;
			case model_script_chunk::animation_enum:
				++state;
				break;
			case model_script_chunk::animation_alias: {
				model_script_animation_alias alias {};
				alias.name = chunk.get_line(false);
				alias.layer = chunk.get_uint();
				alias.next = chunk.get_line(false);
				alias.blend_in = chunk.get_float();
				alias.blend_out = chunk.get_float();
				alias.flags = animation_flags_from_string(chunk.get_line(false));
				alias.alias = chunk.get_line(false);
				alias.direction = chunk.get_line(false).starts_with('R') ? model_script_animation_direction::backward
				                                                         : model_script_animation_direction::forward;
				script.aliases.push_back(std::move(alias));
				break;
			}
			case model_script_chunk::animation_blend: {
				model_script_animation_blend blend {};
				blend.name = chunk.get_line(false);
				blend.next = chunk.get_line(false);
				blend.blend_in = chunk.get_float();
				blend.blend_out = chunk.get_float();
				script.blends.push_back(std::move(blend));
				break;
			}
			case model_script_chunk::animation_combination: {
				model_script_animation_combination combo {};
				combo.name = chunk.get_line(false);
				combo.layer = chunk.get_uint();
				combo.next = chunk.get_line(false);
				combo.blend_in = chunk.get_float();
				combo.blend_out = chunk.get_float();
				combo.flags = animation_flags_from_string(chunk.get_line(false));
				combo.model = chunk.get_line(false);
				combo.last_frame = chunk.get_int();
				script.combinations.push_back(std::move(combo));
				break;
			}
			case model_script_chunk::animation_disable:
				script.disable.push_back(chunk.get_line(false));
				break;
			case model_script_chunk::model_tag: {
				model_script_event event {};
				event.frame = chunk.get_int();
				event.type = event_types.at(chunk.get_line(false));
				(void) chunk.get_line(true);
				script.model_tags.push_back(std::move(event));
				break;
			}
			case model_script_chunk::animation: {
				model_script_animation anim {};
				anim.name = chunk.get_line(false);
				anim.layer = chunk.get_uint();
				anim.next = chunk.get_line(false);
				anim.blend_in = chunk.get_float();
				anim.blend_out = chunk.get_float();
				anim.flags = animation_flags_from_string(chunk.get_line(false));
				anim.model = chunk.get_line(false);
				anim.direction = chunk.get_line(false).starts_with('R') ? model_script_animation_direction::backward
				                                                        : model_script_animation_direction::forward;
				anim.first_frame = chunk.get_int();
				anim.last_frame = chunk.get_int();
				anim.fps = chunk.get_float();
				anim.speed = chunk.get_float();
				anim.collision_volume_scale = chunk.get_float();
				script.animations.push_back(std::move(anim));
				++animation_index;
				break;
			}
			case model_script_chunk::animation_events:
				(void) chunk.get_uint(); // bool?
				++state;
				break;
			case model_script_chunk::event_sfx:
			case model_script_chunk::event_sfx_ground: {
				model_script_sound_effect effect {};
				effect.frame = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.range = chunk.get_float();
				effect.empty_slot = chunk.get_uint() != 0;
				script.animations[animation_index].sfx.push_back(std::move(effect));
				break;
			}
			case model_script_chunk::event_pfx: {
				model_script_particle_effect effect {};
				effect.frame = chunk.get_int();
				effect.index = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.position = chunk.get_line(false);
				effect.attached = chunk.get_uint() != 0;
				script.animations[animation_index].pfx.push_back(std::move(effect));
				break;
			}
			case model_script_chunk::event_pfx_stop: {
				model_script_particle_effect_stop effect {};
				effect.frame = chunk.get_int();
				effect.index = chunk.get_int();
				script.animations[animation_index].pfx_stop.push_back(std::move(effect));
				break;
			}
			case model_script_chunk::event_morph_mesh_animation: {
				model_script_morph_animation anim {};
				anim.frame = chunk.get_int();
				anim.animation = chunk.get_line(false);
				anim.node = chunk.get_line(false);
				script.animations[animation_index].morph_animations.push_back(std::move(anim));
				break;
			}
			case model_script_chunk::event_tag: {
				model_script_event event {};
				event.frame = chunk.get_int();
				event.type = event_types.at(chunk.get_line(false));

				switch (event.type) {
				case model_script_event_type::create_item:
				case model_script_event_type::exchange_item:
					event.slot = chunk.get_line(false);
					event.item = chunk.get_line(false);
					break;
				case model_script_event_type::insert_item:
				case model_script_event_type::place_munition:
					event.slot = chunk.get_line(false);
					break;
				case model_script_event_type::fight_mode: {
					auto mode = chunk.get_line(false);

					if (mode == "FIST") {
						event.fight_mode = model_script_fight_mode::fist;
					} else if (mode == "1H" || mode == "1h") {
						event.fight_mode = model_script_fight_mode::one_handed;
					} else if (mode == "2H" || mode == "2h") {
						event.fight_mode = model_script_fight_mode::two_handed;
					} else if (mode == "BOW") {
						event.fight_mode = model_script_fight_mode::bow;
					} else if (mode == "CBOW") {
						event.fight_mode = model_script_fight_mode::crossbow;
					} else if (mode == "MAG") {
						event.fight_mode = model_script_fight_mode::magic;
					} else {
						event.fight_mode = model_script_fight_mode::none;
					}
					break;
				}
				case model_script_event_type::swap_mesh:
					event.slot = chunk.get_line(false);
					event.slot2 = chunk.get_line(false);
					break;
				case model_script_event_type::hit_limb:
					(void) chunk.get_line(true);
					break;
				case model_script_event_type::dam_multiply:
				case model_script_event_type::par_frame:
				case model_script_event_type::opt_frame:
				case model_script_event_type::hit_end:
				case model_script_event_type::window: {
					auto frames = chunk.get_line(true);
					std::istringstream stream {frames};

					int frame = 0;
					while (!stream.eof()) {
						stream >> frame;
						if (stream.eof())
							break;
						event.frames.push_back(frame);
					}
					break;
				}
				default:
					break;
				}

				script.animations[animation_index].event_tags.push_back(std::move(event));
				break;
			}

			case model_script_chunk::animation_events_end:
				--state;
				break;
			case model_script_chunk::animation_enum_end:
				--state;
				break;
			case model_script_chunk::model_end:
				--state;
				break;
			case model_script_chunk::model_script_end:
				--state;
				break;
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				fmt::print(stderr,
				           "warning: model script: not all data consumed from section 0x{:X}\n",
				           std::uint16_t(type));
			}
		} while (buf.remaining() > 0);

		return script;
	}
} // namespace phoenix
