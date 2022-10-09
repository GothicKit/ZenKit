// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/model_script.hh>
#include <phoenix/phoenix.hh>

#include "model_script_dsl.hh"

#include <fmt/format.h>
#include <lexy/action/parse.hpp>
#include <lexy/input/buffer.hpp>
#include <lexy_ext/report_error.hpp>

#include <sstream>
#include <unordered_map>

namespace phoenix {
	namespace mds {
		enum class parser_chunk : uint16_t {
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
			event_camera_tremor = 0xF5AA,
			error,
			unknown,
		};

		static const std::unordered_map<std::string, mds::event_tag_type> event_types {
		    {"DEF_CREATE_ITEM", event_tag_type::create_item},
		    {"DEF_INSERT_ITEM", event_tag_type::insert_item},
		    {"DEF_REMOVE_ITEM", event_tag_type::remove_item},
		    {"DEF_DESTROY_ITEM", event_tag_type::destroy_item},
		    {"DEF_PLACE_ITEM", event_tag_type::place_item},
		    {"DEF_EXCHANGE_ITEM", event_tag_type::exchange_item},
		    {"DEF_FIGHTMODE", event_tag_type::fight_mode},
		    {"DEF_PLACE_MUNITION", event_tag_type::place_munition},
		    {"DEF_REMOVE_MUNITION", event_tag_type::remove_munition},
		    {"DEF_DRAWSOUND", event_tag_type::draw_sound},
		    {"DEF_UNDRAWSOUND", event_tag_type::undraw_sound},
		    {"DEF_SWAPMESH", event_tag_type::swap_mesh},
		    {"DEF_DRAWTORCH", event_tag_type::draw_torch},
		    {"DEF_INV_TORCH", event_tag_type::inventory_torch},
		    {"DEF_DROP_TORCH", event_tag_type::drop_torch},
		    {"DEF_HIT_LIMB", event_tag_type::hit_limb},
		    {"HIT_LIMB", event_tag_type::hit_limb},
		    {"DEF_HIT_DIR", event_tag_type::hit_direction},
		    {"DEF_DIR", event_tag_type::hit_direction}, // TODO: Validate this!
		    {"DEF_DAM_MULTIPLY", event_tag_type::dam_multiply},
		    {"DEF_PAR_FRAME", event_tag_type::par_frame},
		    {"DEF_OPT_FRAME", event_tag_type::opt_frame},
		    {"DEF_HIT_END", event_tag_type::hit_end},
		    {"DEF_WINDOW", event_tag_type::window},
		};

		event_tag make_event_tag(int32_t frame,
		                         std::string&& type,
		                         std::optional<std::string>&& a,
		                         std::optional<std::string>&& b,
		                         bool attached) {
			event_tag evt {};
			evt.frame = frame;

			auto tp = event_types.find(type);
			if (tp == event_types.end()) {
				evt.type = mds::event_tag_type::unknown;
				PX_LOGW("model_script: unexpected value for event_tag_type: \"{}\"", type);
			} else {
				evt.type = tp->second;
			}

			switch (evt.type) {
			case mds::event_tag_type::create_item:
			case mds::event_tag_type::exchange_item:
				evt.slot = a.value_or("");
				evt.item = b.value_or("");
				break;
			case mds::event_tag_type::insert_item:
			case mds::event_tag_type::place_munition:
				evt.slot = a.value_or("");
				break;
			case mds::event_tag_type::fight_mode: {
				auto mode = a.value_or("");

				if (mode == "FIST") {
					evt.fight_mode = mds::event_fight_mode::fist;
				} else if (mode == "1H" || mode == "1h") {
					evt.fight_mode = mds::event_fight_mode::one_handed;
				} else if (mode == "2H" || mode == "2h") {
					evt.fight_mode = mds::event_fight_mode::two_handed;
				} else if (mode == "BOW") {
					evt.fight_mode = mds::event_fight_mode::bow;
				} else if (mode == "CBOW") {
					evt.fight_mode = mds::event_fight_mode::crossbow;
				} else if (mode == "MAG") {
					evt.fight_mode = mds::event_fight_mode::magic;
				} else {
					evt.fight_mode = mds::event_fight_mode::none;
				}
				break;
			}
			case mds::event_tag_type::swap_mesh:
				evt.slot = a.value_or("");
				evt.slot2 = b.value_or("");
				break;
			case mds::event_tag_type::hit_limb:
				(void) a.value_or("");
				break;
			case mds::event_tag_type::dam_multiply:
			case mds::event_tag_type::par_frame:
			case mds::event_tag_type::opt_frame:
			case mds::event_tag_type::hit_end:
			case mds::event_tag_type::window: {
				auto frames = a.value_or("");
				std::istringstream stream {frames};

				int32_t fr = 0;
				while (!stream.eof()) {
					stream >> fr;
					evt.frames.push_back(fr);
				}
				break;
			}
			default:
				break;
			}

			evt.attached = attached;
			return evt;
		}

