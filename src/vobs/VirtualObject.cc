// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/VirtualObject.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

#include <unordered_map>

namespace zenkit {
	/// \brief A mapping of archive class names to visual_type values.
	static std::unordered_map<std::string, VisualType> visual_type_map = {
	    {"zCDecal", VisualType::DECAL},
	    {"zCMesh", VisualType::MESH},
	    {"zCProgMeshProto", VisualType::MULTI_RESOLUTION_MESH},
	    {"zCParticleFX", VisualType::PARTICLE_EFFECT},
	    {"zCModel", VisualType::MODEL},
	    {"zCAICamera", VisualType::AI_CAMERA},
	    {"zCMorphMesh", VisualType::MORPH_MESH},
	    {"\xA7", VisualType::UNKNOWN},
	    {"%", VisualType::UNKNOWN},
	};

	Decal Decal::parse(ReadArchive& in, GameVersion version) {
		Decal dc {};
		dc.load(in, version);
		return dc;
	}

	void Decal::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
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

	void VirtualObject::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
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

		ArchiveObject hdr {};
		if (has_visual_object) {
			ArchiveObject visual {};
			r.read_object_begin(visual);
			this->associated_visual_type = visual_type_map[visual.class_name];

			if (this->associated_visual_type == VisualType::DECAL) {
				this->visual_decal = Decal {};
				this->visual_decal->load(r, version);
			}

			if (!r.read_object_end()) {
				ZKLOGW("VirtualObject", "visual \"%s\" not fully parsed", visual.class_name.c_str());
				r.skip_object(true);
			}
		}

		// TODO
		if (has_ai_object && r.read_object_begin(hdr)) {
			if (hdr.class_name == "oCAIHuman:oCAniCtrl_Human:zCAIPlayer") {
				(void) r.read_int();   // waterLevel
				(void) r.read_float(); // floorY
				(void) r.read_float(); // waterY
				(void) r.read_float(); // ceilY
				(void) r.read_float(); // feetY
				(void) r.read_float(); // headY
				(void) r.read_float(); // fallDistY
				(void) r.read_float(); // fallStartY

				// TODO: aiNpc
				if (r.read_object_begin(hdr)) {
					r.skip_object(true);
				} else {
					ZKLOGW("VirtualObject.Ai", "aiNpc not found");
				}

				(void) r.read_int();  // walkMode
				(void) r.read_int();  // weaponMode
				(void) r.read_int();  // wmodeLast
				(void) r.read_int();  // wmodeSelect
				(void) r.read_bool(); // changeWeapon
				(void) r.read_int();  // actionMode
			} else if (hdr.class_name == "oCAIVobMove") {
				// TODO: vob
				if (r.read_object_begin(hdr)) {
					r.skip_object(true);
				}

				// TODO: owner
				if (r.read_object_begin(hdr)) {
					r.skip_object(true);
				}
			}

			if (!r.read_object_end()) {
				ZKLOGW("VirtualObject.Ai", "\"%s\" not fully parsed.", hdr.class_name.c_str());
				r.skip_object(true);
			}
		}

		if (has_event_manager_object && r.read_object_begin(hdr)) {
			if (hdr.class_name != "zCEventManager") {
				throw ParserError {"VirtualObject"};
			}

			EventManager em {};
			em.load(r, version);

			r.read_object_end();
		}

		if (r.get_header().save) {
			// save-games contain two extra values for each VOb
			// TODO: These are technically from oCVob!
			this->saved = SaveState {};
			this->saved->sleep_mode = r.read_byte();     // sleepMode
			this->saved->next_on_timer = r.read_float(); // nextOnTimer

			if (this->physics_enabled) {
				this->saved->rigid_body.emplace().load(r, version);
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

	void EventManager::load(ReadArchive& r, GameVersion) {
		this->cleared = r.read_bool();
		this->active = r.read_bool();

		ArchiveObject obj;
		if (r.read_object_begin(obj) && !r.read_object_end()) {
			ZKLOGW("VirtualObject.EventManager", "emCutscene not fully parsed!");
			r.skip_object(true);
		}

		// TODO: this->cutscene.load(r, version);
	}
} // namespace zenkit
