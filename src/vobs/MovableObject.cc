// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/vobs/MovableObject.hh"
#include "zenkit/Archive.hh"

#include "../Internal.hh"

namespace zenkit::vobs {
	void MovableObject::parse(MovableObject& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void MovableObject::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
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

	void InteractiveObject::parse(InteractiveObject& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void InteractiveObject::load(ReadArchive& r, zenkit::GameVersion version) {
		MovableObject::load(r, version);
		this->state = r.read_int();                       // stateNum
		this->target = r.read_string();                   // triggerTarget
		this->item = r.read_string();                     // useWithItem
		this->condition_function = r.read_string();       // conditionFunc
		this->on_state_change_function = r.read_string(); // onStateFunc
		this->rewind = r.read_bool();                     // rewind
	}

	void Container::parse(Container& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Container::load(ReadArchive& r, zenkit::GameVersion version) {
		InteractiveObject::load(r, version);
		this->locked = r.read_bool();        // locked
		this->key = r.read_string();         // keyInstance
		this->pick_string = r.read_string(); // pickLockStr
		this->contents = r.read_string();    // contains

		if (r.is_save_game()) {
			// In save-games, containers contain extra variables
			auto item_count = r.read_int(); // NumOfEntries
			this->s_items.resize(item_count);

			ArchiveObject itm;
			for (auto i = 0; i < item_count; ++i) {
				if (!r.read_object_begin(itm) || itm.class_name != "oCItem:zCVob") {
					throw zenkit::ParserError {"VOb.Container"};
				}

				this->s_items[i] = std::make_unique<Item>();
				this->s_items[i]->load(r, version);

				if (!r.read_object_end()) {
					ZKLOGW("VOb.Container", "oCItem:zCVob object not fully parsed");
					r.skip_object(true);
				}
			}
		}
	}

	void Door::parse(Door& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Door::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		InteractiveObject::load(r, version);
		this->locked = r.read_bool();        // locked
		this->key = r.read_string();         // keyInstance
		this->pick_string = r.read_string(); // pickLockStr
	}

	void Fire::parse(Fire& obj, ReadArchive& r, GameVersion version) {
		obj.load(r, version);
	}

	void Fire::load(zenkit::ReadArchive& r, zenkit::GameVersion version) {
		InteractiveObject::load(r, version);
		this->slot = r.read_string();     // fireSlot
		this->vob_tree = r.read_string(); // fireVobtreeName
	}
} // namespace zenkit::vobs
