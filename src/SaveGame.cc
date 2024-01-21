// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/SaveGame.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"
#include "zenkit/World.hh"

#include "Internal.hh"

#include <set>

namespace zenkit {
	void SaveMetadata::load(ReadArchive& r, GameVersion version) {
		this->title = r.read_string();          // Title
		this->world = r.read_string();          // WorldName
		this->time_day = r.read_int();          // TimeDay
		this->time_hour = r.read_int();         // TimeHour
		this->time_minute = r.read_int();       // TimeMin
		this->save_date = r.read_string();      // SaveDate
		this->version_major = r.read_int();     // VersionMajor
		this->version_minor = r.read_int();     // VersionMinor
		this->play_time_seconds = r.read_int(); // PlayTimeSeconds

		if (version == GameVersion::GOTHIC_2) {
			this->version_point = r.read_int();       // VersionPoint
			this->version_int = r.read_int();         // VersionInt
			this->version_app_name = r.read_string(); // VersionAppName
		}
	}

	void SaveMetadata::save(WriteArchive& w, GameVersion version) const {
		w.write_string("Title", this->title);
		w.write_string("WorldName", this->world);
		w.write_int("TimeDay", this->time_day);
		w.write_int("TimeHour", this->time_hour);
		w.write_int("TimeMin", this->time_minute);
		w.write_string("SaveDate", this->save_date);
		w.write_int("VersionMajor", this->version_major);
		w.write_int("VersionMinor", this->version_minor);
		w.write_int("PlayTimeSeconds", this->play_time_seconds);

		if (version == GameVersion::GOTHIC_2) {
			w.write_int("VersionPoint", this->version_point);
			w.write_int("VersionInt", this->version_int);
			w.write_string("VersionAppName", this->version_app_name);
		}
	}

	void CutscenePoolItem::load(ReadArchive& r, GameVersion) {
		this->name = r.read_string();                                                 // itemName
		this->run_behavior = static_cast<CutscenePoolItemRunBehavior>(r.read_enum()); // runBehaviour
		this->run_behavior_value = r.read_int();                                      // runBehaviourValue
		this->times_played = r.read_int();                                            // numPlayed
		this->deactivated = r.read_int();                                             // deactivated
		this->flags = r.read_int();                                                   // flags
	}

	void CutscenePoolItem::save(WriteArchive& w, GameVersion) const {
		w.write_string("itemName", this->name);
		w.write_enum("runBehaviour", static_cast<std::uint32_t>(this->run_behavior));
		w.write_int("runBehaviourValue", this->run_behavior_value);
		w.write_int("numPlayed", this->times_played);
		w.write_int("deactivated", this->deactivated);
		w.write_int("flags", this->flags);
	}

	void CutsceneManager::load(ReadArchive& r, GameVersion version) {
		auto pool_count = r.read_int(); // poolCount
		for (auto i = 0; i < pool_count; ++i) {
			this->pool_items.push_back(r.read_object<CutscenePoolItem>(version));
		}
	}

	void CutsceneManager::save(WriteArchive& w, GameVersion version) const {
		w.write_int("poolCount", this->pool_items.size());
		for (auto& item : this->pool_items) {
			w.write_object(item, version);
		}
	}

