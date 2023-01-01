// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "archive_binsafe.hh"

#include <cstring>
#include <iostream>

namespace phoenix {
	void archive_reader_binsafe::read_header() {
		_m_bs_version = input.get_uint();
		_m_object_count = input.get_uint();
		auto hash_table_offset = input.get_uint();

		{
			input.mark();
			input.position(hash_table_offset);
			auto hash_table_size = input.get_uint();
			_m_hash_table_entries.resize(hash_table_size);

			for (std::uint32_t i = 0; i < hash_table_size; ++i) {
				auto key_length = input.get_ushort();
				auto insertion_index = input.get_ushort();
				auto hash_value = input.get_uint();
				auto key = input.get_string(key_length);

				_m_hash_table_entries[insertion_index] = hash_table_entry {key, hash_value};
			}

			input.reset();
		}
	}

	bool archive_reader_binsafe::read_object_begin(archive_object& obj) {
		if (input.remaining() < 6)
			return false;
		input.mark();

		if (static_cast<archive_entry_type>(input.get()) != archive_entry_type::string) {
			input.reset();
			return false;
		}

		auto line = input.get_string(input.get_ushort());

		// Fail quickly if we know this can't be an object begin
		if (line.length() <= 2) {
			input.reset();
			return false;
		}

		char* it = line.data();
		if (*it++ != '[') {
			input.reset();
			return false;
		}

		auto* object_name = strtok(it, " ]");
		auto* class_name = strtok(nullptr, " ]");
		auto* version = strtok(nullptr, " ]");
		auto* index = strtok(nullptr, " ]");

		if (object_name == nullptr || class_name == nullptr || version == nullptr || index == nullptr) {
			input.reset();
			return false;
		}

		obj.version = atoi(version);
		obj.index = atoi(index);
		obj.object_name = object_name;
		obj.class_name = class_name;
		return true;
	}

	bool archive_reader_binsafe::read_object_end() {
		if (input.remaining() == 0)
			return true;
		if (input.remaining() < 6)
			return false;
		input.mark();

		if (static_cast<archive_entry_type>(input.get()) != archive_entry_type::string) {
			input.reset();
			return false;
		}

		if (input.get_ushort() != 2) {
			input.reset();
			return false;
		}

		if (input.get_string(2) != "[]") {
			input.reset();
			return false;
		}

		return true;
	}

	const std::string& archive_reader_binsafe::get_entry_key() {
		if (static_cast<archive_entry_type>(input.get(input.position())) != archive_entry_type::hash) {
			throw parser_error {"archive_reader_binsafe", "invalid format"};
		}

		input.skip(1);
		auto hash = input.get_uint();
		return _m_hash_table_entries[hash].key;
	}

	std::string archive_reader_binsafe::read_string() {
		return input.get_string(ensure_entry_meta<archive_entry_type::string>());
	}

	std::int32_t archive_reader_binsafe::read_int() {
		ensure_entry_meta<archive_entry_type::int_>();
		return input.get_int();
	}

	float archive_reader_binsafe::read_float() {
		ensure_entry_meta<archive_entry_type::float_>();
		return input.get_float();
	}

	std::uint8_t archive_reader_binsafe::read_byte() {
		ensure_entry_meta<archive_entry_type::byte>();
		return input.get();
	}

	std::uint16_t archive_reader_binsafe::read_word() {
		ensure_entry_meta<archive_entry_type::word>();
		return input.get_ushort();
	}

	std::uint32_t archive_reader_binsafe::read_enum() {
		ensure_entry_meta<archive_entry_type::enum_>();
		return input.get_uint();
	}

	bool archive_reader_binsafe::read_bool() {
		ensure_entry_meta<archive_entry_type::bool_>();
		return input.get_uint() != 0;
	}

	glm::u8vec4 archive_reader_binsafe::read_color() {
		ensure_entry_meta<archive_entry_type::color>();

		auto b = input.get();
		auto g = input.get();
		auto r = input.get();
		auto a = input.get();

		return {r, g, b, a};
	}

	glm::vec3 archive_reader_binsafe::read_vec3() {
		ensure_entry_meta<archive_entry_type::vec3>();
		return input.get_vec3();
	}

