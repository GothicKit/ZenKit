// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "archive_binsafe.hh"

#include <iostream>
#include <sstream>

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

		if (input.get() != bs_string) {
			input.reset();
			return false;
		}

		auto line = input.get_string(input.get_ushort());

		// Fail quickly if we know this can't be an object begin
		if (line.length() <= 2) {
			input.reset();
			return false;
		}

		char class_name[128];
		char object_name[128];

		auto parsed_elements =
		    std::sscanf(line.c_str(), "[%127s %127s %hu %u]", object_name, class_name, &obj.version, &obj.index);

		if (parsed_elements != 4) {
			input.reset();
			return false;
		}

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

		if (input.get() != bs_string) {
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
		if (input.get(input.position()) != bs_hash) {
			throw parser_error {"archive_reader_binsafe", "invalid format"};
		}

		input.skip(1);
		auto hash = input.get_uint();
		return _m_hash_table_entries[hash].key;
	}

	std::uint16_t archive_reader_binsafe::ensure_entry_meta(archive_binsafe_type tp) {
		(void) get_entry_key();
		auto type = input.get();
		uint16_t size =
		    (type == bs_string || type == bs_raw || type == bs_raw_float) ? input.get_ushort() : type_sizes[type];

		if (type != tp) {
			input.skip(size);
			throw parser_error {"archive_reader_binsafe: type mismatch: expected " + std::to_string(tp) +
			                    ", got: " + std::to_string(static_cast<uint32_t>(type))};
		}

		return size;
	}

	std::string archive_reader_binsafe::read_string() {
		auto rv = input.get_string(ensure_entry_meta(bs_string));
		return rv;
	}

	std::int32_t archive_reader_binsafe::read_int() {
		ensure_entry_meta(bs_int);
		auto rv = input.get_int();
		return rv;
	}

	float archive_reader_binsafe::read_float() {
		ensure_entry_meta(bs_float);
		auto rv = input.get_float();
		return rv;
	}

	std::uint8_t archive_reader_binsafe::read_byte() {
		ensure_entry_meta(bs_byte);
		return input.get();
	}

	std::uint16_t archive_reader_binsafe::read_word() {
		ensure_entry_meta(bs_word);
		return input.get_ushort();
	}

	std::uint32_t archive_reader_binsafe::read_enum() {
		ensure_entry_meta(bs_enum);
		return input.get_uint();
	}

	bool archive_reader_binsafe::read_bool() {
		ensure_entry_meta(bs_bool);
		return input.get_uint() != 0;
	}

	glm::u8vec4 archive_reader_binsafe::read_color() {
		ensure_entry_meta(bs_color);

		auto b = input.get();
		auto g = input.get();
		auto r = input.get();
		auto a = input.get();

		return {r, g, b, a};
	}

	glm::vec3 archive_reader_binsafe::read_vec3() {
		ensure_entry_meta(bs_vec3);
		return input.get_vec3();
	}

	glm::vec2 archive_reader_binsafe::read_vec2() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta(bs_raw_float) - 2 * sizeof(float));

		if (unused < 0) {
			throw parser_error {"archive_reader_binsafe"
			                    "cannot read vec2 (2 * float): not enough space in rawFloat entry."};
		}

		auto c = input.get_vec2();

		// There might be more bytes in this. We'll ignore them.
		input.skip(unused);
		return c;
	}

	void archive_reader_binsafe::skip_entry() {
		auto type = static_cast<archive_binsafe_type>(input.get());

		switch (type) {
		case bs_string:
		case bs_raw:
		case bs_raw_float:
			input.skip(input.get_ushort());
			break;
		case bs_enum:
		case bs_hash:
		case bs_int:
		case bs_float:
		case bs_bool:
		case bs_color:
			(void) input.get_uint();
			break;
		case bs_byte:
			(void) input.get();
			break;
		case bs_word:
			(void) input.get_ushort();
			break;
		case bs_vec3:
			(void) input.get_float();
			(void) input.get_float();
			(void) input.get_float();
			break;
		}
	}

	bounding_box archive_reader_binsafe::read_bbox() {
		auto unused = static_cast<std::int32_t>(ensure_entry_meta(bs_raw_float) - 3 * 2 * sizeof(float));

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
		auto unused = static_cast<std::int32_t>(ensure_entry_meta(bs_raw) - 3 * 3 * sizeof(float));

		if (unused < 0) {
			throw parser_error(
			    "archive_reader_binsafe: cannot read mat3x3 (9 * float): not enough space in raw entry.");
		}

		auto v = input.get_mat3x3();
		input.skip(unused);
		return glm::transpose(v);
	}

	buffer archive_reader_binsafe::read_raw_bytes() {
		auto length = ensure_entry_meta(bs_raw);
		return input.extract(length);
	}

	buffer archive_reader_binsafe::read_raw_bytes(uint32_t size) {
		auto length = ensure_entry_meta(bs_raw);

		if (length < size) {
			throw parser_error {"archive_reader_binsafe", "not enough raw bytes to read!"};
		} else if (length > size) {
			PX_LOGW("read_raw_bytes: reading ", size, " bytes although ", length, " are actually available");
		}

		return input.extract(length);
	}

	constexpr std::string_view type_names[] = {
	    "unknown", // ?            = 0x00
	    "string",  // bs_string    = 0x01,
	    "int",     // bs_int       = 0x02,
	    "float",   // bs_float     = 0x03,
	    "byte",    // bs_byte      = 0x04,
	    "word",    // bs_word      = 0x05,
	    "bool",    // bs_bool      = 0x06,
	    "vec3",    // bs_vec3      = 0x07,
	    "color",   // bs_color     = 0x08,
	    "raw",     // bs_raw       = 0x09,
	    "unknown", // ?            = 0x0A
	    "unknown", // ?            = 0x0B
	    "unknown", // ?            = 0x0C
	    "unknown", // ?            = 0x0D
	    "unknown", // ?            = 0x0E
	    "unknown", // ?            = 0x0F
	    "unknown", // bs_raw_float = 0x10,
	    "enum",    // bs_enum      = 0x11,
	    "hash",    // bs_hash      = 0x12,
	};

	void archive_reader_binsafe::print_entry() {
		auto type = static_cast<archive_binsafe_type>(input.get());

		if (type != bs_hash) {
			std::cout << "<field name=\"unknown\" type=\"" << type_names[type] << "\" ";
		} else {
			auto hash = input.get_uint();
			auto& name = _m_hash_table_entries[hash].key;

			type = static_cast<archive_binsafe_type>(input.get());

			std::cout << "<field name=\"" << name << "\" type=\"" << type_names[type] << "\" ";
		}

		switch (type) {
		case bs_string:
			std::cout << "value=\"" << input.get_string(input.get_ushort()) << "\" ";
			break;
		case bs_raw:
		case bs_raw_float: {
			auto size = input.get_ushort();
			std::cout << "length=\"" << size << "\" ";

			input.skip(size);
			break;
		}
		case bs_enum:
		case bs_int:
		case bs_bool:
		case bs_color:
		case bs_hash:
			std::cout << "value=\"" << input.get_uint() << "\" ";
			break;
		case bs_float:
			std::cout << "value=\"" << input.get_float() << "\" ";
			break;
		case bs_byte:
			std::cout << "value=\"" << static_cast<uint16_t>(input.get()) << "\" ";
			break;
		case bs_word:
			std::cout << "value=\"" << input.get_ushort() << "\" ";
			break;
		case bs_vec3: {
			auto x = input.get_float();
			auto y = input.get_float();
			auto z = input.get_float();

			std::cout << "value=\"(" << x << ", " << y << ", " << z << ")\" ";
			break;
		}
		}

		std::cout << "/>\n";
	}
} // namespace phoenix