	void SaveState::load(ReadArchive& r, GameVersion version) {
		this->day = r.read_int();    // day
		this->hour = r.read_int();   // hour
		this->minute = r.read_int(); // min

		// oCInfoManager {
		auto entry_count = r.read_int(); // NumOfEntries
		this->infos.resize(entry_count);
		for (auto i = 0; i < entry_count; ++i) {
			// oCInfo {
			this->infos[i].told = r.read_bool();   // Told
			this->infos[i].name = r.read_string(); // InstName
			                                       // }
		}
		// }

		// oCMissionManager {
		auto mission_count = r.read_int(); // NumOfEntries
		for (auto i = 0; i < mission_count; ++i) {
			// oCMission {
			auto& mission = this->missions.emplace_back();
			mission.name = r.read_string(); // name
			mission.id = r.read_int();      // ID
			mission.av = r.read_bool();     // Av

			auto unknown_count = r.read_int(); // NumInList
			for (auto j = 0; j < unknown_count; ++j) {
				(void) r.read_int();   // VobIDL
				(void) r.read_int();   // StatusL
				(void) r.read_float(); // startTimeL
			}

			mission.status_index = r.read_int(); // StatusIndex
			                                     // }
		}
		// }

		// oCLogManager {
		auto topic_count = r.read_int(); // LOGMANAGERTOPICCOUNT
		this->log.resize(topic_count);

		for (auto i = 0; i < topic_count; ++i) {
			// oCLogTopic {
			auto& topic = this->log[i];
			topic.description = r.read_string();                          // TOPICDESCRIPTION
			topic.section = static_cast<SaveTopicSection>(r.read_enum()); // TOPICSECTION
			topic.status = static_cast<SaveTopicStatus>(r.read_enum());   // TOPICSTATUS
			topic.entries.resize(r.read_int());                           // LOGTOPICENTRYCOUNT

			// NOTE(lmichaelis): Always the same as `LOGTOPICENTRYCOUNT`.
			(void) r.read_int(); // LOGMANAGERENTRYCOUNT

			for (auto& entrie : topic.entries) {
				entrie = r.read_string(); // ENTRYDESCRIPTION
			}
			// }
		}
		// }

		this->cutscene_manager = r.read_object<CutsceneManager>(version);

		// zCParser {
		auto symbol_count = r.read_int(); // numSymbols
		this->symbols.resize(symbol_count);

		for (SaveSymbolState& sym : this->symbols) {
			sym.name = r.read_string(); // symName0

			// For Gothic II saves, there is additional data stored
			if (version == GameVersion::GOTHIC_1) {
				sym.values.push_back(r.read_int()); // symValue0
			} else {
				auto value_count = r.read_int(); // symName0cnt
				for (auto j = 0; j < value_count; ++j) {
					sym.values.push_back(r.read_int()); // symValue0_0
				}
			}
		}
		// }

		// A two-dimensional array of the form int[42][42] containing guild attitudes
		auto raw = r.read_raw(42 * 42); // guildTable

		for (auto& guild_attitude : this->guild_attitudes) {
			for (unsigned char& j : guild_attitude) {
				j = raw->read_ubyte();
			}
		}
	}

	void SaveState::save(WriteArchive& w, GameVersion version) const {
		w.write_int("day", this->day);
		w.write_int("hour", this->hour);
		w.write_int("min", this->minute);

		// oCInfoManager {
		w.write_int("NumOfEntries", this->infos.size());
		for (auto& info : this->infos) {
			// oCInfo {
			w.write_bool("Told", info.told);
			w.write_string("InstName", info.name);
			// }
		}
		// }

		// oCMissionManager {
		w.write_int("NumOfEntries", this->missions.size());
		for (auto const& mission : this->missions) {
			// oCMission {
			w.write_string("name", mission.name);
			w.write_int("ID", mission.id);
			w.write_bool("Av", mission.av);
			w.write_int("NumInList", 0); // TODO: Unknown!
			w.write_int("StatusIndex", mission.status_index);
			// }
		}
		// }

		// oCLogManager {
		w.write_int("LOGMANAGERTOPICCOUNT", this->log.size());
		for (auto& topic : this->log) {
			// oCLogTopic {
			w.write_string("TOPICDESCRIPTION", topic.description);
			w.write_enum("TOPICSECTION", static_cast<std::uint32_t>(topic.section));
			w.write_enum("TOPICSTATUS", static_cast<std::uint32_t>(topic.status));
			w.write_int("LOGTOPICENTRYCOUNT", topic.entries.size());
			w.write_int("LOGMANAGERENTRYCOUNT", topic.entries.size());

			for (auto& entry : topic.entries) {
				w.write_string("ENTRYDESCRIPTION", entry);
			}
			// }
		}
		// }

		w.write_object(this->cutscene_manager, version);

		// zCParser {
		w.write_int("numSymbols", this->symbols.size());
		for (auto i = 0u; i < this->symbols.size(); ++i) {
			auto& sym = this->symbols[i];
			auto iStr = std::to_string(i);
			w.write_string("symName" + iStr, sym.name);

			// For Gothic II saves, there is additional data stored
			if (version == GameVersion::GOTHIC_1) {
				w.write_int("symValue" + iStr, sym.values[0]);
			} else {
				w.write_int("symName" + iStr + "cnt", sym.values.size());
				for (auto v : sym.values) {
					w.write_int("symValue" + iStr + "_0", v);
				}
			}
		}
		// }

		// A two-dimensional array of the form int[42][42] containing guild attitudes
		std::vector<std::byte> guild_table(42 * 42);
		auto wrgt = Write::to(&guild_table);

		for (auto& guild_attitude : this->guild_attitudes) {
			for (auto j : guild_attitude) {
				wrgt->write_ubyte(j);
			}
		}

		w.write_raw("guildTable", guild_table);
	}

