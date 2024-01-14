// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "ArchiveBinsafe.hh"

#include "phoenix/buffer.hh"

#include "../Internal.hh"

#include <glm/gtc/type_ptr.hpp>

#include <charconv>
#include <cstring>

namespace zenkit {
	void ReadArchiveBinsafe::read_header() {
		_m_bs_version = read->read_uint();
		_m_object_count = read->read_uint();
		auto hash_table_offset = read->read_uint();

		{
			auto mark = read->tell();
			read->seek(hash_table_offset, Whence::BEG);

			auto hash_table_size = read->read_uint();
			_m_hash_table_entries.resize(hash_table_size);

			for (std::uint32_t i = 0; i < hash_table_size; ++i) {
				auto key_length = read->read_ushort();
				auto insertion_index = read->read_ushort();
				auto hash_value = read->read_uint();
				auto key = read->read_string(key_length);

				_m_hash_table_entries[insertion_index] = hash_table_entry {key, hash_value};
			}

			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
		}
	}

	bool ReadArchiveBinsafe::read_object_begin(ArchiveObject& obj) {
		if (read->eof()) return false;

		auto mark = read->tell();
		if (static_cast<ArchiveEntryType>(read->read_ubyte()) != ArchiveEntryType::STRING) {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		auto line = read->read_string(read->read_ushort());

		// Fail quickly if we know this can't be an object begin
		if (line.length() <= 2) {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		char* it = line.data();
		if (*it++ != '[') {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		auto* object_name = strtok(it, " ]");
		auto* class_name = strtok(nullptr, " ]");
		auto* version = strtok(nullptr, " ]");
		auto* index = strtok(nullptr, " ]");

		if (object_name == nullptr || class_name == nullptr || version == nullptr || index == nullptr) {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		obj.version = static_cast<uint16_t>(atoi(version));
		obj.index = static_cast<uint32_t>(atoi(index));
		obj.object_name = object_name;
		obj.class_name = class_name;
		return true;
	}

	bool ReadArchiveBinsafe::read_object_end() {
		if (read->eof()) return true;

		auto mark = read->tell();
		if (static_cast<ArchiveEntryType>(read->read_ubyte()) != ArchiveEntryType::STRING) {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		if (read->read_ushort() != 2) {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		if (read->read_string(2) != "[]") {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		return true;
	}

	std::string const& ReadArchiveBinsafe::get_entry_key() {
		if (static_cast<ArchiveEntryType>(read->read_ubyte()) != ArchiveEntryType::HASH) {
			throw zenkit::ParserError {"ReadArchive.Binsafe", "invalid format"};
		}

		auto hash = read->read_uint();
		return _m_hash_table_entries[hash].key;
	}

	std::string ReadArchiveBinsafe::read_string() {
		return read->read_string(ensure_entry_meta<ArchiveEntryType::STRING>());
	}

	std::int32_t ReadArchiveBinsafe::read_int() {
		ensure_entry_meta<ArchiveEntryType::INTEGER>();
		return read->read_int();
	}

	float ReadArchiveBinsafe::read_float() {
		ensure_entry_meta<ArchiveEntryType::FLOAT>();
		return read->read_float();
	}

	std::uint8_t ReadArchiveBinsafe::read_byte() {
		ensure_entry_meta<ArchiveEntryType::BYTE>();
		return read->read_ubyte();
	}

	std::uint16_t ReadArchiveBinsafe::read_word() {
		ensure_entry_meta<ArchiveEntryType::WORD>();
		return read->read_ushort();
	}

	std::uint32_t ReadArchiveBinsafe::read_enum() {
		ensure_entry_meta<ArchiveEntryType::ENUM>();
		return read->read_uint();
	}

	bool ReadArchiveBinsafe::read_bool() {
		ensure_entry_meta<ArchiveEntryType::BOOL>();
		return read->read_uint() != 0;
	}

	glm::u8vec4 ReadArchiveBinsafe::read_color() {
		ensure_entry_meta<ArchiveEntryType::COLOR>();

		auto b = read->read_ubyte();
		auto g = read->read_ubyte();
		auto r = read->read_ubyte();
		auto a = read->read_ubyte();

		return {r, g, b, a};
	}

	glm::vec3 ReadArchiveBinsafe::read_vec3() {
		ensure_entry_meta<ArchiveEntryType::VEC3>();
		return read->read_vec3();
	}

	glm::vec2 ReadArchiveBinsafe::read_vec2() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta<ArchiveEntryType::RAW_FLOAT>() - 2 * sizeof(float));

		if (unused < 0) {
			throw ParserError {"ReadArchive.Binsafe"
			                   "cannot read vec2 (2 * float): not enough space in rawFloat entry."};
		}

		auto c = read->read_vec2();

		// There might be more bytes in this. We'll ignore them.
		read->seek(unused, Whence::CUR);
		return c;
	}

	AxisAlignedBoundingBox ReadArchiveBinsafe::read_bbox() {
		auto unused =
		    static_cast<std::int32_t>(ensure_entry_meta<ArchiveEntryType::RAW_FLOAT>() - 3 * 2 * sizeof(float));

		if (unused < 0) {
			throw ParserError {"ReadArchive.Binsafe",
			                   "cannot read bbox (6 * float): not enough space in rawFloat entry."};
		}

		AxisAlignedBoundingBox aabb {};
		aabb.load(read);

		// There might be more bytes in this. We'll ignore them.
		read->seek(unused, Whence::CUR);
		return aabb;
	}

	glm::mat3x3 ReadArchiveBinsafe::read_mat3x3() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta<ArchiveEntryType::RAW>() - 3 * 3 * sizeof(float));

		if (unused < 0) {
			throw ParserError("ReadArchive.Binsafe: cannot read mat3x3 (9 * float): not enough space in raw entry.");
		}

		auto v = read->read_mat3();

		// There might be more bytes in this. We'll ignore them.
		read->seek(unused, Whence::CUR);
		return v;
	}

	phoenix::buffer ReadArchiveBinsafe::read_raw_bytes(uint32_t size) {
		auto length = ensure_entry_meta<ArchiveEntryType::RAW>();

		if (length < size) {
			throw ParserError {"ReadArchive.Binsafe", "not enough raw bytes to read!"};
		} else if (length > size) {
			ZKLOGW("ReadArchive.Binsafe", "Reading %d bytes although %d are actually available", size, length);
		}

		std::vector bytes(length, std::byte {});
		read->read(bytes.data(), length);
		return phoenix::buffer::of(std::move(bytes));
	}

	std::unique_ptr<Read> ReadArchiveBinsafe::read_raw(std::size_t size) {
		auto length = ensure_entry_meta<ArchiveEntryType::RAW>();

		if (length < size) {
			throw ParserError {"ReadArchive.Binsafe", "not enough raw bytes to read!"};
		} else if (length > size) {
			ZKLOGW("ReadArchive.Binsafe", "Reading %zu bytes although %d are actually available", size, length);
		}

		std::vector bytes(length, std::byte {});
		read->read(bytes.data(), length);
		return Read::from(std::move(bytes));
	}

	void ReadArchiveBinsafe::skip_entry() {
		auto type = static_cast<ArchiveEntryType>(read->read_ubyte());

		switch (type) {
		case ArchiveEntryType::STRING:
		case ArchiveEntryType::RAW:
		case ArchiveEntryType::RAW_FLOAT:
			read->seek(read->read_ushort(), Whence::CUR);
			break;
		case ArchiveEntryType::ENUM:
		case ArchiveEntryType::HASH:
		case ArchiveEntryType::INTEGER:
		case ArchiveEntryType::FLOAT:
		case ArchiveEntryType::BOOL:
		case ArchiveEntryType::COLOR:
			(void) read->read_uint();
			break;
		case ArchiveEntryType::BYTE:
			(void) read->read_ubyte();
			break;
		case ArchiveEntryType::WORD:
			(void) read->read_ushort();
			break;
		case ArchiveEntryType::VEC3:
			(void) read->read_float();
			(void) read->read_float();
			(void) read->read_float();
			break;
		}
	}

	template <ArchiveEntryType tp>
	std::uint16_t ReadArchiveBinsafe::ensure_entry_meta() {
		auto type = static_cast<ArchiveEntryType>(read->read_ubyte());

		if (type != ArchiveEntryType::HASH) {
			throw zenkit::ParserError {"ReadArchive.Binsafe", "invalid format"};
		}

		read->seek(sizeof(uint32_t), Whence::CUR);
		type = static_cast<ArchiveEntryType>(read->read_ubyte());

		if (type != tp) {
			throw zenkit::ParserError {"ReadArchive.Binsafe: type mismatch: expected " +
			                           std::to_string(static_cast<uint8_t>(tp)) +
			                           ", got: " + std::to_string(static_cast<uint32_t>(type))};
		}

		if constexpr (tp == ArchiveEntryType::STRING || tp == ArchiveEntryType::RAW ||
		              tp == ArchiveEntryType::RAW_FLOAT) {
			return read->read_ushort();
		} else {
			return type_sizes[static_cast<uint8_t>(type)];
		}
	}

	WriteArchiveBinsafe::WriteArchiveBinsafe(Write* w) : _m_write(w) {
		this->_m_head = this->_m_write->tell();
		this->write_header();
	}

	std::uint32_t WriteArchiveBinsafe::write_object_begin(std::string_view object_name,
	                                                      std::string_view class_name,
	                                                      std::uint16_t version) {
		this->_m_write->write_ubyte(static_cast<uint8_t>(ArchiveEntryType::STRING));

		char buf[6 + 127 * 2 + 10 * 2];
		auto n = snprintf(buf,
		                  sizeof buf - 1,
		                  "[%.127s %.127s %d %d]",
		                  object_name.data(),
		                  class_name.data(),
		                  version,
		                  _m_index);

		this->_m_write->write_ushort(static_cast<uint16_t>(n));
		this->_m_write->write_string(buf);

		auto idx = _m_index;
		_m_index++;

		return idx;
	}

	void WriteArchiveBinsafe::write_object_end() {
		this->_m_write->write_ubyte(static_cast<uint8_t>(ArchiveEntryType::STRING));
		this->_m_write->write_ushort(2);
		this->_m_write->write_string("[]");
	}

	void WriteArchiveBinsafe::write_ref(std::string_view object_name, uint32_t index) {
		this->_m_write->write_ubyte(static_cast<uint8_t>(ArchiveEntryType::STRING));

		char buf[6 + 127 * 2 + 10 * 2];
		auto n = snprintf(buf, sizeof buf - 1, "[%.127s %.127s %d %d]", object_name.data(), "\xA7", 0, index);

		this->_m_write->write_ushort(n);
		this->_m_write->write_string(buf);

		this->_m_write->write_ubyte(static_cast<uint8_t>(ArchiveEntryType::STRING));
		this->_m_write->write_ushort(2);
		this->_m_write->write_string("[]");
	}

	void WriteArchiveBinsafe::write_string(std::string_view name, std::string_view v) {
		this->write_entry(name, ArchiveEntryType::STRING);
		this->_m_write->write_ushort(v.length());
		this->_m_write->write_string(v);
	}

	void WriteArchiveBinsafe::write_int(std::string_view name, std::int32_t v) {
		this->write_entry(name, ArchiveEntryType::INTEGER);
		this->_m_write->write_int(v);
	}

	void WriteArchiveBinsafe::write_float(std::string_view name, float v) {
		this->write_entry(name, ArchiveEntryType::FLOAT);
		this->_m_write->write_float(v);
	}

	void WriteArchiveBinsafe::write_byte(std::string_view name, std::uint8_t v) {
		this->write_entry(name, ArchiveEntryType::BYTE);
		this->_m_write->write_ubyte(v);
	}

	void WriteArchiveBinsafe::write_word(std::string_view name, std::uint16_t v) {
		this->write_entry(name, ArchiveEntryType::WORD);
		this->_m_write->write_ushort(v);
	}

	void WriteArchiveBinsafe::write_enum(std::string_view name, std::uint32_t v) {
		this->write_entry(name, ArchiveEntryType::ENUM);
		this->_m_write->write_uint(v);
	}

	void WriteArchiveBinsafe::write_bool(std::string_view name, bool v) {
		this->write_entry(name, ArchiveEntryType::BOOL);
		this->_m_write->write_uint(v ? 1 : 0);
	}

	void WriteArchiveBinsafe::write_color(std::string_view name, glm::u8vec4 v) {
		this->write_entry(name, ArchiveEntryType::COLOR);
		this->_m_write->write_ubyte(v.b);
		this->_m_write->write_ubyte(v.g);
		this->_m_write->write_ubyte(v.r);
		this->_m_write->write_ubyte(v.a);
	}

	void WriteArchiveBinsafe::write_vec3(std::string_view name, glm::vec3 const& v) {
		this->write_entry(name, ArchiveEntryType::VEC3);
		this->_m_write->write_vec3(v);
	}

	void WriteArchiveBinsafe::write_vec2(std::string_view name, glm::vec2 v) {
		this->write_raw_float(name, glm::value_ptr(v), 2);
	}

	void WriteArchiveBinsafe::write_bbox(std::string_view name, AxisAlignedBoundingBox const& v) {
		float values[] {v.min.x, v.min.y, v.min.z, v.max.x, v.max.y, v.max.z};
		this->write_raw_float(name, values, 6);
	}

	void WriteArchiveBinsafe::write_mat3x3(std::string_view name, glm::mat3x3 const& v) {
		auto vT = glm::transpose(v);
		this->write_raw_float(name, glm::value_ptr(vT), 9);
	}

	void WriteArchiveBinsafe::write_raw(std::string_view name, std::vector<std::byte> const& v) {
		this->write_raw(name, v.data(), v.size());
	}

	void WriteArchiveBinsafe::write_raw(std::string_view name, std::byte const* v, std::uint16_t length) {
		this->write_entry(name, ArchiveEntryType::RAW);
		this->_m_write->write_ushort(length);
		this->_m_write->write(v, length);
	}

	void WriteArchiveBinsafe::write_raw_float(std::string_view name, float const* v, std::uint16_t length) {
		this->write_entry(name, ArchiveEntryType::RAW_FLOAT);
		this->_m_write->write_ushort(length * 4);
		this->_m_write->write(v, length * 4);
	}

	void WriteArchiveBinsafe::write_header() {
		auto cur = this->_m_write->tell();
		this->_m_write->seek(_m_head, Whence::BEG);

		char const* username = getenv("USER");
		if (username == nullptr) username = getenv("USERNAME");
		if (username == nullptr) username = "Anonymous";

		char date_buffer[20];
		std::time_t clk;
		std::time(&clk);
		auto* time = std::localtime(&clk);
		strftime(date_buffer, 20, "%d.%m.%Y %H:%M:%S", time);

		this->_m_write->write_line("ZenGin Archive");
		this->_m_write->write_line("ver 1");
		this->_m_write->write_line("zCArchiverBinSafe");
		this->_m_write->write_line("BIN_SAFE");
		this->_m_write->write_line(is_save_game() ? "saveGame 1" : "saveGame 0");
		this->_m_write->write_string("date ");
		this->_m_write->write_line(date_buffer);
		this->_m_write->write_string("user ");
		this->_m_write->write_line(username);
		this->_m_write->write_line("END");

		this->_m_write->write_uint(2);
		this->_m_write->write_uint(_m_index);

		// Write the hash table!
		this->_m_write->write_uint(cur);

		if (cur != _m_head) {
			this->_m_write->seek(cur, Whence::BEG);
		} else {
			cur = this->_m_write->tell();
		}

		this->_m_write->write_uint(_m_hash_keys.size());

		std::vector<std::pair<std::string, size_t>> vc {_m_hash_keys.begin(), _m_hash_keys.end()};
		std::sort(vc.begin(), vc.end(), [](auto const& a, auto const& b) { return a.second < b.second; });

		for (auto const& [key, value] : vc) {
			this->_m_write->write_ushort(key.length());
			this->_m_write->write_ushort(value);

			auto hash_value = 0u;
			for (char c : key) {
				hash_value = hash_value * 0x21 + c;
			}

			this->_m_write->write_uint(hash_value & 0x61);
			this->_m_write->write_string(key);
		}

		// Place the cursor before the hash table.
		this->_m_write->seek(cur, Whence::BEG);
	}

	void WriteArchiveBinsafe::write_entry(std::string_view name, ArchiveEntryType type) {
		this->_m_write->write_ubyte(static_cast<uint8_t>(ArchiveEntryType::HASH));

		auto it = this->_m_hash_keys.find(name);
		if (it == this->_m_hash_keys.end()) {
			auto key = this->_m_hash_keys.size();
			this->_m_hash_keys.emplace(std::string {name}, key);
			this->_m_write->write_uint(key);
		} else {
			this->_m_write->write_uint(it->second);
		}

		this->_m_write->write_ubyte(static_cast<uint8_t>(type));
	}
} // namespace zenkit
