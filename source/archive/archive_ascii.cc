// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "archive_ascii.hh"

#include <fmt/format.h>

#include <charconv>
#include <sstream>

namespace phoenix {
	void archive_reader_ascii::read_header() {
		{
			std::string objects = input.get_line();
			if (!objects.starts_with("objects ")) {
				throw parser_error {"archive_reader_ascii", "objects field missing"};
			}
			_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
		}

		if (input.get_line() != "END") {
			throw parser_error {"archive_reader_ascii", "second END missing"};
		}
	}

	bool archive_reader_ascii::read_object_begin(archive_object& obj) {
		if (input.remaining() < 3)
			return false;

		input.mark();
		auto line = input.get_line();

		if (!line.starts_with('[') || !line.ends_with(']') || line.length() <= 2) {
			input.reset();
			return false;
		}

		std::stringstream ss {line.substr(1, line.size() - 2)};
		ss >> obj.object_name >> obj.class_name >> obj.version >> obj.index;
		return true;
	}

	bool archive_reader_ascii::read_object_end() {
		if (input.remaining() < 3)
			return false;
		auto line = input.get_line();

		if (line != "[]") {
			input.reset();
			return false;
		}

		return true;
	}

	std::string archive_reader_ascii::read_entry(std::string_view type) {
		auto line = input.get_line();
		line = line.substr(line.find('=') + 1);
		auto colon = line.find(':');

		if (line.substr(0, colon) != type) {
			throw parser_error {"archive_reader_ascii",
			                    fmt::format("type mismatch: expected {}, got: {}", type, line.substr(0, colon))};
		}

		auto rv = line.substr(colon + 1);
		return rv;
	}

	std::string archive_reader_ascii::read_string() {
		return read_entry("string");
	}

	std::int32_t archive_reader_ascii::read_int() {
		return std::stoi(read_entry("int"));
	}

	float archive_reader_ascii::read_float() {
		return std::stof(read_entry("float"));
	}

	std::uint8_t archive_reader_ascii::read_byte() {
		return std::stoul(read_entry("byte")) & 0xFF;
	}

	std::uint16_t archive_reader_ascii::read_word() {
		return std::stoul(read_entry("word")) & 0xFF'FF;
	}

	std::uint32_t archive_reader_ascii::read_enum() {
		return std::stoul(read_entry("enum")) & 0xFFFF'FFFF;
	}

	bool archive_reader_ascii::read_bool() {
		return std::stoul(read_entry("bool")) == 1;
	}

	glm::u8vec4 archive_reader_ascii::read_color() {
		std::stringstream in {read_entry("color")};

		std::uint16_t r, g, b, a;
		in >> r >> g >> b >> a;
		return glm::u8vec4 {(std::uint8_t) r, (std::uint8_t) g, (std::uint8_t) b, (std::uint8_t) a};
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
		(void) input.get_line();
	}

	bounding_box archive_reader_ascii::read_bbox() {
		std::stringstream in {read_entry("rawFloat")};
		bounding_box box {};

		in >> box.min.x >> box.min.y >> box.min.z >> box.max.x >> box.max.y >> box.max.z;
		return box;
	}

	glm::mat3x3 archive_reader_ascii::read_mat3x3() {
		auto in = read_entry("raw");

		if (in.length() < 2 /* 2 chars a byte */ * sizeof(float) * 9) {
			throw parser_error {"archive_reader_ascii", "raw entry does not contain enough bytes to be a 3x3 matrix"};
		}

		auto beg_it = in.begin().base();

		glm::mat3x3 v {};
		std::uint8_t tmp[4];

		for (int i = 0; i < 9; ++i) {
			std::from_chars(beg_it + 0, beg_it + 2, tmp[0], 16);
			std::from_chars(beg_it + 2, beg_it + 4, tmp[1], 16);
			std::from_chars(beg_it + 4, beg_it + 6, tmp[2], 16);
			std::from_chars(beg_it + 6, beg_it + 8, tmp[3], 16);
			beg_it += 8;

			memcpy(&v[i / 3][i % 3], tmp, sizeof(float));
		}

		return glm::transpose(v);
	}

	buffer archive_reader_ascii::read_raw_bytes() {
		auto in = read_entry("raw");
		std::vector<std::byte> out {};
		out.resize(in.length() / 2);

		auto beg_it = in.begin().base();

		for (std::byte& i : out) {
			std::from_chars(beg_it + 0, beg_it + 2, reinterpret_cast<std::uint8_t&>(i), 16);
			beg_it += 2;
		}

		return buffer::of(std::move(out));
	}
} // namespace phoenix
