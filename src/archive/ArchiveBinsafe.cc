// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "ArchiveBinsafe.hh"

#include "phoenix/buffer.hh"

#include "../Internal.hh"

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

		obj.version = atoi(version);
		obj.index = atoi(index);
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
			throw zenkit::ParserError {"ReadArchive.Binsafe"
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
			throw zenkit::ParserError {"ReadArchive.Binsafe",
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
			throw zenkit::ParserError(
			    "ReadArchive.Binsafe: cannot read mat3x3 (9 * float): not enough space in raw entry.");
		}

		auto v = read->read_mat3();

		// There might be more bytes in this. We'll ignore them.
		read->seek(unused, Whence::CUR);
		return v;
	}

	phoenix::buffer ReadArchiveBinsafe::read_raw_bytes(uint32_t size) {
		auto length = ensure_entry_meta<ArchiveEntryType::RAW>();

		if (length < size) {
			throw zenkit::ParserError {"ReadArchive.Binsafe", "not enough raw bytes to read!"};
		} else if (length > size) {
			ZKLOGW("ReadArchive.Binsafe", "Reading %d bytes although %d are actually available", size, length);
		}

		std::vector<std::byte> bytes(length, std::byte {});
		read->read(bytes.data(), length);
		return phoenix::buffer::of(std::move(bytes));
	}

	std::unique_ptr<Read> ReadArchiveBinsafe::read_raw(uint32_t size) {
		auto length = ensure_entry_meta<ArchiveEntryType::RAW>();

		if (length < size) {
			throw zenkit::ParserError {"ReadArchive.Binsafe", "not enough raw bytes to read!"};
		} else if (length > size) {
			ZKLOGW("ReadArchive.Binsafe", "Reading %d bytes although %d are actually available", size, length);
		}

		std::vector<std::byte> bytes(length, std::byte {});
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
} // namespace zenkit
