// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Stream.hh"
#include "zenkit/Texture.hh"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;
}

namespace zenkit::unstable {
	/// \brief Contains general information about a save-game.
	///
	/// The information contained within this struct is found in the `SAVEINFO.SAV` file within
	/// the save-game folder. It contains metadata about the save-game, like a title and the game
	/// version it was created with.
	struct SaveInfo {
		std::string title;
		std::string world;
		std::int32_t time_day;
		std::int32_t time_hour;
		std::int32_t time_minute;
		std::string save_date;
		std::int32_t version_major;
		std::int32_t version_minor;
		std::int32_t play_time_seconds;
		std::int32_t version_point;
		std::int32_t version_int;
		std::string version_app_name;

		/// \brief Parses a save-game info structure from the data in the given buffer.
		/// \param buf The buffer to read from.
		/// \return The parsed save-game info structure.
		/// \throws ParserError if parsing fails.
		/// \note Instead of calling this directly, you probably want to parse a whole save-game.
		///       Use save_game::parse for that instead.
		ZKREM("use ::load()") ZKAPI static SaveInfo parse(phoenix::buffer&& buf);

		ZKAPI void load(Read* r);
	};

	/// \brief The section log entries are in.
	enum class SaveTopicSection : std::uint32_t {
		QUESTS = 0x00,
		INFOS = 0x01,
	};

	/// \brief The status of a single log entry.
	enum class SaveTopicStatus : std::uint32_t {
		ACTIVE = 0x01,
		COMPLETED = 0x02,
		FAILED = 0x03,
		OBSOLETE = 0x04,
	};

	struct SaveLogTopic {
		std::string description;
		SaveTopicSection section;
		SaveTopicStatus status;
		std::vector<std::string> entries;
	};

	struct SaveInfoState {
		std::string name;
		bool told;
	};

	struct SaveSymbolState {
		std::string name;
		std::vector<std::uint32_t> values;
	};

	struct SaveScriptState {
		std::int32_t day;
		std::int32_t hour;
		std::int32_t minute;

		std::vector<SaveInfoState> infos;
		std::vector<SaveSymbolState> symbols;
		std::vector<SaveLogTopic> log;
		std::uint8_t guild_attitudes[42][42];

		ZKINT void load(Read* r, bool g2);
	};

	struct SaveGame {
	public:
		/// \brief Parses a save-game from the data in the given directory.
		/// \param path The path of the save-game folder.
		/// \return The parsed save-game.
		/// \throws ParserError if parsing fails.
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static SaveGame parse(std::filesystem::path const& path);

		/// \brief Opens the saved world file with the given world name as a buffer and returns it.
		///
		/// To open the world the player was in when creating the save you can use
		/// `sav.open_world_save(sav.current_world)`.
		///
		/// \param world_name The name of the world to open, including the `.ZEN` extension.
		/// \return A buffer containing the world's data or std::nullopt if the world is not present in this save.
		[[nodiscard]] ZKREM("use ::load_world()") ZKAPI std::optional<phoenix::buffer> open_world_save(
		    std::string_view world_name) const;

		[[nodiscard]] ZKAPI std::optional<std::unique_ptr<Read>> open_world(std::string_view world_name) const;

		/// \brief Parses a save-game from the data in the given directory.
		/// \param path The path of the save-game folder.
		/// \return The parsed save-game.
		/// \throws ParserError if parsing fails.
		ZKAPI void load(std::filesystem::path const& path);

	public:
		/// \brief Contains metadata about the save-game, like its name and version numbers.
		SaveInfo metadata;

		/// \brief The name of the world file the player is currently in.
		std::string current_world;

		/// \brief Script state data.
		///
		/// Contains the states of relevant script symbols, the state of the quest log,
		/// as well as the state of each piece of in-game information and the attitudes
		/// of guilds towards each other.
		SaveScriptState script;

		/// \brief The thumbnail image of the save-game. Not present if `THUMB.SAV` is missing from the save-game.
		std::optional<Texture> thumbnail;

	private:
		std::filesystem::path _m_root_path;
	};
} // namespace zenkit::unstable
