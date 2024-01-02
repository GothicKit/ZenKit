// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "Object.hh"
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
	class World;
	class Read;
} // namespace zenkit

namespace zenkit::unstable {
	/// \brief Contains general information about a save-game.
	///
	/// The information contained within this struct is found in the `SAVEINFO.SAV` file within
	/// the save-game folder. It contains metadata about the save-game, like a title and the game
	/// version it was created with.
	struct SaveInfo : Object {
		ZK_OBJECT(ObjectType::oCSavegameInfo);

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

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
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

	struct Mission {
		std::string name;
		int id;
		bool av;
		int status_index;
	};

	enum class CutscenePoolItemRunBehavior : uint32_t {
		ALWAYS,
		TIMES,
		PER_HOUR,
	};

	struct CutscenePoolItem : Object {
		ZK_OBJECT(ObjectType::zCCSPoolItem);

		std::string name;
		CutscenePoolItemRunBehavior run_behavior;
		int run_behavior_value;
		int times_played;
		int deactivated;
		int flags;

		void load(ReadArchive& r, GameVersion version) override;
		void save(WriteArchive& w, GameVersion version) const override;
	};

	struct CutsceneManager : Object {
		ZK_OBJECT(ObjectType::oCCSManager);

		std::vector<std::shared_ptr<CutscenePoolItem>> pool_items;

		void load(ReadArchive& r, GameVersion version) override;
		void save(WriteArchive& w, GameVersion version) const override;
	};

	struct SaveScriptState {
		std::int32_t day;
		std::int32_t hour;
		std::int32_t minute;

		std::vector<Mission> missions;
		std::shared_ptr<CutsceneManager> cutscene_manager;

		std::vector<SaveInfoState> infos;
		std::vector<SaveSymbolState> symbols;
		std::vector<SaveLogTopic> log;
		std::uint8_t guild_attitudes[42][42];

		ZKINT void load(ReadArchive& r, GameVersion version);
		ZKINT void save(WriteArchive& w, GameVersion version) const;
	};

	struct SaveGame {
	public:
		[[nodiscard]] ZKAPI std::shared_ptr<World> open_world(std::string_view world_name) const;

		/// \brief Parses a save-game from the data in the given directory.
		/// \param path The path of the save-game folder.
		/// \return The parsed save-game.
		/// \throws ParserError if parsing fails.
		ZKAPI void load(std::filesystem::path const& path, GameVersion version);

	public:
		/// \brief Contains metadata about the save-game, like its name and version numbers.
		std::shared_ptr<SaveInfo> metadata;

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

		GameVersion version;

	private:
		std::filesystem::path _m_root_path;
	};
} // namespace zenkit::unstable
