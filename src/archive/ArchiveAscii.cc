// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "ArchiveAscii.hh"
#include "zenkit/Error.hh"

#include "../Internal.hh"

#include <array>
#include <charconv>
#include <cstring>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace zenkit {
	void ReadArchiveAscii::read_header() {
		{
			std::string objects = read->read_line(true);
			if (objects.find("objects ") != 0) {
				throw ParserError {"ReadArchive.Ascii", "objects field missing"};
			}

			try {
				_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
			} catch (std::invalid_argument const& e) {
				throw ParserError {"ReadArchive.Ascii", e, "reading int"};
			}
		}

		if (read->read_line(true) != "END") {
			throw ParserError {"ReadArchive.Ascii", "second END missing"};
		}
	}

	bool ReadArchiveAscii::read_object_begin(ArchiveObject& obj) {
		if (read->eof()) return false;

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
		if (read->eof()) return false;

		auto mark = read->tell();
		auto line = read->read_line(true);

		// Compatibility fix for binary data in ASCII archives.
		std::string_view view = line;
		size_t spaces_count = 0;
		for (; spaces_count < view.size() && std::isspace(static_cast<unsigned char>(view[spaces_count]));
		     ++spaces_count)
			;

		if (spaces_count > 0) {
			view.remove_prefix(spaces_count);
		}

		if (view != "[]") {
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
			throw ParserError {"ReadArchive.Ascii",
			                   "type mismatch: expected " + std::string {type} + ", got: " + line.substr(0, colon)};
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
			throw ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	float ReadArchiveAscii::read_float() {
		try {
			return std::stof(read_entry("float"));
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	std::uint8_t ReadArchiveAscii::read_byte() {
		try {
			return std::stoul(read_entry("int")) & 0xFF;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	std::uint16_t ReadArchiveAscii::read_word() {
		try {
			return std::stoul(read_entry("int")) & 0xFF'FF;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	std::uint32_t ReadArchiveAscii::read_enum() {
		try {
			return std::stoul(read_entry("enum")) & 0xFFFF'FFFF;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	bool ReadArchiveAscii::read_bool() {
		try {
			return std::stoul(read_entry("bool")) != 0;
		} catch (std::invalid_argument const& e) {
			throw ParserError {"ReadArchive.Ascii", e, "reading int"};
		}
	}

	Color ReadArchiveAscii::read_color() {
		std::stringstream in {read_entry("color")};

		std::uint16_t r, g, b, a;
		in >> r >> g >> b >> a;
		return Color {static_cast<std::uint8_t>(r),
		              static_cast<std::uint8_t>(g),
		              static_cast<std::uint8_t>(b),
		              static_cast<std::uint8_t>(a)};
	}

	Vec3 ReadArchiveAscii::read_vec3() {
		std::stringstream in {read_entry("vec3")};
		Vec3 v {};

		in >> v.x >> v.y >> v.z;
		return v;
	}

	Vec2 ReadArchiveAscii::read_vec2() {
		std::stringstream in {read_entry("rawFloat")};
		Vec2 v {};

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

	Mat3 ReadArchiveAscii::read_mat3x3() {
		auto in = read_entry("raw");

		if (in.length() < 2 /* 2 chars a byte */ * sizeof(float) * 9) {
			throw ParserError {"ReadArchive.Ascii", "raw entry does not contain enough bytes to be a 3x3 matrix"};
		}

		auto beg_it = in.data();

		Mat3 v {};
		for (int32_t i = 0; i < 9; ++i) {
			std::uint8_t tmp[4];
			std::from_chars(beg_it + 0, beg_it + 2, tmp[0], 16);
			std::from_chars(beg_it + 2, beg_it + 4, tmp[1], 16);
			std::from_chars(beg_it + 4, beg_it + 6, tmp[2], 16);
			std::from_chars(beg_it + 6, beg_it + 8, tmp[3], 16);
			beg_it += 8;

			memcpy(&v[i / 3][i % 3], tmp, sizeof(float));
		}

		return v.transpose();
	}

	std::unique_ptr<Read> ReadArchiveAscii::read_raw(std::size_t size) {
		auto in = read_entry("raw");
		auto length = in.length() / 2;

		if (length < size) {
			throw ParserError {"ReadArchive.Ascii", "not enough raw bytes to read!"};
		}

		if (length > size) {
			ZKLOGW("ReadArchive.Ascii", "Reading %zu bytes although %zu are actually available", size, length);
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

	WriteArchiveAscii::WriteArchiveAscii(Write* w) : _m_write(w) {
		this->_m_head = this->_m_write->tell();
		this->write_header();
	}

	template <typename T, size_t N>
	std::enable_if_t<!std::is_floating_point_v<T>, std::string_view> intosv(std::array<char, N>& buf, T v) {
		auto r = std::to_chars(buf.data(), buf.data() + buf.size(), v);
		return std::string_view {buf.data(), static_cast<size_t>(r.ptr - buf.data())};
	}

	std::uint32_t WriteArchiveAscii::write_object_begin(std::string_view object_name,
	                                                    std::string_view class_name,
	                                                    std::uint16_t version) {
		this->write_indent();

		std::array<char, std::numeric_limits<uint32_t>::digits10 + 1> buf {};
		bool empty_class = class_name.empty() || class_name == "%";

		this->_m_write->write_char('[');
		this->_m_write->write_string(object_name);
		this->_m_write->write_char(' ');
		this->_m_write->write_string(empty_class ? "%" : class_name.data());
		this->_m_write->write_char(' ');
		this->_m_write->write_string(intosv(buf, version));
		this->_m_write->write_char(' ');
		this->_m_write->write_string(intosv(buf, empty_class ? 0 : _m_index));
		this->_m_write->write_char(']');
		this->_m_write->write_char('\n');

		this->_m_indent++;
		return empty_class ? 0 : _m_index++;
	}

	void WriteArchiveAscii::write_object_end() {
		this->_m_indent--;
		this->write_indent();

		this->_m_write->write_line("[]");
	}

	void WriteArchiveAscii::write_ref(std::string_view object_name, uint32_t index) {
		this->write_indent();

		std::array<char, std::numeric_limits<uint32_t>::digits10 + 1> buf {};

		this->_m_write->write_char('[');
		this->_m_write->write_string(object_name);
		this->_m_write->write_string(" \xA7 0 ");
		this->_m_write->write_string(intosv(buf, index));
		this->_m_write->write_char(']');
		this->_m_write->write_char('\n');

		this->write_indent();
		this->_m_write->write_line("[]");
	}

	void WriteArchiveAscii::write_string(std::string_view name, std::string_view v) {
		this->write_entry(name, "string", v);
	}

	void WriteArchiveAscii::write_int(std::string_view name, std::int32_t v) {
		std::array<char, std::numeric_limits<int32_t>::digits10 + 1> buf {};
		this->write_entry(name, "int", intosv(buf, v));
	}

	void WriteArchiveAscii::write_float(std::string_view name, float v) {
		this->write_entry(name, "float", std::to_string(v));
	}

	void WriteArchiveAscii::write_byte(std::string_view name, std::uint8_t v) {
		std::array<char, std::numeric_limits<uint8_t>::digits10 + 1> buf {};
		this->write_entry(name, "byte", intosv(buf, v));
	}

	void WriteArchiveAscii::write_word(std::string_view name, std::uint16_t v) {
		std::array<char, std::numeric_limits<uint16_t>::digits10 + 1> buf {};
		this->write_entry(name, "word", intosv(buf, v));
	}

	void WriteArchiveAscii::write_enum(std::string_view name, std::uint32_t v) {
		std::array<char, std::numeric_limits<uint32_t>::digits10 + 1> buf {};
		this->write_entry(name, "enum", intosv(buf, v));
	}

	void WriteArchiveAscii::write_bool(std::string_view name, bool v) {
		this->write_entry(name, "bool", v ? "1" : "0");
	}

	void WriteArchiveAscii::write_color(std::string_view name, Color v) {
		std::array<char, (std::numeric_limits<uint8_t>::digits10 + 2) * 4> buf {};

		auto n = std::snprintf(buf.data(), buf.size(), "%d %d %d %d", v.r, v.g, v.b, v.a);

		std::string_view s(buf.data(), static_cast<size_t>(n));
		this->write_entry(name, "color", s);
	}

	void WriteArchiveAscii::write_vec3(std::string_view name, Vec3 const& v) {
		std::array<char,
		           (std::numeric_limits<float>::max_exponent10 + std::numeric_limits<float>::max_digits10 + 3) * 3>
		    buf {};
		auto n = std::snprintf(buf.data(), buf.size(), "%f %f %f", v.x, v.y, v.z);

		std::string_view s(buf.data(), static_cast<size_t>(n));
		this->write_entry(name, "vec3", s);
	}

	void WriteArchiveAscii::write_vec2(std::string_view name, Vec2 v) {
		this->write_raw_float(name, v.pointer(), 2);
	}

	void WriteArchiveAscii::write_bbox(std::string_view name, AxisAlignedBoundingBox const& v) {
		float values[] {v.min.x, v.min.y, v.min.z, v.max.x, v.max.y, v.max.z};
		this->write_raw_float(name, values, 6);
	}

	void WriteArchiveAscii::write_mat3x3(std::string_view name, Mat3 const& v) {
		auto vT = v.transpose();
		this->write_raw_float(name, vT.pointer(), 9);
	}

	void WriteArchiveAscii::write_raw(std::string_view name, std::vector<std::byte> const& v) {
		this->write_raw(name, v.data(), static_cast<uint16_t>(v.size()));
	}

	void WriteArchiveAscii::write_raw(std::string_view name, std::byte const* v, std::uint16_t length) {
		this->write_indent();
		this->_m_write->write_string(name);
		this->_m_write->write_string("=raw:");

		std::array<char, 3> buf {};
		buf[2] = '\0';
		for (auto i = 0u; i < length; ++i) {
			std::to_chars(buf.data(), buf.data() + buf.size(), static_cast<unsigned char>(v[i]), 16);

			if (buf[1] == '\0') {
				this->_m_write->write_char('0');
				this->_m_write->write_char(buf[0]);
			} else {
				this->_m_write->write_string(buf.data());
			}
		}

		this->_m_write->write_char('\n');
	}

	void WriteArchiveAscii::write_raw_float(std::string_view name, float const* v, std::uint16_t length) {
		this->write_indent();
		this->_m_write->write_string(name);
		this->_m_write->write_string("=rawFloat:");

		for (auto i = 0u; i < length; ++i) {
			this->_m_write->write_string(std::to_string(v[i]));
			this->_m_write->write_char(' ');
		}

		this->_m_write->write_char('\n');
	}

	void WriteArchiveAscii::write_entry(std::string_view name, std::string_view type, std::string_view value) {
		this->write_indent();
		this->_m_write->write_string(name);
		this->_m_write->write_char('=');
		this->_m_write->write_string(type);
		this->_m_write->write_char(':');
		this->_m_write->write_line(value);
	}

	void WriteArchiveAscii::write_indent() {
		for (auto i = 0u; i < _m_indent; ++i) {
			_m_write->write_char('\t');
		}
	}

	void WriteArchiveAscii::write_header() {
		auto cur = this->_m_write->tell();
		this->_m_write->seek(static_cast<ssize_t>(this->_m_head), Whence::BEG);

		char const* username = getenv("USER");
		if (username == nullptr) username = getenv("USERNAME");
		if (username == nullptr) username = "Anonymous";

		char date_buffer[20];
		std::time_t clk;
		std::time(&clk);
		auto* time = std::localtime(&clk);
		strftime(date_buffer, 20, "%-d.%-m.%Y %H:%M:%S", time);

		this->_m_write->write_line("ZenGin Archive");
		this->_m_write->write_line("ver 1");
		this->_m_write->write_line("zCArchiverGeneric");
		this->_m_write->write_line("ASCII");
		this->_m_write->write_line(is_save_game() ? "saveGame 1" : "saveGame 0");
		this->_m_write->write_string("date ");
		this->_m_write->write_line(date_buffer);
		this->_m_write->write_string("user ");
		this->_m_write->write_line(username);
		this->_m_write->write_line("END");

		memset(date_buffer, ' ', 20);
		date_buffer[11] = '\0';
		std::to_chars(date_buffer, date_buffer + 10, _m_index);

		this->_m_write->write_string("objects ");
		this->_m_write->write_line(date_buffer);
		this->_m_write->write_line("END\n");

		if (cur != _m_head) {
			this->_m_write->seek(static_cast<ssize_t>(cur), Whence::BEG);
		}
	}
} // namespace zenkit
