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

	void VDoor::parse(VDoor& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VDoor::load(ReadArchive& r, GameVersion version) {
		VInteractiveObject::load(r, version);
		this->locked = r.read_bool();        // locked
		this->key = r.read_string();         // keyInstance
		this->pick_string = r.read_string(); // pickLockStr
	}

	void VFire::parse(VFire& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void VFire::load(ReadArchive& r, GameVersion version) {
		VInteractiveObject::load(r, version);
		this->slot = r.read_string();     // fireSlot
		this->vob_tree = r.read_string(); // fireVobtreeName
	}
} // namespace zenkit
