// Copyright © 2022 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>
#include <phoenix/save_game.hh>

#include <optional>
#include <set>

namespace phoenix::unstable {
	save_info save_info::parse(buffer&& buf) {
		save_info info;
		auto archive = archive_reader::open(buf);

		archive_object hdr;
		if (!archive->read_object_begin(hdr) || hdr.class_name != "oCSavegameInfo") {
			throw parser_error {"save_info", "expected oCSavegameInfo object not found"};
		}

		info.title = archive->read_string();          // Title
		info.world = archive->read_string();          // WorldName
		info.time_day = archive->read_int();          // TimeDay
		info.time_hour = archive->read_int();         // TimeHour
		info.time_minute = archive->read_int();       // TimeMin
		info.save_date = archive->read_string();      // SaveDate
		info.version_major = archive->read_int();     // VersionMajor
		info.version_minor = archive->read_int();     // VersionMinor
		info.play_time_seconds = archive->read_int(); // PlayTimeSeconds

		if (!archive->read_object_end()) {
			// Gothic II stores more information.
			info.version_point = archive->read_int();       // VersionPoint
			info.version_int = archive->read_int();         // VersionInt
			info.version_app_name = archive->read_string(); // VersionAppName
		}

		if (!archive->read_object_end()) {
			PX_LOGW("save_info: {} not fully parsed", hdr.class_name);
		}

		return info;
	}

	script_state script_state::parse(buffer&& buf, bool g2) {
		script_state sav;
		auto ar = archive_reader::open(buf);

		sav.day = ar->read_int();    // day
		sav.hour = ar->read_int();   // hour
		sav.minute = ar->read_int(); // min

		auto entry_count = ar->read_int(); // NumOfEntries
		sav.infos.resize(entry_count);

		for (auto i = 0; i < entry_count; ++i) {
			sav.infos[i].told = ar->read_bool();   // Told
			sav.infos[i].name = ar->read_string(); // InstName
		}

		ar->read_int();                    // NumOfEntries
		auto topic_count = ar->read_int(); // LOGMANAGERTOPICCOUNT
		sav.log.resize(topic_count);

		for (auto i = 0; i < topic_count; ++i) {
			auto& topic = sav.log[i];
			topic.description = ar->read_string();                       // TOPICDESCRIPTION
			topic.section = static_cast<topic_section>(ar->read_enum()); // TOPICSECTION
			topic.status = static_cast<topic_status>(ar->read_enum());   // TOPICSTATUS
			topic.entries.resize(ar->read_int());                        // LOGTOPICENTRYCOUNT

			(void) ar->read_int(); // LOGMANAGERENTRYCOUNT

			for (auto j = 0u; j < topic.entries.size(); ++j) {
				topic.entries[j] = ar->read_string(); // ENTRYDESCRIPTION
			}
		}

		archive_object obj;
		if (!ar->read_object_begin(obj) || obj.class_name != "oCCSManager:zCCSManager") {
			throw parser_error {"save_info", "expected oCCSManager:zCCSManager object not found"};
		}

		ar->read_int(); // poolCount

		if (!ar->read_object_end()) {
			PX_LOGE("save_game: {} not fully parsed", obj.class_name);
			ar->skip_object(true);
		}

		auto symbol_count = ar->read_int(); // numSymbols
		sav.symbols.resize(symbol_count);

		for (auto i = 0; i < symbol_count; ++i) {
			symbol_state sym;
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
		auto raw = ar->read_raw_bytes(42 * 42); // guildTable

		for (int i = 0; i < 42; ++i) {
			for (int j = 0; j < 42; ++j) {
				sav.guild_attitudes[i][j] = raw.get();
			}
		}

		if (buf.remaining() > 0) {
			PX_LOGE("save_game: not fully parsed");
		}

		return sav;
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

	save_game save_game::parse(const std::filesystem::path& path) {
		save_game sav;
		sav._m_root_path = path;

		if (!std::filesystem::is_directory(path)) {
			throw parser_error {"save_game", "save game path does not exist or is not a directory"};
		}

		std::set<std::filesystem::path> entries {};
		for (auto& file : std::filesystem::directory_iterator(path)) {
			entries.emplace(file.path());
		}

		// Load SAVEINFO.SAV
		{
			PX_LOGI("save_game: loading SAVEINFO.SAV");
			auto file_save_info = find_file_matching(entries, "SAVEINFO.SAV");
			if (!file_save_info) {
				throw parser_error {"save_game",
				                    "expected SAVEINFO.SAV not found. this is probably not a Gothic savegame"};
			}

			sav.metadata = save_info::parse(buffer::mmap(*file_save_info));
			sav.current_world = sav.metadata.world + ".ZEN";
		}

		// Load THUMB.SAV
		{
			PX_LOGI("save_game: loading THUMB.SAV");
			auto file_thumb = find_file_matching(entries, "THUMB.SAV");
			if (file_thumb) {
				sav.thumbnail = texture::parse(buffer::mmap(*file_thumb));
			}
		}

		// Load SAVEDAT.SAV
		{
			PX_LOGI("save_game: loading SAVEDAT.SAV");
			auto file_save_dat = find_file_matching(entries, "SAVEDAT.SAV");
			if (!file_save_dat) {
				throw parser_error {"save_game",
				                    "expected SAVEDAT.SAV not found. this is probably not a Gothic savegame"};
			}

			sav.script = script_state::parse(buffer::mmap(*file_save_dat), !sav.metadata.version_app_name.empty());
		}

		return sav;
	}

	std::optional<buffer> save_game::open_world_save(std::string_view world_name) const {
		auto path = _m_root_path / world_name;
		path.replace_extension("SAV");

		if (!std::filesystem::exists(path))
			return std::nullopt;
		return buffer::mmap(path);
	}
} // namespace phoenix::unstable