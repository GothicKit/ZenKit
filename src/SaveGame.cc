// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/SaveGame.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

#include <set>

namespace zenkit::unstable {
	SaveInfo SaveInfo::parse(phoenix::buffer&& buf) {
		SaveInfo info;

		auto read = Read::from(&buf);
		info.load(read.get());

		return info;
	}

	void SaveInfo::load(Read* r) {
		auto archive = ReadArchive::from(r);

		ArchiveObject hdr;
		if (!archive->read_object_begin(hdr) || hdr.class_name != "oCSavegameInfo") {
			throw zenkit::ParserError {"SaveInfo", "expected oCSavegameInfo object not found"};
		}

		this->title = archive->read_string();          // Title
		this->world = archive->read_string();          // WorldName
		this->time_day = archive->read_int();          // TimeDay
		this->time_hour = archive->read_int();         // TimeHour
		this->time_minute = archive->read_int();       // TimeMin
		this->save_date = archive->read_string();      // SaveDate
		this->version_major = archive->read_int();     // VersionMajor
		this->version_minor = archive->read_int();     // VersionMinor
		this->play_time_seconds = archive->read_int(); // PlayTimeSeconds

		if (!archive->read_object_end()) {
			// Gothic II stores more information.
			this->version_point = archive->read_int();       // VersionPoint
			this->version_int = archive->read_int();         // VersionInt
			this->version_app_name = archive->read_string(); // VersionAppName

			if (!archive->read_object_end()) {
				ZKLOGW("SaveInfo", "\"%s\" not fully parsed", hdr.class_name.c_str());
			}
		}
	}

	void SaveScriptState::load(zenkit::Read* r, bool g2) {
		auto ar = ReadArchive::from(r);

		this->day = ar->read_int();    // day
		this->hour = ar->read_int();   // hour
		this->minute = ar->read_int(); // min

		auto entry_count = ar->read_int(); // NumOfEntries
		this->infos.resize(entry_count);

		for (auto i = 0; i < entry_count; ++i) {
			this->infos[i].told = ar->read_bool();   // Told
			this->infos[i].name = ar->read_string(); // InstName
		}

		ar->read_int();                    // NumOfEntries
		auto topic_count = ar->read_int(); // LOGMANAGERTOPICCOUNT
		this->log.resize(topic_count);

		for (auto i = 0; i < topic_count; ++i) {
			auto& topic = this->log[i];
			topic.description = ar->read_string();                          // TOPICDESCRIPTION
			topic.section = static_cast<SaveTopicSection>(ar->read_enum()); // TOPICSECTION
			topic.status = static_cast<SaveTopicStatus>(ar->read_enum());   // TOPICSTATUS
			topic.entries.resize(ar->read_int());                           // LOGTOPICENTRYCOUNT

			(void) ar->read_int(); // LOGMANAGERENTRYCOUNT

			for (auto& entrie : topic.entries) {
				entrie = ar->read_string(); // ENTRYDESCRIPTION
			}
		}

		ArchiveObject obj;
		if (!ar->read_object_begin(obj) || obj.class_name != "oCCSManager:zCCSManager") {
			throw zenkit::ParserError {"SaveScriptState", "expected oCCSManager:zCCSManager object not found"};
		}

		ar->read_int(); // poolCount

		if (!ar->read_object_end()) {
			ZKLOGW("SaveScriptState", "\"%s\" not fully parsed", obj.class_name.c_str());
			ar->skip_object(true);
		}

		auto symbol_count = ar->read_int(); // numSymbols
		this->symbols.resize(symbol_count);

		for (auto i = 0; i < symbol_count; ++i) {
			SaveSymbolState sym;
			sym.name = ar->read_string(); // symName0

			// For Gothic II saves, there is additional data stored
			if (g2) {
				auto value_count = ar->read_int(); // symName0cnt

				for (auto j = 0; j < value_count; ++j) {
					sym.values.push_back(ar->read_int()); // symValue0_0
				}
			} else {
				sym.values.push_back(ar->read_int()); // symValue0
			}
		}

		// A two-dimensional array of the form int[42][42] containing guild attitudes
		auto raw = ar->read_raw(42 * 42); // guildTable

		for (auto& guild_attitude : this->guild_attitudes) {
			for (unsigned char& j : guild_attitude) {
				j = raw->read_ubyte();
			}
		}
	}

	std::optional<std::filesystem::path> find_file_matching(const std::set<std::filesystem::path>& choices,
	                                                        std::string_view filename) {
		auto result = std::find_if(choices.begin(), choices.end(), [filename](const std::filesystem::path& path) {
			return phoenix::iequals(path.filename().string(), filename);
		});

		if (result == choices.end()) {
			return std::nullopt;
		}

		return *result;
	}

	SaveGame SaveGame::parse(const std::filesystem::path& path) {
		SaveGame sav;
		sav.load(path);
		return sav;
	}

	std::optional<phoenix::buffer> SaveGame::open_world_save(std::string_view world_name) const {
		auto path = _m_root_path / world_name;
		path.replace_extension("SAV");

		if (!std::filesystem::exists(path))
			return std::nullopt;
		return phoenix::buffer::mmap(path);
	}

	std::optional<std::unique_ptr<Read>> SaveGame::open_world(std::string_view world_name) const {
		auto path = _m_root_path / world_name;
		path.replace_extension("SAV");

		if (!std::filesystem::exists(path))
			return std::nullopt;
		return Read::from(path);
	}

	void SaveGame::load(std::filesystem::path const& path) {
		this->_m_root_path = path;

		if (!std::filesystem::is_directory(path)) {
			throw zenkit::ParserError {"SaveGame", "save game path does not exist or is not a directory"};
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
				throw zenkit::ParserError {"SaveGame",
				                           "expected SAVEINFO.SAV not found. this is probably not a Gothic savegame"};
			}

			auto r = Read::from(*file_save_info);
			this->metadata.load(r.get());
			this->current_world = this->metadata.world + ".ZEN";
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
				throw zenkit::ParserError {"SaveGame",
				                           "expected SAVEDAT.SAV not found. this is probably not a Gothic savegame"};
			}

			auto r = Read::from(*file_save_dat);
			this->script.load(r.get(), !this->metadata.version_app_name.empty());
		}
	}
} // namespace zenkit::unstable