	SaveGame::SaveGame(GameVersion version) : _m_version(version) {}

	std::optional<std::filesystem::path> find_file_matching(std::set<std::filesystem::path> const& choices,
	                                                        std::string_view filename) {
		auto result = std::find_if(choices.begin(), choices.end(), [filename](std::filesystem::path const& path) {
			return phoenix::iequals(path.filename().string(), filename);
		});

		if (result == choices.end()) {
			return std::nullopt;
		}

		return *result;
	}

	std::shared_ptr<World> SaveGame::load_world(std::string world_name) const {
		auto path = _m_path / world_name;
		path.replace_extension("SAV");

		if (!std::filesystem::exists(path)) return nullptr;

		auto r = Read::from(path);
		auto ar = ReadArchive::from(r.get());
		return ar->read_object<World>(_m_version);
	}

	void SaveGame::load(std::filesystem::path const& path) {
		this->_m_path = path;

		if (!std::filesystem::is_directory(path)) {
			throw ParserError {"SaveGame", "save game path does not exist or is not a directory"};
		}

		std::set<std::filesystem::path> entries {};
		for (auto& file : std::filesystem::directory_iterator(path)) {
			entries.emplace(file.path());
		}

		// Load SAVEINFO.SAV
		{
			ZKLOGI("SaveGame", "Loading SAVEINFO.SAV");
			auto file_save_info = find_file_matching(entries, "SAVEINFO.SAV");
			if (!file_save_info) {
				throw ParserError {"SaveGame",
				                   "expected SAVEINFO.SAV not found. this is probably not a Gothic savegame"};
			}

			auto r = Read::from(*file_save_info);
			auto ar = ReadArchive::from(r.get());
			this->metadata = *ar->read_object<SaveMetadata>(_m_version);
		}

		// Load THUMB.SAV
		{
			ZKLOGI("SaveGame", "Loading THUMB.SAV");
			auto file_thumb = find_file_matching(entries, "THUMB.SAV");
			if (file_thumb) {
				auto r = Read::from(*file_thumb);

				this->thumbnail.emplace();
				this->thumbnail->load(r.get());
			}
		}

		// Load SAVEDAT.SAV
		{
			ZKLOGI("SaveGame", "Loading SAVEDAT.SAV");
			auto file_save_dat = find_file_matching(entries, "SAVEDAT.SAV");
			if (!file_save_dat) {
				throw ParserError {"SaveGame",
				                   "expected SAVEDAT.SAV not found. this is probably not a Gothic savegame"};
			}

			auto r = Read::from(*file_save_dat);
			auto ar = ReadArchive::from(r.get());
			this->state.load(*ar, _m_version);
		}
	}

	void SaveGame::save(std::filesystem::path const& path, World& world, std::string world_name) {
		// Copy over the current save if necessary.
		if (_m_path != path) {
			if (std::filesystem::exists(path)) {
				std::filesystem::remove_all(path);
			}

			if (!_m_path.empty() && std::filesystem::exists(_m_path)) {
				std::filesystem::copy(_m_path, path, std::filesystem::copy_options::recursive);
			} else {
				std::filesystem::create_directories(path);
			}
		}

		{
			auto w = Write::to(path / "SAVEINFO.SAV");
			auto ar = WriteArchive::to(w.get(), ArchiveFormat::ASCII);

			ar->write_object("%", &this->metadata, _m_version);
			ar->write_header();
		}

		{
			auto w = Write::to(path / "THUMB.SAV");

			if (this->thumbnail) {
				this->thumbnail->save(w.get());
			}
		}

		{
			auto w = Write::to(path / "SAVEHDR.SAV");
			w->write_string(world_name);
			w->write_string(".ZEN\n");
		}

		{
			auto w = Write::to(path / "SAVEDAT.SAV");
			auto ar = WriteArchive::to_save(w.get(), ArchiveFormat::ASCII);

			this->state.save(*ar, _m_version);
			ar->write_header();
		}

		{
			auto w = Write::to(path / (world_name + ".SAV"));
			auto ar = WriteArchive::to_save(w.get(), ArchiveFormat::BINARY);

			ar->write_object("%", &world, _m_version);
			ar->write_header();
		}

		_m_path = path;
	}

	std::shared_ptr<World> SaveGame::load_world() const {
		return load_world(metadata.world + ".ZEN");
	}
} // namespace zenkit
