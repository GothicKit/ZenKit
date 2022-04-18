// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "archive_binsafe.hh"

#include <fmt/format.h>
#include <phoenix/detail/error.hh>
#include <sstream>

namespace phoenix {
	void archive_reader_binsafe::read_header() {
		_m_bs_version = input.get_uint();
		_m_object_count = input.get_uint();
		auto hash_table_offset = input.get_uint();

		// TODO: Understand and implement hashes
		auto revert = input.position();

		{
			input.position(hash_table_offset);
			auto hash_table_size = input.get_uint();
			_m_hash_table_entries.resize(hash_table_size);

			for (std::uint32_t i = 0; i < hash_table_size; ++i) {
				auto key_length = input.get_ushort();
				auto insertion_index = input.get_ushort();
				auto hash_value = input.get_uint();

				_m_hash_table_entries[insertion_index] = hash_table_entry {input.get_string(key_length), hash_value};
			}
		}

		input.position(revert);
	}

	bool archive_reader_binsafe::read_object_begin(archive_object& obj) {
		input.mark();

		if (input.get() != bs_string) {
			input.reset();
			return false;
		}

		auto line = input.get_string(input.get_ushort());
		if (!line.starts_with('[') || !line.ends_with(']') || line.length() <= 2) {
			input.reset();
			return false;
		}

		std::stringstream ss {line.substr(1, line.size() - 2)};
		ss >> obj.object_name >> obj.class_name >> obj.version >> obj.index;

		skip_optional_hash();
		return true;
	}

	bool archive_reader_binsafe::read_object_end() {
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

		skip_optional_hash();
		return true;
	}

	void archive_reader_binsafe::skip_optional_hash() {
		if (input.get(input.position()) != bs_hash)
			return;

		(void) input.skip(1).get_uint();
	}

	std::uint16_t archive_reader_binsafe::assure_entry(archive_binsafe_type tp) {
		auto type = input.get();
		auto size =
		    (type == bs_string || type == bs_raw || type == bs_raw_float) ? input.get_ushort() : type_sizes[type];

		if (type != tp) {
			input.skip(size);
			throw parser_error(fmt::format("archive_reader_binsafe: type mismatch: expected {}, got: {}", tp, type));
		}

		return size;
	}

	std::string archive_reader_binsafe::read_string() {
		auto rv = input.get_string(assure_entry(bs_string));
		skip_optional_hash();
		return rv;
	}

	std::int32_t archive_reader_binsafe::read_int() {
		assure_entry(bs_int);
		auto rv = input.get_int();
		skip_optional_hash();
		return rv;
	}

	float archive_reader_binsafe::read_float() {
		assure_entry(bs_float);
		auto rv = input.get_float();
		skip_optional_hash();
		return rv;
	}

	std::uint8_t archive_reader_binsafe::read_byte() {
		assure_entry(bs_byte);
		auto rv = input.get();
		skip_optional_hash();
		return rv;
	}

	std::uint16_t archive_reader_binsafe::read_word() {
		assure_entry(bs_word);
		auto rv = input.get_ushort();
		skip_optional_hash();
		return rv;
	}

	std::uint32_t archive_reader_binsafe::read_enum() {
		assure_entry(bs_enum);
		auto rv = input.get_uint();
		skip_optional_hash();
		return rv;
	}

	bool archive_reader_binsafe::read_bool() {
		assure_entry(bs_bool);
		auto rv = input.get_uint(); // TODO: Check if this actually applies
		skip_optional_hash();
		return rv == 1;
	}

	glm::u8vec4 archive_reader_binsafe::read_color() {
		assure_entry(bs_color);
		glm::u8vec4 c {
		    input.get(),
		    input.get(),
		    input.get(),
		    input.get(),
		};
		skip_optional_hash();
		return c;
	}

	glm::vec3 archive_reader_binsafe::read_vec3() {
		assure_entry(bs_vec3);
		auto c = input.get_vec3();
		skip_optional_hash();
		return c;
	}

	glm::vec2 archive_reader_binsafe::read_vec2() {
		auto unused = static_cast<std::int32_t>(assure_entry(bs_raw_float) - 2 * sizeof(float));

		if (unused < 0) {
			throw parser_error(
			    "archive_reader_binsafe: cannot read vec2 (2 * float): not enough space in rawFloat entry.");
		}

		auto c = input.get_vec2();

		// There might be more bytes in this. We'll ignore them.
		input.skip(unused);
		skip_optional_hash();
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

		skip_optional_hash();
	}

	bounding_box archive_reader_binsafe::read_bbox() {
		auto unused = static_cast<std::int32_t>(assure_entry(bs_raw_float) - 3 * 2 * sizeof(float));

		if (unused < 0) {
			throw parser_error(
			    "archive_reader_binsafe: cannot read bbox (6 * float): not enough space in rawFloat entry.");
		}

		auto c = bounding_box::parse(input);

		// There might be more bytes in this. We'll ignore them.
		input.skip(unused);
		skip_optional_hash();
		return c;
	}

	glm::mat3x3 archive_reader_binsafe::read_mat3x3() {
		auto unused = static_cast<std::int32_t>(assure_entry(bs_raw) - 3 * 3 * sizeof(float));

		if (unused < 0) {
			throw parser_error("archive_reader_binsafe: cannot read bbox (9 * float): not enough space in raw entry.");
		}

		glm::mat3x3 v = input.get_mat3x3();

		input.skip(unused);
		skip_optional_hash();
		return v;
	}

	buffer archive_reader_binsafe::read_raw_bytes() {
		auto length = assure_entry(bs_raw);
		auto buf = input.extract(length);

		skip_optional_hash();
		return buf;
	}
} // namespace phoenix