	glm::vec2 archive_reader_binsafe::read_vec2() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta<archive_entry_type::raw_float>() - 2 * sizeof(float));

		if (unused < 0) {
			throw parser_error {"archive_reader_binsafe"
			                    "cannot read vec2 (2 * float): not enough space in rawFloat entry."};
		}

		auto c = input.get_vec2();

		// There might be more bytes in this. We'll ignore them.
		input.skip(unused);
		return c;
	}

	bounding_box archive_reader_binsafe::read_bbox() {
		auto unused =
		    static_cast<std::int32_t>(ensure_entry_meta<archive_entry_type::raw_float>() - 3 * 2 * sizeof(float));

		if (unused < 0) {
			throw parser_error {"archive:reader_binsafe",
			                    "cannot read bbox (6 * float): not enough space in rawFloat entry."};
		}

		auto c = bounding_box::parse(input);

		// There might be more bytes in this. We'll ignore them.
		input.skip(unused);
		return c;
	}

	glm::mat3x3 archive_reader_binsafe::read_mat3x3() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta<archive_entry_type::raw>() - 3 * 3 * sizeof(float));

		if (unused < 0) {
			throw parser_error(
			    "archive_reader_binsafe: cannot read mat3x3 (9 * float): not enough space in raw entry.");
		}

		auto v = input.get_mat3x3();
		input.skip(unused);
		return glm::transpose(v);
	}

	buffer archive_reader_binsafe::read_raw_bytes() {
		auto length = ensure_entry_meta<archive_entry_type::raw>();
		return input.extract(length);
	}

	buffer archive_reader_binsafe::read_raw_bytes(uint32_t size) {
		auto length = ensure_entry_meta<archive_entry_type::raw>();

		if (length < size) {
			throw parser_error {"archive_reader_binsafe", "not enough raw bytes to read!"};
		} else if (length > size) {
			PX_LOGW("read_raw_bytes: reading ", size, " bytes although ", length, " are actually available");
		}

		return input.extract(length);
	}

	void archive_reader_binsafe::skip_entry() {
		auto type = static_cast<archive_entry_type>(input.get());

		switch (type) {
		case archive_entry_type::string:
		case archive_entry_type::raw:
		case archive_entry_type::raw_float:
			input.skip(input.get_ushort());
			break;
		case archive_entry_type::enum_:
		case archive_entry_type::hash:
		case archive_entry_type::int_:
		case archive_entry_type::float_:
		case archive_entry_type::bool_:
		case archive_entry_type::color:
			(void) input.get_uint();
			break;
		case archive_entry_type::byte:
			(void) input.get();
			break;
		case archive_entry_type::word:
			(void) input.get_ushort();
			break;
		case archive_entry_type::vec3:
			(void) input.get_float();
			(void) input.get_float();
			(void) input.get_float();
			break;
		}
	}

	std::variant<archive_object, archive_object_end, archive_entry> archive_reader_binsafe::unstable__next() {
		static archive_object obj {};
		if (read_object_begin(obj)) {
			return obj;
		} else if (read_object_end()) {
			return archive_object_end {};
		} else {
			archive_entry entry {};
			entry.name = get_entry_key();
			entry.type = static_cast<archive_entry_type>(input.get());

			switch (entry.type) {
			case archive_entry_type::string:
				entry.value = input.get_string(input.get_ushort());
				break;
			case archive_entry_type::raw:
			case archive_entry_type::raw_float:
				entry.value = input.extract(input.get_ushort());
				break;
			case archive_entry_type::enum_:
				entry.value = input.get_uint();
				break;
			case archive_entry_type::hash:
				entry.value = input.get_uint();
				break;
			case archive_entry_type::int_:
				entry.value = input.get_int();
				break;
			case archive_entry_type::float_:
				entry.value = input.get_float();
				break;
			case archive_entry_type::bool_:
				entry.value = input.get_uint() != 0;
				break;
			case archive_entry_type::color: {
				auto b = input.get();
				auto g = input.get();
				auto r = input.get();
				auto a = input.get();

				entry.value = glm::u8vec4 {r, g, b, a};
				break;
			}
			case archive_entry_type::byte:
				entry.value = input.get();
				break;
			case archive_entry_type::word:
				entry.value = input.get_ushort();
				break;
			case archive_entry_type::vec3: {
				auto x = input.get_float();
				auto y = input.get_float();
				auto z = input.get_float();

				entry.value = glm::vec3 {x, y, z};
				break;
			}
			}

			return entry;
		}
	}
} // namespace phoenix
