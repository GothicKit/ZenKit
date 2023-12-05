// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/VirtualObject.hh"
#include "zenkit/Archive.hh"
#include "zenkit/vobs/Misc.hh"

#include "../Internal.hh"

#include <unordered_map>

namespace zenkit {
	/// \brief A mapping of archive class names to visual_type values.
	static std::unordered_map<ObjectType, VisualType> visual_type_map = {
	    {ObjectType::zCDecal, VisualType::DECAL},
	    {ObjectType::zCMesh, VisualType::MESH},
	    {ObjectType::zCProgMeshProto, VisualType::MULTI_RESOLUTION_MESH},
	    {ObjectType::zCParticleFX, VisualType::PARTICLE_EFFECT},
	    {ObjectType::zCModel, VisualType::MODEL},
	    {ObjectType::zCAICamera, VisualType::AI_CAMERA},
	    {ObjectType::zCMorphMesh, VisualType::MORPH_MESH},
	};

	VisualDecal VisualDecal::parse(ReadArchive& in, GameVersion version) {
		VisualDecal dc {};
		dc.load(in, version);
		return dc;
	}

	void VisualDecal::load(ReadArchive& r, GameVersion version) {
		this->name = r.read_string();                                 // name
		this->dimension = r.read_vec2();                              // decalDim
		this->offset = r.read_vec2();                                 // decalOffset
		this->two_sided = r.read_bool();                              // decal2Sided
		this->alpha_func = static_cast<AlphaFunction>(r.read_enum()); // decalAlphaFunc
		this->texture_anim_fps = r.read_float();                      // decalTexAniFPS

		if (version == GameVersion::GOTHIC_2) {
			this->alpha_weight = r.read_byte();    // decalAlphaWeight
			this->ignore_daylight = r.read_bool(); // ignoreDayLight
		}
	}

	void VirtualObject::parse(VirtualObject& obj, ReadArchive& in, GameVersion version) {
		obj.load(in, version);
	}

