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

	struct VMovableObject : VirtualObject {
		std::string name;
		std::int32_t hp;
		std::int32_t damage;
		bool movable;
		bool takable;
		bool focus_override;
		SoundMaterialType material;
		std::string visual_destroyed;
		std::string owner;
		std::string owner_guild;
		bool destroyed;

		/// \brief Parses an interactive VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		ZKREM("use ::load()") ZKAPI static void parse(VMovableObject& obj, ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	struct VInteractiveObject : VMovableObject {
		std::int32_t state;
		std::string target;
		std::string item;
		std::string condition_function;
		std::string on_state_change_function;
		bool rewind;

		/// \brief Parses a interactive VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		ZKREM("use ::load()")
		ZKAPI static void parse(VInteractiveObject& obj, ReadArchive& ctx, GameVersion version);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb representing a campfire.
	struct VFire : VInteractiveObject {
		std::string slot;
		std::string vob_tree;

		/// \brief Parses a campfire VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		/// \see mob::parse
		ZKREM("use ::load()") ZKAPI static void parse(VFire& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb representing a container.
	struct VContainer : VInteractiveObject {
		bool locked;
		std::string key;
		std::string pick_string;
		std::string contents;

		// Save-game only variables
		std::vector<std::shared_ptr<VItem>> s_items;

		/// \brief Parses a container VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		/// \see mob::parse
		/// \see mob_container::parse
		ZKREM("use ::load()") ZKAPI static void parse(VContainer& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	/// \brief A VOb representing a door.
	struct VDoor : VInteractiveObject {
		bool locked;
		std::string key;
		std::string pick_string;

		/// \brief Parses a door VOb the given *ZenGin* archive.
		/// \param[out] obj The object to read.
		/// \param[in,out] ctx The archive reader to read from.
		/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
		/// \throws ParserError if parsing fails.
		/// \see vob::parse
		/// \see mob::parse
		/// \see mob_container::parse
		ZKREM("use ::load") ZKAPI static void parse(VDoor& obj, ReadArchive& ctx, GameVersion version);
		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};

	struct VLadder : VInteractiveObject {};
	struct VSwitch : VInteractiveObject {};
	struct VWheel : VInteractiveObject {};

	struct VBed : VInteractiveObject {
		[[nodiscard]] ZKAPI uint16_t get_version_identifier(GameVersion game) const override;
	};
} // namespace zenkit
