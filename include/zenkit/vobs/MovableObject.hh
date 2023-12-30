// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Misc.hh"
#include "zenkit/vobs/Misc.hh"
#include "zenkit/vobs/VirtualObject.hh"

#include <cstdint>
#include <string>

namespace zenkit {
	class ReadArchive;

	/// \brief The different sounds a material can make.
	/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#soundMaterial
	enum class SoundMaterialType : std::uint32_t {
		WOOD = 0,
		STONE = 1,
		METAL = 2,
		LEATHER = 3,
		CLAY = 4,
		GLASS = 5,

		// Deprecated entries.
		wood ZKREM("renamed to SoundMaterialType::WOOD") = WOOD,
		stone ZKREM("renamed to SoundMaterialType::STONE") = STONE,
		metal ZKREM("renamed to SoundMaterialType::METAL") = METAL,
		leather ZKREM("renamed to SoundMaterialType::LEATHER") = LEATHER,
		clay ZKREM("renamed to SoundMaterialType::CLAY") = CLAY,
		glass ZKREM("renamed to SoundMaterialType::GLASS") = GLASS,
	};

	/// \brief A VObject which can optionally be moved and/or carried.
	/// \see https://zk.gothickit.dev/engine/objects/oCMOB/
	struct VMovableObject : VirtualObject {
		ZK_OBJECT(ObjectType::oCMOB);

		/// \brief The name of the object as seen in-game when focusing it.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#focusName
		std::string name;

		/// \brief The number of hitpoints of the object.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#hitpoints
		std::int32_t hp;

		/// \brief The damage dealt by the object when being used.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#damage
		std::int32_t damage;

		/// \brief Whether the object can be moved by the player.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#moveable
		bool movable;

		/// \brief Whether the object can be carried by the player.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#takeable
		bool takable;

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#focusOverride
		bool focus_override;

		/// \brief The sound to play when the object is hit.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#soundMaterial
		SoundMaterialType material;

		/// \brief The name of the visual to use when the object is destroyed.
		///
		/// Behaves like Visual#name.
		///
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#visualDestroyed
		std::string visual_destroyed;

		/// \brief The name of the script instance of the NPC who is the owner of this object.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#owner
		std::string owner;

		/// \brief The name of the guild this object belongs to.
		///
		/// This name corresponds to the script constant for each guild (i.e. `GIL_BAU`), found in `Constants.d`.
		///
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#ownerGuild
		std::string owner_guild;

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/oCMOB/#isDestroyed
		bool destroyed;

