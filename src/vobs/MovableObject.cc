// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/MovableObject.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

namespace zenkit {
	void VMovableObject::parse(VMovableObject& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VMovableObject::load(ReadArchive& r, GameVersion version) {
		VirtualObject::load(r, version);
		this->name = r.read_string();                                   // focusName
		this->hp = r.read_int();                                        // hitpoints
		this->damage = r.read_int();                                    // damage
		this->movable = r.read_bool();                                  // moveable
		this->takable = r.read_bool();                                  // takeable
		this->focus_override = r.read_bool();                           // focusOverride
		this->material = static_cast<SoundMaterialType>(r.read_enum()); // soundMaterial
		this->visual_destroyed = r.read_string();                       // visualDestroyed
		this->owner = r.read_string();                                  // owner
		this->owner_guild = r.read_string();                            // ownerGuild
		this->destroyed = r.read_bool();                                // isDestroyed
	}

	void VMovableObject::save(WriteArchive& w, GameVersion version) const {
		VirtualObject::save(w, version);
		w.write_string("focusName", name);
		w.write_int("hitpoints", hp);
		w.write_int("damage", damage);
		w.write_bool("moveable", movable);
		w.write_bool("takeable", takable);
		w.write_bool("focusOverride", focus_override);
		w.write_enum("soundMaterial", static_cast<std::uint32_t>(material));
		w.write_string("visualDestroyed", visual_destroyed);
		w.write_string("owner", owner);
		w.write_string("ownerGuild", owner_guild);
		w.write_bool("isDestroyed", destroyed);
	}

	uint16_t VMovableObject::get_version_identifier(GameVersion) const {
		return 47105;
	}

	void VInteractiveObject::parse(VInteractiveObject& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VInteractiveObject::load(ReadArchive& r, GameVersion version) {
		VMovableObject::load(r, version);
		this->state = r.read_int();                       // stateNum
		this->target = r.read_string();                   // triggerTarget
		this->item = r.read_string();                     // useWithItem
		this->condition_function = r.read_string();       // conditionFunc
		this->on_state_change_function = r.read_string(); // onStateFunc
		this->rewind = r.read_bool();                     // rewind
	}

	void VInteractiveObject::save(WriteArchive& w, GameVersion version) const {
		VMovableObject::save(w, version);
		w.write_int("stateNum", state);
		w.write_string("triggerTarget", target);
		w.write_string("useWithItem", item);
		w.write_string("conditionFunc", condition_function);
		w.write_string("onStateFunc", on_state_change_function);
		w.write_bool("rewind", rewind);
	}

	uint16_t VInteractiveObject::get_version_identifier(GameVersion) const {
		return 35585;
	}

	void VContainer::parse(VContainer& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VContainer::load(ReadArchive& r, GameVersion version) {
		VInteractiveObject::load(r, version);
		this->locked = r.read_bool();        // locked
		this->key = r.read_string();         // keyInstance
		this->pick_string = r.read_string(); // pickLockStr
		this->contents = r.read_string();    // contains

		if (r.is_save_game()) {
			// In save-games, containers contain extra variables
			auto item_count = static_cast<size_t>(r.read_int()); // NumOfEntries
			this->s_items.resize(item_count);

			for (auto i = 0u; i < item_count; ++i) {
				this->s_items[i] = r.read_object<VItem>(version);
			}
		}
	}

	void VContainer::save(WriteArchive& w, GameVersion version) const {
		VInteractiveObject::save(w, version);
		w.write_bool("locked", locked);
		w.write_string("keyInstance", key);
		w.write_string("pickLockStr", pick_string);
		w.write_string("contains", contents);

		if (w.is_save_game()) {
			// In save-games, containers contain extra variables
			w.write_int("NumOfEntries", s_items.size());

			for (auto& it : this->s_items) {
				w.write_object(it, version);
			}
		}
	}

	uint16_t VContainer::get_version_identifier(GameVersion) const {
		return 64513;
	}

	void VDoor::parse(VDoor& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VDoor::load(ReadArchive& r, GameVersion version) {
		VInteractiveObject::load(r, version);
		this->locked = r.read_bool();        // locked
		this->key = r.read_string();         // keyInstance
		this->pick_string = r.read_string(); // pickLockStr
	}

	void VDoor::save(WriteArchive& w, GameVersion version) const {
		VInteractiveObject::save(w, version);
		w.write_bool("locked", this->locked);
		w.write_string("keyInstance", this->key);
		w.write_string("pickLockStr", this->pick_string);
	}

	uint16_t VDoor::get_version_identifier(GameVersion) const {
		return 64513;
	}

	uint16_t VBed::get_version_identifier(GameVersion) const {
		return 61440;
	}

	void VFire::parse(VFire& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VFire::load(ReadArchive& r, GameVersion version) {
		VInteractiveObject::load(r, version);
		this->slot = r.read_string();     // fireSlot
		this->vob_tree = r.read_string(); // fireVobtreeName
	}

	void VFire::save(WriteArchive& w, GameVersion version) const {
		VInteractiveObject::save(w, version);
		w.write_string("fireSlot", slot);
		w.write_string("fireVobtreeName", vob_tree);
	}

	uint16_t VFire::get_version_identifier(GameVersion) const {
		return 18433;
	}
} // namespace zenkit