	void VirtualObject::load(ReadArchive& r, GameVersion version) {
		auto packed = r.read_int() != 0; // pack
		bool has_visual_object = true;
		bool has_ai_object = true;
		bool has_event_manager_object = false;

		if (packed) {
			auto bin = r.read_raw(version == GameVersion::GOTHIC_1 ? 74 : 83); // dataRaw

			this->bbox.load(bin.get());
			this->position = bin->read_vec3();
			this->rotation = bin->read_mat3();

			std::uint8_t bit0 = bin->read_ubyte();
			std::uint16_t bit1;

			if (version == GameVersion::GOTHIC_1) {
				bit1 = bin->read_ubyte();
			} else {
				bit1 = bin->read_ushort();
			}

			this->show_visual = static_cast<bool>((bit0 & 0b00000001) >> 0);
			this->sprite_camera_facing_mode = static_cast<SpriteAlignment>((bit0 & 0b00000110) >> 1);
			this->cd_static = static_cast<bool>((bit0 & 0b00001000) >> 3);
			this->cd_dynamic = static_cast<bool>((bit0 & 0b00010000) >> 4);
			this->vob_static = static_cast<bool>((bit0 & 0b00100000) >> 5);
			this->dynamic_shadows = static_cast<ShadowType>((bit0 & 0b11000000) >> 6);

			bool has_preset_name = static_cast<bool>((bit1 & 0b000000000000001u) >> 0u);
			bool has_vob_name = static_cast<bool>((bit1 & 0b000000000000010u) >> 1u);
			bool has_visual_name = static_cast<bool>((bit1 & 0b000000000000100u) >> 2u);
			has_visual_object = static_cast<bool>((bit1 & 0b000000000001000u) >> 3u);
			has_ai_object = static_cast<bool>((bit1 & 0b000000000010000u) >> 4u);

			// Quirk: bit 5 of this bitfield specifies whether an event manger object is
			// present but this is only relevant in save-games.
			has_event_manager_object = static_cast<bool>((bit1 & 0b000000000100000u) >> 5u) && r.get_header().save;

			this->physics_enabled = static_cast<bool>((bit1 & 0b000000001000000u) >> 6u);

			if (version == GameVersion::GOTHIC_2) {
				this->anim_mode = static_cast<AnimationType>(bit1 & 0b000000110000000u >> 7u);
				this->bias = static_cast<std::int32_t>((bit1 & 0b011111000000000u) >> 9u);
				this->ambient = static_cast<bool>((bit1 & 0b100000000000000u) >> 14u);

				this->anim_strength = bin->read_float();
				this->far_clip_scale = bin->read_float();
			}

			if (has_preset_name) {
				this->preset_name = r.read_string(); // presetName
			}

			if (has_vob_name) {
				this->vob_name = r.read_string(); // vobName
			}

			if (has_visual_name) {
				this->visual_name = r.read_string(); // visual
			}
		} else {
			this->preset_name = r.read_string();
			this->bbox = r.read_bbox(); // bbox3DWS

			this->rotation = r.read_mat3x3(); // trafoOSToWSRot
			this->position = r.read_vec3();   // trafoOSToWSPos

			this->vob_name = r.read_string();                                              // vobName
			this->visual_name = r.read_string();                                           // visual
			this->show_visual = r.read_bool();                                             // showVisual
			this->sprite_camera_facing_mode = static_cast<SpriteAlignment>(r.read_enum()); // visualCamAlign

			if (version == GameVersion::GOTHIC_1) {
				this->cd_static = r.read_bool();                                // cdStatic
				this->cd_dynamic = r.read_bool();                               // cdDyn
				this->vob_static = r.read_bool();                               // staticVob
				this->dynamic_shadows = static_cast<ShadowType>(r.read_enum()); // dynShadow
			} else {
				this->anim_mode = static_cast<AnimationType>(r.read_enum());    // visualAniMode
				this->anim_strength = r.read_float();                           // visualAniModeStrength
				this->far_clip_scale = r.read_float();                          // vobFarClipZScale
				this->cd_static = r.read_bool();                                // cdStatic
				this->cd_dynamic = r.read_bool();                               // cdDyn
				this->vob_static = r.read_bool();                               // staticVob
				this->dynamic_shadows = static_cast<ShadowType>(r.read_enum()); // dynShadow
				this->bias = r.read_int();                                      // zbias
				this->ambient = r.read_bool();                                  // isAmbient
			}
		}

		if (has_visual_object) {
			this->visual = std::dynamic_pointer_cast<Visual>(r.read_object(version));

			if (visual != nullptr) {
				auto it = visual_type_map.find(visual->get_object_type());
				if (it == visual_type_map.end()) {
					this->associated_visual_type = VisualType::UNKNOWN;
				} else {
					this->associated_visual_type = it->second;
				}

				this->visual->name = this->visual_name;
				this->visual->type = it->second;

				if (this->visual->type == VisualType::DECAL) {
					this->visual_decal.emplace(*reinterpret_cast<VisualDecal*>(this->visual.get()));
				}
			}
		}

		if (has_ai_object) {
			// NOTE(lmichaelis): The NDK does not seem to support `reinterpret_pointer_cast`.
			auto obj = r.read_object(version);
			this->ai = std::shared_ptr<Ai>(obj, reinterpret_cast<Ai*>(obj.get()));
		}

		if (has_event_manager_object) {
			(void) /* em = */ r.read_object<EventManager>(version);
		}

		if (r.get_header().save) {
			// save-games contain two extra values for each VOb
			// TODO: These are technically from oCVob!
			this->sleep_mode = r.read_byte();     // sleepMode
			this->next_on_timer = r.read_float(); // nextOnTimer

			if (this->physics_enabled) {
				this->rigid_body.emplace().load(r, version);
			}
		}
	}

	void RigidBody::load(ReadArchive& r, GameVersion) {
		this->vel = r.read_vec3();
		this->mode = r.read_byte();
		this->gravity_enabled = r.read_bool();
		this->gravity_scale = r.read_float();
		this->slide_direction = r.read_vec3();
	}

	void EventManager::load(ReadArchive& r, GameVersion version) {
		this->cleared = r.read_bool();
		this->active = r.read_bool();

		(void) /* TODO: emCutscene = */ r.read_object(version);
	}

	void AiHuman::load(ReadArchive& r, GameVersion version) {
		water_level = r.read_int();    // waterLevel
		floor_y = r.read_float();      // floorY
		water_y = r.read_float();      // waterY
		ceil_y = r.read_float();       // ceilY
		feet_y = r.read_float();       // feetY
		head_y = r.read_float();       // headY
		fall_dist_y = r.read_float();  // fallDistY
		fall_start_y = r.read_float(); // fallStartY

		npc = r.read_object<VNpc>(version); // aiNpc

		walk_mode = r.read_int();      // walkMode
		weapon_mode = r.read_int();    // weaponMode
		wmode_ast = r.read_int();      // wmodeLast
		wmode_select = r.read_int();   // wmodeSelect
		change_weapon = r.read_bool(); // changeWeapon
		action_mode = r.read_int();    // actionMode
	}

	void AiMove::load(ReadArchive& r, GameVersion version) {
		// NOTE(lmichaelis): The NDK does not seem to support `reinterpret_pointer_cast`.
		auto obj = r.read_object(version);
		vob = std::shared_ptr<VirtualObject>(obj, reinterpret_cast<VirtualObject*>(obj.get())); // vob
		owner = r.read_object<VNpc>(version);                                                   // owner
	}
} // namespace zenkit