		ZKREM("use ::load()") ZKAPI static void parse(VMovableObject& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief Interactive objects can be interacted with by NPCs and the player.
	///
	/// Every time the object is interacted with, an `OnTrigger` event is sent to the #target object.
	///
	/// \see https://zk.gothickit.dev/engine/objects/oCMobInter/
	struct VInteractiveObject : VMovableObject {
		ZK_OBJECT(ObjectType::oCMobInter);

		/// \brief Unknown.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobInter/#stateNum
		std::int32_t state;

		/// \brief The name of the VObject(s) to send an `OnTrigger` event to when this object is interacted with.
		///
		/// Controlled by the #rewind property.
		///
		/// \see https://zk.gothickit.dev/engine/objects/oCMobInter/#triggerTarget
		std::string target;

		/// \brief  The name of the item which the player or NPC must have in their inventory in order to
		//          interact with the object. Corresponds to the name of the item instance in the scripts.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobInter/#useWithItem
		std::string item;

		/// \brief The name of a script function which, when called, determines whether the object can be interacted
		/// with.
		///
		/// The script function returns an int which is either `0`, if the object should be disabled and `1` if it
		/// should be enabled.
		///
		/// \see https://zk.gothickit.dev/engine/objects/oCMobInter/#conditionFunc
		std::string condition_function;

		/// \brief The name of a script function to be called when the object is being used.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobInter/#onStateFunc
		std::string on_state_change_function;

		/// \brief Determines whether an `OnTrigger` event should be sent every time the object is interacted
		///        with or only the first time.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobInter/#rewind
		bool rewind;

		ZKREM("use ::load()") ZKAPI static void parse(VInteractiveObject& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief An object with an integrated fire effect. Only supports rigged models as visuals.
	/// \see https://zk.gothickit.dev/engine/objects/oCMobFire/
	struct VFire : VInteractiveObject {
		ZK_OBJECT(ObjectType::oCMobFire);

		/// \brief The bone of the rigged model to place the fire effect at.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobFire/#fireSlot
		std::string slot;

		/// \brief The name of the template file which contains the configuration of the fire effect.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobFire/#fireVobtreeName
		std::string vob_tree;

		ZKREM("use ::load()") ZKAPI static void parse(VFire& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief An object which contains items.
	/// \see https://zk.gothickit.dev/engine/objects/oCMobContainer/
	struct VContainer : VInteractiveObject {
		ZK_OBJECT(ObjectType::oCMobContainer);

		/// \brief Determines whether the container is locked.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobContainer/#locked
		bool locked;

		/// \brief The name of the item script instance which unlocks the container.
		/// \note Only relevant if the container is locked.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobContainer/#keyInstance
		std::string key;

		/// \brief The combination which unlocks this container when picking the lock.
		///
		/// Each character of the string is either `R` or `L` where `R` stands for "Right" and `L` stands for "Left".
		/// If empty, the lock can not be picked. Example: `LRRLLRL`,
		///
		/// \note Only relevant if the container is locked.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobContainer/#pickLockStr
		std::string pick_string;

		/// \brief The items found inside the container as a comma-separated list.
		///
		/// Each element of the list starts with the name of the item script instance and is optionally followed by
		/// a colon and a number, indicating the number of that item to be found inside.
		/// Example: `ItMi_Gold:75, ItFo_Fish:2, ItMi_Quartz`.
		///
		/// \see https://zk.gothickit.dev/engine/objects/oCMobContainer/#contents
		std::string contents;

		// Save-game only variables
		std::vector<std::shared_ptr<VItem>> s_items;

		ZKREM("use ::load()") ZKAPI static void parse(VContainer& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb representing a door.
	/// \see https://zk.gothickit.dev/engine/objects/oCMobDoor/
	struct VDoor : VInteractiveObject {
		ZK_OBJECT(ObjectType::oCMobDoor);

		/// \brief Determines whether the container is locked.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobDoor/#locked
		bool locked;

		/// \brief The name of the item script instance which unlocks the container.
		/// \note Only relevant if the container is locked.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobDoor/#keyInstance
		std::string key;

		/// \brief The combination which unlocks this container when picking the lock.
		///
		/// Each character of the string is either `R` or `L` where `R` stands for "Right" and `L` stands for "Left".
		/// If empty, the lock can not be picked. Example: `LRRLLRL`,
		///
		/// \note Only relevant if the container is locked.
		/// \see https://zk.gothickit.dev/engine/objects/oCMobDoor/#pickLockStr
		std::string pick_string;

		ZKREM("use ::load") ZKAPI static void parse(VDoor& obj, ReadArchive& ctx, GameVersion version);

		/// \brief Load this object from the given archive.
		/// \param r The archive to read from;
		/// \param version The version of the game the object was made for.
		ZKAPI void load(ReadArchive& r, GameVersion version) override;

		/// \brief Save this object to the given archive.
		/// \param w The archive to save to.
		/// \param version The version of the game to save for.
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VObject representing a ladder.
	/// \see https://zk.gothickit.dev/engine/objects/oCMobLadder/
	struct VLadder : VInteractiveObject {
		ZK_OBJECT(ObjectType::oCMobLadder);
	};

	/// \brief A VObject representing a switch with two states.
	/// \see https://zk.gothickit.dev/engine/objects/oCMobSwitch/
	struct VSwitch : VInteractiveObject {
		ZK_OBJECT(ObjectType::oCMobSwitch);
	};

	/// \brief A wheel with four different states.
	/// \see https://zk.gothickit.dev/engine/objects/oCMobWheel/
	struct VWheel : VInteractiveObject {
		ZK_OBJECT(ObjectType::oCMobWheel);
	};

	struct VBed : VInteractiveObject {
		ZK_OBJECT(ObjectType::oCMobBed);

		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};
} // namespace zenkit
