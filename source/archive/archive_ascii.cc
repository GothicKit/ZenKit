// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "archive_ascii.hh"

#include <charconv>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace phoenix {
	static const std::unordered_map<std::string, archive_entry_type> type_name_to_enum {
	    {"string", archive_entry_type::string},
	    {"int", archive_entry_type::int_},
	    {"float", archive_entry_type::float_},
	    {"byte", archive_entry_type::byte},
	    {"word", archive_entry_type::word},
	    {"bool_", archive_entry_type::bool_},
	    {"vec3", archive_entry_type::vec3},
	    {"color", archive_entry_type::color},
	    {"raw", archive_entry_type::raw},
	    {"rawFloat", archive_entry_type::raw_float},
	    {"enum", archive_entry_type::enum_},
	    {"hash", archive_entry_type::hash},
	};

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

	bool archive_reader_ascii::read_object_end() {
		// When there are less than 3 bytes left in the input, this must be the end of the archive.
		if (input.remaining() < 3)
			return true;

		input.mark();
		auto line = input.get_line();

		// Compatibility fix for binary data in ASCII archives.
		// TODO: Optimize using `find_if`!
		while (std::isspace(static_cast<unsigned char>(line[0]))) {
			line = line.substr(1);
		}

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
			                    "type mismatch: expected " + std::string {type} + ", got: " + line.substr(0, colon)};
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
		return std::stoul(read_entry("int")) & 0xFF;
	}

	std::uint16_t archive_reader_ascii::read_word() {
		return std::stoul(read_entry("int")) & 0xFF'FF;
	}

	std::uint32_t archive_reader_ascii::read_enum() {
		return std::stoul(read_entry("enum")) & 0xFFFF'FFFF;
	}

	bool archive_reader_ascii::read_bool() {
		return std::stoul(read_entry("bool")) != 0;
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

		auto beg_it = in.data();

		glm::mat3x3 v {};
		std::uint8_t tmp[4];

		for (int32_t i = 0; i < 9; ++i) {
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
		auto length = in.length() / 2;

		std::vector<std::byte> out {};
		out.resize(length);

		auto beg_it = in.data();

		for (std::byte& i : out) {
			std::from_chars(beg_it + 0, beg_it + 2, reinterpret_cast<std::uint8_t&>(i), 16);
			beg_it += 2;
		}

		return buffer::of(std::move(out));
	}

	buffer archive_reader_ascii::read_raw_bytes(uint32_t size) {
		auto in = read_entry("raw");
		auto length = in.length() / 2;

		if (length < size) {
			throw parser_error {"archive_reader_ascii", "not enough raw bytes to read!"};
		} else if (length > size) {
			PX_LOGW("read_raw_bytes: reading ", size, " bytes although ", length, " are actually available");
		}

		std::vector<std::byte> out {};
		out.resize(length);

		auto beg_it = in.data();

		for (std::byte& i : out) {
			std::from_chars(beg_it + 0, beg_it + 2, reinterpret_cast<std::uint8_t&>(i), 16);
			beg_it += 2;
		}

		return buffer::of(std::move(out));
	}

	std::variant<archive_object, archive_object_end, archive_entry> archive_reader_ascii::unstable__next() {
		static archive_object tmp {};
		if (read_object_begin(tmp)) {
			return tmp;
		} else if (read_object_end()) {
			return archive_object_end {};
		} else {
			input.mark();

			archive_entry entry {};

			auto line = input.get_line();
			entry.name = line.substr(line.find('='));

			line = line.substr(line.find('=') + 1);
			entry.type = type_name_to_enum.at(line.substr(0, line.find(':')));

			input.reset();

			switch (entry.type) {
			case archive_entry_type::string:
				entry.value = read_string();
				break;
			case archive_entry_type::int_:
				entry.value = read_int();
				break;
			case archive_entry_type::float_:
				entry.value = read_float();
				break;
			case archive_entry_type::byte:
				entry.value = read_byte();
				break;
			case archive_entry_type::word:
				entry.value = read_word();
				break;
			case archive_entry_type::bool_:
				entry.value = read_bool();
				break;
			case archive_entry_type::vec3:
				entry.value = read_vec3();
				break;
			case archive_entry_type::color:
				entry.value = read_color();
				break;
			case archive_entry_type::raw:
			case archive_entry_type::raw_float:
				entry.value = read_raw_bytes();
				break;
			case archive_entry_type::enum_:
				entry.value = read_enum();
				break;
			case archive_entry_type::hash:
				entry.value = 0u;
				break;
			}

			return entry;
		}
	}
} // namespace phoenix
