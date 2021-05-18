// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "archive_binsafe.hh"

#include <fmt/format.h>
#include <phoenix/detail/error.hh>
#include <sstream>

namespace phoenix {
	void archive_reader_binsafe::read_header() {
		_m_bs_version = input.read_u32();
		_m_object_count = input.read_u32();
		_m_hash_table_offset = input.read_u32();// TODO: Understand and implement hashes
	}

	bool archive_reader_binsafe::read_object_begin(archive_object& obj) {
		return peek_input([&](reader& in) {
			if (in.read_u8() != bs_string) { return false; }

			auto line = in.read_string(in.read_u16());
			if (!line.starts_with('[') || !line.ends_with(']')) { return false; }
			if (line.length() <= 2) { return false; }

			std::stringstream ss {line.substr(1, line.size() - 2)};
			ss >> obj.object_name >> obj.class_name >> obj.version >> obj.index;

			skip_optional_hash();
			return true;
		});
	}

	bool archive_reader_binsafe::read_object_end() {
		return peek_input([&](reader& in) {
			if (in.read_u8() != bs_string) { return false; }
			if (in.read_u16() != 2) { return false; }
			if (in.read_string(2) != "[]") { return false; }
			skip_optional_hash();

			return true;
		});
	}

	void archive_reader_binsafe::skip_optional_hash() {
		if (peek_input([](reader& in) { return in.read_u8() == bs_hash; })) {
			input.ignore(sizeof(u32));
		}
	}

	u16 archive_reader_binsafe::assure_entry(archive_binsafe_type tp) {
		auto type = input.read_u8();
		auto size = (type == bs_string || type == bs_raw || type == bs_raw_float) ? input.read_u16() : type_sizes[type];

		if (type != tp) {
			input.ignore(size);
			throw parser_error(fmt::format("archive_reader_binsafe: type mismatch: expected {}, got: {}", tp, type));
		}

		return size;
	}

	std::string archive_reader_binsafe::read_string() {
		return input.read_string(assure_entry(bs_string));
	}

	s32 archive_reader_binsafe::read_int() {
		assure_entry(bs_int);
		auto rv = input.read_s32();
		skip_optional_hash();
		return rv;
	}

	float archive_reader_binsafe::read_float() {
		assure_entry(bs_float);
		auto rv = input.read_f32();
		skip_optional_hash();
		return rv;
	}

	u8 archive_reader_binsafe::read_byte() {
		assure_entry(bs_byte);
		auto rv = input.read_u8();
		skip_optional_hash();
		return rv;
	}

	u16 archive_reader_binsafe::read_word() {
		assure_entry(bs_word);
		auto rv = input.read_u16();
		skip_optional_hash();
		return rv;
	}

	u32 archive_reader_binsafe::read_enum() {
		assure_entry(bs_enum);
		auto rv = input.read_u32();
		skip_optional_hash();
		return rv;
	}

	bool archive_reader_binsafe::read_bool() {
		assure_entry(bs_bool);
		auto rv = input.read_u32();// TODO: Check if this actually applies
		skip_optional_hash();
		return rv == 1;
	}

	color archive_reader_binsafe::read_color() {
		assure_entry(bs_color);
		color c {
				input.read_u8(),
				input.read_u8(),
				input.read_u8(),
				input.read_u8(),
		};
		skip_optional_hash();
		return c;
	}

	glm::vec3 archive_reader_binsafe::read_vec3() {
		assure_entry(bs_vec3);
		auto c = input.read_vec3();
		skip_optional_hash();
		return c;
	}

	glm::vec2 archive_reader_binsafe::read_vec2() {
		auto unused = assure_entry(bs_raw_float) - 2 * sizeof(float);

		if (unused < 0) {
			throw parser_error("archive_reader_binsafe: cannot read vec2 (2 * float): not enough space in rawFloat entry.");
		}

		auto c = input.read_vec2();

		// There might be more bytes in this. We'll ignore them.
		input.ignore(unused);
		skip_optional_hash();
		return c;
	}

	void archive_reader_binsafe::skip_entry() {
		auto type = static_cast<archive_binsafe_type>(input.read_u8());

		switch (type) {
			case bs_string:
			case bs_raw:
			case bs_raw_float:
				input.ignore(input.read_u16());
				break;
			case bs_enum:
			case bs_hash:
			case bs_int:
			case bs_float:
			case bs_bool:
			case bs_color:
				input.ignore(sizeof(u32));
				break;
			case bs_byte:
				input.ignore(sizeof(u8));
				break;
			case bs_word:
				input.ignore(sizeof(u16));
				break;
			case bs_vec3:
				input.ignore(sizeof(float) * 3);
				break;
		}

		skip_optional_hash();
	}

	std::tuple<glm::vec3, glm::vec3> archive_reader_binsafe::read_bbox() {
		auto unused = assure_entry(bs_raw_float) - 3 * 2 * sizeof(float);

		if (unused < 0) {
			throw parser_error("archive_reader_binsafe: cannot read bbox (6 * float): not enough space in rawFloat entry.");
		}

		auto c = std::make_tuple(input.read_vec3(), input.read_vec3());

		// There might be more bytes in this. We'll ignore them.
		input.ignore(unused);
		skip_optional_hash();
		return c;
	}
}// namespace phoenix