		animation_flags animation_flags_from_string(std::string_view str) {
			uint8_t flags = animation_flags::af_none;

			for (char c : str) {
				switch (c) {
				case 'M':
					flags |= animation_flags::af_move;
					break;
				case 'R':
					flags |= animation_flags::af_rotate;
					break;
				case 'E':
					flags |= animation_flags::af_queue;
					break;
				case 'F':
					flags |= animation_flags::af_fly;
					break;
				case 'I':
					flags |= animation_flags::af_idle;
					break;
				default:
					break;
				}
			}

			return static_cast<animation_flags>(flags);
		}
	} // namespace mds

	static model_script parse_binary_script(buffer& buf) {
		model_script script {};
		mds::parser_chunk type = mds::parser_chunk::unknown;
		int32_t ani_index = -1;

		do {
			type = static_cast<mds::parser_chunk>(buf.get_ushort());

			auto length = buf.get_uint();
			auto chunk = buf.extract(length);

			switch (type) {
			case mds::parser_chunk::mesh_and_tree: {
				mds::skeleton mat {};
				mat.disable_mesh = chunk.get_uint() != 0;
				mat.name = chunk.get_line(false);
				script.skeleton = mat;
				break;
			}
			case mds::parser_chunk::register_mesh:
				script.meshes.push_back(chunk.get_line());
				break;
			case mds::parser_chunk::animation: {
				mds::animation anim {};
				anim.name = chunk.get_line(false);
				anim.layer = chunk.get_uint();
				anim.next = chunk.get_line(false);
				anim.blend_in = chunk.get_float();
				anim.blend_out = chunk.get_float();
				anim.flags = mds::animation_flags_from_string(chunk.get_line(false));
				anim.model = chunk.get_line(false);
				anim.direction = chunk.get_line(false).starts_with('R') ? mds::animation_direction::backward
				                                                        : mds::animation_direction::forward;
				anim.first_frame = chunk.get_int();
				anim.last_frame = chunk.get_int();
				anim.fps = chunk.get_float();
				anim.speed = chunk.get_float();
				anim.collision_volume_scale = chunk.get_float();
				script.animations.push_back(std::move(anim));
				++ani_index;
				break;
			}
			case mds::parser_chunk::animation_alias: {
				mds::animation_alias alias {};
				alias.name = chunk.get_line(false);
				alias.layer = chunk.get_uint();
				alias.next = chunk.get_line(false);
				alias.blend_in = chunk.get_float();
				alias.blend_out = chunk.get_float();
				alias.flags = mds::animation_flags_from_string(chunk.get_line(false));
				alias.alias = chunk.get_line(false);
				alias.direction = chunk.get_line(false).starts_with('R') ? mds::animation_direction::backward
				                                                         : mds::animation_direction::forward;
				script.aliases.push_back(std::move(alias));
				break;
			}
			case mds::parser_chunk::animation_blend: {
				mds::animation_blending blend {};
				blend.name = chunk.get_line(false);
				blend.next = chunk.get_line(false);
				blend.blend_in = chunk.get_float();
				blend.blend_out = chunk.get_float();
				script.blends.push_back(std::move(blend));
				break;
			}
			case mds::parser_chunk::animation_combination: {
				mds::animation_combination combo {};
				combo.name = chunk.get_line(false);
				combo.layer = chunk.get_uint();
				combo.next = chunk.get_line(false);
				combo.blend_in = chunk.get_float();
				combo.blend_out = chunk.get_float();
				combo.flags = mds::animation_flags_from_string(chunk.get_line(false));
				combo.model = chunk.get_line(false);
				combo.last_frame = chunk.get_int();
				script.combinations.push_back(std::move(combo));
				break;
			}
			case mds::parser_chunk::animation_disable:
				script.disabled_animations.push_back(chunk.get_line(false));
				break;
			case mds::parser_chunk::event_camera_tremor: {
				mds::event_camera_tremor trem {};
				trem.frame = chunk.get_int();
				trem.field1 = chunk.get_int();
				trem.field2 = chunk.get_int();
				trem.field3 = chunk.get_int();
				trem.field4 = chunk.get_int();
				script.animations[ani_index].tremors.push_back(std::move(trem));
				break;
			}
			case mds::parser_chunk::event_sfx: {
				mds::event_sfx effect {};
				effect.frame = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.range = chunk.get_float();
				effect.empty_slot = chunk.get_uint() != 0;
				script.animations[ani_index].sfx.push_back(std::move(effect));
				break;
			}
			case mds::parser_chunk::event_sfx_ground: {
				mds::event_sfx_ground effect {};
				effect.frame = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.range = chunk.get_float();
				effect.empty_slot = chunk.get_uint() != 0;
				script.animations[ani_index].sfx_ground.push_back(std::move(effect));
				break;
			}
			case mds::parser_chunk::model_tag: {
				mds::model_tag tag {};
				(void) chunk.get_int();

				auto event_type = chunk.get_line(false);
				if (event_type != "DEF_HIT_LIMB" && event_type != "HIT_LIMB") {
					PX_LOGW("model_script: unexpected type for modelTag: \"{}\"", event_type);
				}

				tag.bone = chunk.get_line(true);
				script.model_tags.push_back(std::move(tag));
				break;
			}
			case mds::parser_chunk::event_tag: {
				mds::event_tag event {};
				event.frame = chunk.get_int();

				auto event_type = chunk.get_line(false);
				auto tp = mds::event_types.find(event_type);
				if (tp == mds::event_types.end()) {
					event.type = mds::event_tag_type::unknown;
					PX_LOGW("model_script: unexpected value for event_tag_type: \"{}\"", event_type);
				} else {
					event.type = tp->second;
				}

				switch (event.type) {
				case mds::event_tag_type::create_item:
				case mds::event_tag_type::exchange_item:
					event.slot = chunk.get_line(true);
					event.item = chunk.get_line(true);
					break;
				case mds::event_tag_type::insert_item:
				case mds::event_tag_type::place_munition:
					event.slot = chunk.get_line(true);
					break;
				case mds::event_tag_type::fight_mode: {
					auto mode = chunk.get_line(true);

					if (mode == "FIST") {
						event.fight_mode = mds::event_fight_mode::fist;
					} else if (mode == "1H" || mode == "1h") {
						event.fight_mode = mds::event_fight_mode::one_handed;
					} else if (mode == "2H" || mode == "2h") {
						event.fight_mode = mds::event_fight_mode::two_handed;
					} else if (mode == "BOW") {
						event.fight_mode = mds::event_fight_mode::bow;
					} else if (mode == "CBOW") {
						event.fight_mode = mds::event_fight_mode::crossbow;
					} else if (mode == "MAG") {
						event.fight_mode = mds::event_fight_mode::magic;
					} else {
						event.fight_mode = mds::event_fight_mode::none;
					}
					break;
				}
				case mds::event_tag_type::swap_mesh:
					event.slot = chunk.get_line(true);
					event.slot2 = chunk.get_line(true);
					break;
				case mds::event_tag_type::hit_limb:
					(void) chunk.get_line(true); // TODO
					break;
				case mds::event_tag_type::hit_direction:
					(void) chunk.get_line(true); // TODO
					break;
				case mds::event_tag_type::draw_sound:
				case mds::event_tag_type::undraw_sound:
				case mds::event_tag_type::remove_munition:
				case mds::event_tag_type::destroy_item:
				case mds::event_tag_type::inventory_torch:
				case mds::event_tag_type::remove_item:
					(void) chunk.get_line(true); // TODO
					break;
				case mds::event_tag_type::dam_multiply:
				case mds::event_tag_type::par_frame:
				case mds::event_tag_type::opt_frame:
				case mds::event_tag_type::hit_end:
				case mds::event_tag_type::window: {
					auto frames = chunk.get_line(true);
					std::istringstream stream {frames};

					int32_t frame = 0;
					while (!stream.eof()) {
						stream >> frame;
						event.frames.push_back(frame);
					}
					break;
				}
				default:
					break;
				}

				script.animations[ani_index].events.push_back(std::move(event));
				break;
			}
			case mds::parser_chunk::event_pfx: {
				mds::event_pfx effect {};
				effect.frame = chunk.get_int();
				effect.index = chunk.get_int();
				effect.name = chunk.get_line(false);
				effect.position = chunk.get_line(false);
				effect.attached = chunk.get_uint() != 0;
				script.animations[ani_index].pfx.push_back(std::move(effect));
				break;
			}
			case mds::parser_chunk::event_pfx_stop: {
				mds::event_pfx_stop effect {};
				effect.frame = chunk.get_int();
				effect.index = chunk.get_int();
				script.animations[ani_index].pfx_stop.push_back(effect);
				break;
			}
			case mds::parser_chunk::event_morph_mesh_animation: {
				mds::event_morph_animate anim {};
				anim.frame = chunk.get_int();
				anim.animation = chunk.get_line(false);
				anim.node = chunk.get_line(false);
				(void) chunk.get_float();
				(void) chunk.get_float();
				script.animations[ani_index].morph.push_back(std::move(anim));
				break;
			}
			case mds::parser_chunk::model_script:
				(void) chunk.get_uint();      // bool
				(void) chunk.get_line(false); // path
				break;
			case mds::parser_chunk::source: {
				(void) date::parse(chunk);    // date
				(void) chunk.get_line(false); // path
				break;
			}
			case mds::parser_chunk::model:
			case mds::parser_chunk::animation_enum:
			case mds::parser_chunk::animation_events_end:
			case mds::parser_chunk::animation_enum_end:
			case mds::parser_chunk::model_end:
			case mds::parser_chunk::model_script_end:
				// empty
				break;
			case mds::parser_chunk::animation_events:
				(void) chunk.get_uint(); // bool
				break;
			case mds::parser_chunk::model_mesh:
			case mds::parser_chunk::unknown:
			default:
				break;
			}

			if (chunk.remaining() != 0) {
				PX_LOGW("model_script: {} bytes remaining in section 0x{:4X}", chunk.remaining(), std::uint16_t(type));
			}
		} while (buf.remaining() > 0);

		return script;
	}

	model_script model_script::parse(buffer& buf) {
		auto potential_chunk_type = buf.get_ushort(buf.position());

		if (potential_chunk_type >= 0xF000 || potential_chunk_type == 0xD000) {
			return parse_binary_script(buf);
		}

		lexy::buffer<lexy::ascii_encoding> b {(char*) buf.array(), buf.remaining()};
		auto result = lexy::parse<mds::d_script>(b, lexy_ext::report_error);

		if (!result.has_value()) {
			throw parser_error {"syntax error"};
		}

		return result.value();
	}

	model_script model_script::parse_binary(buffer& buf) {
		return model_script::parse(buf);
	}
} // namespace phoenix
