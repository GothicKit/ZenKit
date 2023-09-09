// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "ArchiveAscii.hh"

#include "phoenix/buffer.hh"

#include "../Internal.hh"

#include <charconv>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace zenkit {
	void ReadArchiveAscii::read_header() {
		{
			std::string objects = read->read_line(true);
			if (objects.find("objects ") != 0) {
				throw zenkit::ParserError {"ReadArchive.Ascii", "objects field missing"};
			}

			try {
				_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
			} catch (std::invalid_argument const& e) {
				throw zenkit::ParserError {"ReadArchive.Ascii", e, "reading int"};
			}
		}

		if (read->read_line(true) != "END") {
			throw zenkit::ParserError {"ReadArchive.Ascii", "second END missing"};
		}
	}

	bool ReadArchiveAscii::read_object_begin(ArchiveObject& obj) {
		if (read->eof())
			return false;

		auto mark = read->tell();
		auto line = read->read_line(true);

		// Fail quickly if we know this can't be an object begin
		if (line.length() <= 2) {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		char class_name[128];
		char object_name[128];

		auto parsed_elements =
		    std::sscanf(line.c_str(), "[%127s %127s %hu %u]", object_name, class_name, &obj.version, &obj.index);

		if (parsed_elements != 4) {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		obj.object_name = object_name;
		obj.class_name = class_name;
		return true;
	}

	bool ReadArchiveAscii::read_object_end() {
		// When there are less than 3 bytes left in the input, this must be the end of the archive.
		if (read->eof())
			return false;

		auto mark = read->tell();
		auto line = read->read_line(true);

		// Compatibility fix for binary data in ASCII archives.
		// TODO: Optimize using `find_if`!
		while (std::isspace(static_cast<unsigned char>(line[0]))) {
			line = line.substr(1);
		}

		if (line != "[]") {
			read->seek(static_cast<ssize_t>(mark), Whence::BEG);
			return false;
		}

		return true;
	}

	std::string ReadArchiveAscii::read_entry(std::string_view type) {
		auto line = read->read_line(true);
		line = line.substr(line.find('=') + 1);
		auto colon = line.find(':');

		if (line.substr(0, colon) != type) {
			throw zenkit::ParserError {"ReadArchive.Ascii",
			                           "type mismatch: expected " + std::string {type} +
			                               ", got: " + line.substr(0, colon)};
		}

		return line.substr(colon + 1);
	}

	std::string ReadArchiveAscii::read_string() {
		return read_entry("string");
	}

	std::int32_t ReadArchiveAscii::read_int() {
		try {
			return std::stoi(read_entry("int"));
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	float ReadArchiveAscii::read_float() {
		try {
			return std::stof(read_entry("float"));
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	std::uint8_t ReadArchiveAscii::read_byte() {
		try {
			return std::stoul(read_entry("int")) & 0xFF;
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	std::uint16_t ReadArchiveAscii::read_word() {
		try {
			return std::stoul(read_entry("int")) & 0xFF'FF;
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	std::uint32_t ReadArchiveAscii::read_enum() {
		try {
			return std::stoul(read_entry("enum")) & 0xFFFF'FFFF;
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	bool ReadArchiveAscii::read_bool() {
		try {
			return std::stoul(read_entry("bool")) != 0;
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	glm::u8vec4 ReadArchiveAscii::read_color() {
		std::stringstream in {read_entry("color")};

		std::uint16_t r, g, b, a;
		in >> r >> g >> b >> a;
		return glm::u8vec4 {(std::uint8_t) r, (std::uint8_t) g, (std::uint8_t) b, (std::uint8_t) a};
	}

	glm::vec3 ReadArchiveAscii::read_vec3() {
		std::stringstream in {read_entry("vec3")};
		glm::vec3 v {};

		in >> v.x >> v.y >> v.z;
		return v;
	}

	glm::vec2 ReadArchiveAscii::read_vec2() {
		std::stringstream in {read_entry("rawFloat")};
		glm::vec2 v {};

		in >> v.x >> v.y;
		return v;
	}

	void ReadArchiveAscii::skip_entry() {
		(void) read->read_line(true);
	}

	AxisAlignedBoundingBox ReadArchiveAscii::read_bbox() {
		std::stringstream in {read_entry("rawFloat")};
		AxisAlignedBoundingBox box {};

		in >> box.min.x >> box.min.y >> box.min.z >> box.max.x >> box.max.y >> box.max.z;
		return box;
	}

	glm::mat3x3 ReadArchiveAscii::read_mat3x3() {
		auto in = read_entry("raw");

		if (in.length() < 2 /* 2 chars a byte */ * sizeof(float) * 9) {
			throw zenkit::ParserError {"ReadArchive.Ascii",
			                           "raw entry does not contain enough bytes to be a 3x3 matrix"};
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

	phoenix::buffer ReadArchiveAscii::read_raw_bytes(uint32_t size) {
		auto in = read_entry("raw");
		auto length = in.length() / 2;

		if (length < size) {
			throw zenkit::ParserError {"ReadArchive.Ascii", "not enough raw bytes to read!"};
		} else if (length > size) {
			ZKLOGW("ReadArchive.Ascii", "Reading %d bytes although %zu are actually available", size, length);
		}

		std::vector<std::byte> out {};
		out.resize(length);

		auto beg_it = in.data();

		for (std::byte& i : out) {
			std::from_chars(beg_it + 0, beg_it + 2, reinterpret_cast<std::uint8_t&>(i), 16);
			beg_it += 2;
		}

		return phoenix::buffer::of(std::move(out));
	}

	std::unique_ptr<Read> ReadArchiveAscii::read_raw(uint32_t size) {
		auto in = read_entry("raw");
		auto length = in.length() / 2;

		if (length < size) {
			throw zenkit::ParserError {"ReadArchive.Ascii", "not enough raw bytes to read!"};
		} else if (length > size) {
			ZKLOGW("ReadArchive.Ascii", "Reading %d bytes although %zu are actually available", size, length);
		}

		std::vector<std::byte> out {};
		out.resize(length);

		auto beg_it = in.data();

		for (std::byte& i : out) {
			std::from_chars(beg_it + 0, beg_it + 2, reinterpret_cast<std::uint8_t&>(i), 16);
			beg_it += 2;
		}

		return Read::from(std::move(out));
	}
} // namespace zenkit
