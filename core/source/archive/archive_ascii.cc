// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "archive_ascii.hh"

#include <fmt/format.h>
#include <phoenix/detail/error.hh>

#include <bit>
#include <charconv>
#include <sstream>

namespace phoenix {
	void archive_reader_ascii::read_header() {
		{
			std::string objects = input.read_line();
			if (!objects.starts_with("objects ")) { throw parser_error("not an archive: objects missing"); }
			_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
		}

		if (input.read_line() != "END") {
			throw parser_error("not an archive: END(2) missing");
		}
	}

	bool archive_reader_ascii::read_object_begin(archive_object& obj) {
		return peek_input([&](reader& in) {
			auto v = in.read_line();

			if (!v.starts_with('[') || !v.ends_with(']')) { return false; }
			if (v.length() <= 2) { return false; }

			std::stringstream ss {v.substr(1, v.size() - 2)};
			ss >> obj.object_name >> obj.class_name >> obj.version >> obj.index;
			return true;
		});
	}

	bool archive_reader_ascii::read_object_end() {
		return peek_input([&](reader& in) {
			auto v = in.read_line();
			if (v == "[]") {
				return true;
			}

			return false;
		});
	}

	std::string archive_reader_ascii::read_entry(std::string_view type) {
		auto line = input.read_line();
		line = line.substr(line.find('=') + 1);
		auto colon = line.find(':');

		if (line.substr(0, colon) != type) {
			throw parser_error(fmt::format("archive_reader_ascii: type mismatch: expected {}, got: {}.", type, line.substr(0, colon)));
		}

		auto rv = line.substr(colon + 1);
		return rv;
	}

	std::string archive_reader_ascii::read_string() {
		return read_entry("string");
	}

	s32 archive_reader_ascii::read_int() {
		return std::stoi(read_entry("int"));
	}

	float archive_reader_ascii::read_float() {
		return std::stof(read_entry("float"));
	}

	u8 archive_reader_ascii::read_byte() {
		return std::stoul(read_entry("byte")) & 0xFF;
	}

	u16 archive_reader_ascii::read_word() {
		return std::stoul(read_entry("word")) & 0xFF'FF;
	}

	u32 archive_reader_ascii::read_enum() {
		return std::stoul(read_entry("enum")) & 0xFFFF'FFFF;
	}

	bool archive_reader_ascii::read_bool() {
		return std::stoul(read_entry("bool")) == 1;
	}

	color archive_reader_ascii::read_color() {
		std::stringstream in {read_entry("color")};

		u16 r, g, b, a;
		in >> r >> g >> b >> a;
		return color {(u8) r, (u8) g, (u8) b, (u8) a};
	}

	glm::vec3 archive_reader_ascii::read_vec3() {
		std::stringstream in {read_entry("vec3")};
		glm::vec3 v {};

		in >> v.x >> v.y >> v.z;
		return v;
	}

	glm::vec2 archive_reader_ascii::read_vec2() {
		std::stringstream in {read_entry("rawFloat")};
		glm::vec2 v {};

		in >> v.x >> v.y;
		return v;
	}

	void archive_reader_ascii::skip_entry() {
		(void) input.read_line();
	}

	std::tuple<glm::vec3, glm::vec3> archive_reader_ascii::read_bbox() {
		std::stringstream in {read_entry("rawFloat")};
		glm::vec3 min {};
		glm::vec3 max {};

		in >> min.x >> min.y >> min.z >> max.x >> max.y >> max.z;
		return std::make_tuple(min, max);
	}

	glm::mat3x3 archive_reader_ascii::read_mat3x3() {
		auto in = read_entry("raw");

		if (in.length() < 2 /* 2 chars a byte */ * sizeof(float) * 9) {
			throw parser_error("archive_reader_ascii: raw entry does not contain enough bytes to be a 3x3 matrix");
		}

		auto beg_it = in.begin().base();

		glm::mat3x3 v {};
		u8 tmp[4];

		for (int i = 0; i < 9; ++i) {
			std::from_chars(beg_it + 0, beg_it + 2, tmp[0], 16);
			std::from_chars(beg_it + 2, beg_it + 4, tmp[1], 16);
			std::from_chars(beg_it + 4, beg_it + 6, tmp[2], 16);
			std::from_chars(beg_it + 6, beg_it + 8, tmp[3], 16);
			beg_it += 8;

			memcpy(&v[i / 3][i % 3], tmp, sizeof(float));
		}

		return v;
	}
}// namespace phoenix