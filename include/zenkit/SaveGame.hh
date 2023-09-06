// Copyright © 2022 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include <phoenix/buffer.hh>
#include <phoenix/texture.hh>

#include <filesystem>
#include <optional>
#include <string>

namespace phoenix::unstable {
	/// \brief Contains general information about a save-game.
	///
	/// The information contained within this struct is found in the `SAVEINFO.SAV` file within
	/// the save-game folder. It contains metadata about the save-game, like a title and the game
	/// version it was created with.
	struct save_info {
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
		/// \throws parser_error if parsing fails.
		/// \note Instead of calling this directly, you probably want to parse a whole save-game.
		///       Use save_game::parse for that instead.
		PHOENIX_API static save_info parse(buffer&& buf);
	};

	/// \brief The section log entries are in.
	enum class topic_section : std::uint32_t {
		quests = 0x00,
		infos = 0x01,
	};

	/// \brief The status of a single log entry.
	enum class topic_status : std::uint32_t {
		active = 0x01,
		completed = 0x02,
		failed = 0x03,
		obsolete = 0x04,
	};

	struct log_topic {
		std::string description;
		topic_section section;
		topic_status status;
		std::vector<std::string> entries;
	};

	struct info_state {
		std::string name;
		bool told;
	};

	struct symbol_state {
		std::string name;
		std::vector<std::uint32_t> values;
	};

	struct script_state {
		std::int32_t day;
		std::int32_t hour;
		std::int32_t minute;

		std::vector<info_state> infos;
		std::vector<symbol_state> symbols;
		std::vector<log_topic> log;
		std::uint8_t guild_attitudes[42][42];

		/// \brief Parses a save-game script state structure from the data in the given buffer.
		/// \param buf The buffer to read from.
		/// \param g2 Set to true if a Gothic II save-game is being parsed.
		/// \return The parsed save-game script state structure.
		/// \throws parser_error if parsing fails.
		/// \note Instead of calling this directly, you probably want to parse a whole save-game.
		///       Use save_game::parse for that instead.
		PHOENIX_INTERNAL static script_state parse(buffer&& buf, bool g2);
	};

	struct save_game {
	public:
		/// \brief Parses a save-game from the data in the given directory.
		/// \param path The path of the save-game folder.
		/// \return The parsed save-game.
		/// \throws parser_error if parsing fails.
		PHOENIX_API static save_game parse(const std::filesystem::path& path);

		/// \brief Opens the saved world file with the given world name as a buffer and returns it.
		///
		/// To open the world the player was in when creating the save you can use
		/// `sav.open_world_save(sav.current_world)`.
		///
		/// \param world_name The name of the world to open, including the `.ZEN` extension.
		/// \return A buffer containing the world's data or std::nullopt if the world is not present in this save.
		PHOENIX_API std::optional<buffer> open_world_save(std::string_view world_name) const;

	public:
		/// \brief Contains metadata about the save-game, like its name and version numbers.
		save_info metadata;

		/// \brief The name of the world file the player is currently in.
		std::string current_world;

		/// \brief Script state data.
		///
		/// Contains the states of relevant script symbols, the state of the quest log,
		/// as well as the state of each piece of in-game information and the attitudes
		/// of guilds towards each other.
		script_state script;

		/// \brief The thumbnail image of the save-game. Not present if `THUMB.SAV` is missing from the save-game.
		std::optional<texture> thumbnail;

	private:
		std::filesystem::path _m_root_path;
	};
} // namespace phoenix::unstable
