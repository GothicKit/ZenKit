// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/ModelScript.hh"
#include "zenkit/Date.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"
#include "ModelScriptDsl.hh"

#include <sstream>
#include <unordered_map>

namespace zenkit {
	enum class ModelScriptBinaryChunkType : uint16_t {
		// model_mesh = 0xD000,
		ROOT = 0xF000,
		END = 0xFFFF,
		SOURCE = 0xF100,
		// model = 0xF200,
		// model_end = 0xF2FF,
		MESH_AND_TREE = 0xF300,
		REGISTER_MESH = 0xF400,
		// animation_enum = 0xF500,
		// animation_enum_end = 0xF5FF,
		// CHUNK_ANI_MAX_FPS           = 0xF510,
		ANIMATION = 0xF520,
		ANIMATION_ALIAS = 0xF530,
		ANIMATION_BLEND = 0xF540,
		// CHUNK_ANI_SYNC              = 0xF550,
		// CHUNK_ANI_BATCH             = 0xF560,
		ANIMATION_COMBINE = 0xF570,
		ANIMATION_DISABLE = 0xF580,
		MODEL_TAG = 0xF590,
		ANIMATION_EVENTS = 0xF5A0,
		// animation_events_end = 0xF5AF,
		EVENT_SFX = 0xF5A1,
		EVENT_SFX_GROUND = 0xF5A2,
		EVENT_TAG = 0xF5A3,
		EVENT_PFX = 0xF5A4,
		EVENT_PFX_STOP = 0xF5A5,
		// CHUNK_EVENT_PFX_GRND        = 0xF5A6,
		// CHUNK_EVENT_SET_MESH        = 0xF5A7,
		// CHUNK_EVENT_SWAP_MESH       = 0xF5A8,
		EVENT_MM_ANI = 0xF5A9,
		EVENT_CAMERA_TREMOR = 0xF5AA,
		UNKNOWN,
	};

	static std::unordered_map<std::string, MdsEventType> const event_types {
	    {"DEF_CREATE_ITEM", MdsEventType::ITEM_CREATE},
	    {"DEF_INSERT_ITEM", MdsEventType::ITEM_INSERT},
	    {"DEF_REMOVE_ITEM", MdsEventType::ITEM_REMOVE},
	    {"DEF_DESTROY_ITEM", MdsEventType::ITEM_DESTROY},
	    {"DEF_PLACE_ITEM", MdsEventType::ITEM_PLACE},
	    {"DEF_EXCHANGE_ITEM", MdsEventType::ITEM_EXCHANGE},
	    {"DEF_FIGHTMODE", MdsEventType::SET_FIGHT_MODE},
	    {"DEF_PLACE_MUNITION", MdsEventType::MUNITION_PLACE},
	    {"DEF_REMOVE_MUNITION", MdsEventType::MUNITION_REMOVE},
	    {"DEF_DRAWSOUND", MdsEventType::SOUND_DRAW},
	    {"DEF_UNDRAWSOUND", MdsEventType::SOUND_UNDRAW},
	    {"DEF_SWAPMESH", MdsEventType::MESH_SWAP},
	    {"DEF_DRAWTORCH", MdsEventType::TORCH_DRAW},
	    {"DEF_INV_TORCH", MdsEventType::TORCH_INVENTORY},
	    {"DEF_DROP_TORCH", MdsEventType::TORCH_DROP},
	    {"DEF_HIT_LIMB", MdsEventType::HIT_LIMB},
	    {"HIT_LIMB", MdsEventType::HIT_LIMB},
	    {"DEF_HIT_DIR", MdsEventType::HIT_DIRECTION},
	    {"DEF_DIR", MdsEventType::HIT_DIRECTION}, // TODO: Validate this!
	    {"DEF_DAM_MULTIPLY", MdsEventType::DAMAGE_MULTIPLIER},
	    {"DEF_PAR_FRAME", MdsEventType::PARRY_FRAME},
	    {"DEF_OPT_FRAME", MdsEventType::OPTIMAL_FRAME},
	    {"DEF_HIT_END", MdsEventType::HIT_END},
	    {"DEF_WINDOW", MdsEventType::COMBO_WINDOW},
	};

