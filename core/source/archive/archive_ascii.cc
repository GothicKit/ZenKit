// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "archive_ascii.hh"

#include <fmt/format.h>
#include <phoenix/detail/error.hh>
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
}// namespace phoenix