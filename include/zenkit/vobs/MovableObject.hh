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

	namespace vobs {
		struct MovableObject : public VirtualObject {
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
			ZKREM("use ::load()") ZKAPI static void parse(MovableObject& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		struct InteractiveObject : public MovableObject {
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
			ZKAPI static void parse(InteractiveObject& obj, ReadArchive& ctx, GameVersion version);

			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb representing a campfire.
		struct Fire : public InteractiveObject {
			std::string slot;
			std::string vob_tree;

			/// \brief Parses a campfire VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			ZKREM("use ::load()") ZKAPI static void parse(Fire& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb representing a container.
		struct Container : public InteractiveObject {
			bool locked;
			std::string key;
			std::string pick_string;
			std::string contents;

			// Save-game only variables
			std::vector<std::unique_ptr<Item>> s_items;

			/// \brief Parses a container VOb the given *ZenGin* archive.
			/// \param[out] obj The object to read.
			/// \param[in,out] ctx The archive reader to read from.
			/// \note After this function returns the position of \p ctx will be at the end of the parsed object.
			/// \throws ParserError if parsing fails.
			/// \see vob::parse
			/// \see mob::parse
			/// \see mob_container::parse
			ZKREM("use ::load()") ZKAPI static void parse(Container& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};

		/// \brief A VOb representing a door.
		struct Door : public InteractiveObject {
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
			ZKREM("use ::load") ZKAPI static void parse(Door& obj, ReadArchive& ctx, GameVersion version);
			ZKAPI void load(ReadArchive& r, GameVersion version) override;
		};
	} // namespace vobs
} // namespace zenkit