	namespace mds {
		MdsEventTag make_event_tag(int32_t frame,
		                           std::string&& type,
		                           std::optional<std::string>&& a,
		                           std::optional<std::string>&& b,
		                           bool attached) {
			MdsEventTag evt {};
			evt.frame = frame;

			if (auto tp = event_types.find(type); tp == event_types.end()) {
				evt.type = MdsEventType::UNKNOWN;
				ZKLOGW("ModelScript", "Unexpected value for MdsEventType: \"%s\"", type.c_str());
			} else {
				evt.type = tp->second;
			}

			switch (evt.type) {
			case MdsEventType::ITEM_CREATE:
			case MdsEventType::ITEM_EXCHANGE:
				evt.slot = a.value_or("");
				evt.item = b.value_or("");
				break;
			case MdsEventType::ITEM_INSERT:
			case MdsEventType::MUNITION_PLACE:
				evt.slot = a.value_or("");
				break;
			case MdsEventType::SET_FIGHT_MODE: {
				if (auto mode = a.value_or(""); mode == "FIST") {
					evt.fight_mode = MdsFightMode::FIST;
				} else if (mode == "1H" || mode == "1h") {
					evt.fight_mode = MdsFightMode::SINGLE_HANDED;
				} else if (mode == "2H" || mode == "2h") {
					evt.fight_mode = MdsFightMode::DUAL_HANDED;
				} else if (mode == "BOW") {
					evt.fight_mode = MdsFightMode::BOW;
				} else if (mode == "CBOW") {
					evt.fight_mode = MdsFightMode::CROSSBOW;
				} else if (mode == "MAG") {
					evt.fight_mode = MdsFightMode::MAGIC;
				} else {
					evt.fight_mode = MdsFightMode::NONE;
				}
				break;
			}
			case MdsEventType::MESH_SWAP:
				evt.slot = a.value_or("");
				evt.slot2 = b.value_or("");
				break;
			case MdsEventType::HIT_LIMB:
				evt.slot = a.value_or("");
				break;
			case MdsEventType::DAMAGE_MULTIPLIER:
			case MdsEventType::PARRY_FRAME:
			case MdsEventType::OPTIMAL_FRAME:
			case MdsEventType::HIT_END:
			case MdsEventType::COMBO_WINDOW: {
				auto frames = a.value_or("");
				std::istringstream stream {frames};

				int32_t fr = 0;
				while (!stream.eof() && !stream.fail()) {
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

		AnimationFlags animation_flags_from_string(std::string_view str) {
			auto flags = AnimationFlags::NONE;

			for (char c : str) {
				switch (c) {
				case 'M':
					flags |= AnimationFlags::MOVE;
					break;
				case 'R':
					flags |= AnimationFlags::ROTATE;
					break;
				case 'E':
					flags |= AnimationFlags::QUEUE;
					break;
				case 'F':
					flags |= AnimationFlags::FLY;
					break;
				case 'I':
					flags |= AnimationFlags::IDLE;
					break;
				case 'P':
					flags |= AnimationFlags::INPLACE;
					break;
				default:
					break;
				}
			}

			return flags;
		}
	} // namespace mds

	void parse_binary_script(ModelScript& script, Read* r) {
		auto ani_index = static_cast<uint32_t>(-1);
		proto::read_chunked<ModelScriptBinaryChunkType>(
		    r,
		    "ModelScript.Binary",
		    [&script, &ani_index](Read* c, ModelScriptBinaryChunkType type) {
			    switch (type) {
			    case ModelScriptBinaryChunkType::MESH_AND_TREE: {
				    MdsSkeleton mat {};
				    mat.disable_mesh = c->read_uint() != 0;
				    mat.name = c->read_line(false);
				    script.skeleton = mat;
				    break;
			    }
			    case ModelScriptBinaryChunkType::REGISTER_MESH:
				    script.meshes.push_back(c->read_line(true));
				    break;
			    case ModelScriptBinaryChunkType::ANIMATION: {
				    MdsAnimation anim {};
				    anim.name = c->read_line(false);
				    anim.layer = c->read_uint();
				    anim.next = c->read_line(false);
				    anim.blend_in = c->read_float();
				    anim.blend_out = c->read_float();
				    anim.flags = mds::animation_flags_from_string(c->read_line(false));
				    anim.model = c->read_line(false);
				    anim.direction =
				        c->read_line(false).find('R') == 0 ? AnimationDirection::BACKWARD : AnimationDirection::FORWARD;
				    anim.first_frame = c->read_int();
				    anim.last_frame = c->read_int();
				    anim.fps = c->read_float();
				    anim.speed = c->read_float();
				    anim.collision_volume_scale = c->read_float();
				    script.animations.push_back(std::move(anim));
				    ++ani_index;
				    break;
			    }
			    case ModelScriptBinaryChunkType::ANIMATION_ALIAS: {
				    MdsAnimationAlias alias {};
				    alias.name = c->read_line(false);
				    alias.layer = c->read_uint();
				    alias.next = c->read_line(false);
				    alias.blend_in = c->read_float();
				    alias.blend_out = c->read_float();
				    alias.flags = mds::animation_flags_from_string(c->read_line(false));
				    alias.alias = c->read_line(false);
				    alias.direction =
				        c->read_line(false).find('R') == 0 ? AnimationDirection::BACKWARD : AnimationDirection::FORWARD;
				    script.aliases.push_back(std::move(alias));
				    break;
			    }
			    case ModelScriptBinaryChunkType::ANIMATION_BLEND: {
				    MdsAnimationBlend blend {};
				    blend.name = c->read_line(false);
				    blend.next = c->read_line(false);
				    blend.blend_in = c->read_float();
				    blend.blend_out = c->read_float();
				    script.blends.push_back(std::move(blend));
				    break;
			    }
			    case ModelScriptBinaryChunkType::ANIMATION_COMBINE: {
				    MdsAnimationCombine combo {};
				    combo.name = c->read_line(false);
				    combo.layer = c->read_uint();
				    combo.next = c->read_line(false);
				    combo.blend_in = c->read_float();
				    combo.blend_out = c->read_float();
				    combo.flags = mds::animation_flags_from_string(c->read_line(false));
				    combo.model = c->read_line(false);
				    combo.last_frame = c->read_int();
				    script.combinations.push_back(std::move(combo));
				    break;
			    }
			    case ModelScriptBinaryChunkType::ANIMATION_DISABLE:
				    script.disabled_animations.push_back(c->read_line(false));
				    break;
			    case ModelScriptBinaryChunkType::EVENT_CAMERA_TREMOR: {
				    MdsCameraTremor trem {};
				    trem.frame = c->read_int();
				    trem.field1 = c->read_int();
				    trem.field2 = c->read_int();
				    trem.field3 = c->read_int();
				    trem.field4 = c->read_int();
				    script.animations[ani_index].tremors.push_back(trem);
				    break;
			    }
			    case ModelScriptBinaryChunkType::EVENT_SFX: {
				    MdsSoundEffect effect {};
				    effect.frame = c->read_int();
				    effect.name = c->read_line(false);
				    effect.range = c->read_float();
				    effect.empty_slot = c->read_uint() != 0;
				    script.animations[ani_index].sfx.push_back(std::move(effect));
				    break;
			    }
			    case ModelScriptBinaryChunkType::EVENT_SFX_GROUND: {
				    MdsSoundEffectGround effect {};
				    effect.frame = c->read_int();
				    effect.name = c->read_line(false);
				    effect.range = c->read_float();
				    effect.empty_slot = c->read_uint() != 0;
				    script.animations[ani_index].sfx_ground.push_back(std::move(effect));
				    break;
			    }
			    case ModelScriptBinaryChunkType::MODEL_TAG: {
				    MdsModelTag tag {};
				    (void) c->read_int();

				    auto event_type = c->read_line(false);
				    if (event_type != "DEF_HIT_LIMB" && event_type != "HIT_LIMB") {
					    ZKLOGW("ModelScript", "Unexpected type for modelTag: \"%s\"", event_type.c_str());
				    }

				    tag.bone = c->read_line(true);
				    script.model_tags.push_back(std::move(tag));
				    break;
			    }
			    case ModelScriptBinaryChunkType::EVENT_TAG: {
				    MdsEventTag event {};
				    event.frame = c->read_int();

				    auto event_type = c->read_line(false);
				    if (auto tp = event_types.find(event_type); tp == event_types.end()) {
					    event.type = MdsEventType::UNKNOWN;
					    ZKLOGW("ModelScript", "Unexpected value for MdsEventType: \"%s\"", event_type.c_str());
				    } else {
					    event.type = tp->second;
				    }

				    switch (event.type) {
				    case MdsEventType::ITEM_CREATE:
				    case MdsEventType::ITEM_EXCHANGE:
					    event.slot = c->read_line(true);
					    event.item = c->read_line(true);
					    break;
				    case MdsEventType::ITEM_INSERT:
				    case MdsEventType::MUNITION_PLACE:
					    event.slot = c->read_line(true);
					    break;
				    case MdsEventType::SET_FIGHT_MODE: {
					    if (auto mode = c->read_line(true); mode == "FIST") {
						    event.fight_mode = MdsFightMode::FIST;
					    } else if (mode == "1H" || mode == "1h") {
						    event.fight_mode = MdsFightMode::SINGLE_HANDED;
					    } else if (mode == "2H" || mode == "2h") {
						    event.fight_mode = MdsFightMode::DUAL_HANDED;
					    } else if (mode == "BOW") {
						    event.fight_mode = MdsFightMode::BOW;
					    } else if (mode == "CBOW") {
						    event.fight_mode = MdsFightMode::CROSSBOW;
					    } else if (mode == "MAG") {
						    event.fight_mode = MdsFightMode::MAGIC;
					    } else {
						    event.fight_mode = MdsFightMode::NONE;
					    }
					    break;
				    }
				    case MdsEventType::MESH_SWAP:
					    event.slot = c->read_line(true);
					    event.slot2 = c->read_line(true);
					    break;
				    case MdsEventType::HIT_LIMB:
					    event.slot = c->read_line(true); // TODO
					    break;
				    case MdsEventType::HIT_DIRECTION:
					    event.slot = c->read_line(true); // TODO
					    break;
				    case MdsEventType::SOUND_DRAW:
				    case MdsEventType::SOUND_UNDRAW:
				    case MdsEventType::MUNITION_REMOVE:
				    case MdsEventType::ITEM_DESTROY:
				    case MdsEventType::TORCH_INVENTORY:
				    case MdsEventType::ITEM_REMOVE:
					    event.slot = c->read_line(true); // TODO
					    break;
				    case MdsEventType::DAMAGE_MULTIPLIER:
				    case MdsEventType::PARRY_FRAME:
				    case MdsEventType::OPTIMAL_FRAME:
				    case MdsEventType::HIT_END:
				    case MdsEventType::COMBO_WINDOW: {
					    auto frames = c->read_line(true);
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
			    case ModelScriptBinaryChunkType::EVENT_PFX: {
				    MdsParticleEffect effect {};
				    effect.frame = c->read_int();
				    effect.index = c->read_int();
				    effect.name = c->read_line(false);
				    effect.position = c->read_line(false);
				    effect.attached = c->read_uint() != 0;
				    script.animations[ani_index].pfx.push_back(std::move(effect));
				    break;
			    }
			    case ModelScriptBinaryChunkType::EVENT_PFX_STOP: {
				    MdsParticleEffectStop effect {};
				    effect.frame = c->read_int();
				    effect.index = c->read_int();
				    script.animations[ani_index].pfx_stop.push_back(effect);
				    break;
			    }
			    case ModelScriptBinaryChunkType::EVENT_MM_ANI: {
				    MdsMorphAnimation anim {};
				    anim.frame = c->read_int();
				    anim.animation = c->read_line(false);
				    anim.node = c->read_line(false);
				    (void) c->read_float();
				    (void) c->read_float();
				    script.animations[ani_index].morph.push_back(std::move(anim));
				    break;
			    }
			    case ModelScriptBinaryChunkType::ROOT:
				    (void) c->read_uint();      // bool
				    (void) c->read_line(false); // path
				    break;
			    case ModelScriptBinaryChunkType::SOURCE: {
				    Date d {};
				    d.load(c);

				    (void) c->read_line(false); // path
				    break;
			    }
				    // case ModelScriptBinaryChunkType::model:
				    // case ModelScriptBinaryChunkType::animation_enum:
				    // case ModelScriptBinaryChunkType::animation_events_end:
				    // case ModelScriptBinaryChunkType::animation_enum_end:
				    // case ModelScriptBinaryChunkType::model_end:
			    case ModelScriptBinaryChunkType::END:
				    // empty
				    break;
			    case ModelScriptBinaryChunkType::ANIMATION_EVENTS:
				    (void) c->read_uint(); // bool
				    break;
			    case ModelScriptBinaryChunkType::UNKNOWN:
			    default:
				    break;
			    }
			    return false;
		    });
	}

	void ModelScript::load(Read* r) {
		auto potential_chunk_type = r->read_ushort();
		r->seek(-2, Whence::CUR);

		if (potential_chunk_type >= 0xF000 || potential_chunk_type == 0xD000) {
			this->load_binary(r);
			return;
		}

		this->load_source(r);
	}

	void ModelScript::load_binary(Read* r) {
		parse_binary_script(*this, r);
	}

	void ModelScript::load_source(Read* r) {
		MdsParser p {r};
		p.parse_script(*this);
	}

	bool operator&(AnimationFlags a, AnimationFlags b) {
		return static_cast<bool>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}

	AnimationFlags operator|(AnimationFlags a, AnimationFlags b) {
		return static_cast<AnimationFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	AnimationFlags& operator|=(AnimationFlags& a, AnimationFlags b) {
		a = a | b;
		return a;
	}
} // namespace zenkit
