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

	/// \brief Contains general information about a save-game.
	///
	/// The information contained within this struct is found in the `SAVEINFO.SAV` file within
	/// the save-game folder. It contains metadata about the save-game, like a title and the game
	/// version it was created with.
	struct SaveMetadata : Object {
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
		std::vector<int32_t> values;
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

	struct SaveState {
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

	class SaveGame {
	public:
		explicit SaveGame(GameVersion version);

		void load(std::filesystem::path const& path);
		void save(std::filesystem::path const& path, World& world, std::string world_name);

		std::shared_ptr<World> load_world() const;
		std::shared_ptr<World> load_world(std::string name) const;

	public:
		SaveMetadata metadata;
		SaveState state;
		std::optional<Texture> thumbnail;

	private:
		GameVersion _m_version;
		std::filesystem::path _m_path;
	};
} // namespace zenkit
