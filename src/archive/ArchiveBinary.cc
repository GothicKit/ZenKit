// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "ArchiveBinary.hh"
#include "zenkit/Error.hh"

#include <glm/gtc/type_ptr.hpp>

#include <charconv>
#include <stdexcept>

namespace zenkit {
	void ReadArchiveBinary::read_header() {
		{
			std::string objects = read->read_line(true);
			if (objects.find("objects ") != 0) {
				throw ParserError {"ReadArchiveBinary", "objects header field missing"};
			}

			try {
				_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
			} catch (std::invalid_argument const& e) {
				throw ParserError {"ReadArchiveBinary", e, "reading int"};
			}
		}

		if (read->read_line_then_ignore("\n") != "END") {
			throw ParserError {"ReadArchiveBinary", "second END missing"};
		}
	}

	bool ReadArchiveBinary::read_object_begin(ArchiveObject& obj) {
		if (read->eof()) return false;

		auto pos = read->tell();
		_m_object_end.push(pos + read->read_uint());

		obj.version = read->read_ushort();
		obj.index = read->read_uint();
		obj.object_name = read->read_line(false);
		obj.class_name = read->read_line(false);
		return true;
	}

	bool ReadArchiveBinary::read_object_end() {
		if (read->tell() == _m_object_end.top()) {
			_m_object_end.pop();
			return true;
		}

		return read->eof();
	}

	std::string ReadArchiveBinary::read_string() {
		return read->read_line(false);
	}

	std::int32_t ReadArchiveBinary::read_int() {
		return read->read_int();
	}

	float ReadArchiveBinary::read_float() {
		return read->read_float();
	}

	std::uint8_t ReadArchiveBinary::read_byte() {
		return read->read_ubyte();
	}

	std::uint16_t ReadArchiveBinary::read_word() {
		return read->read_ushort();
	}

	std::uint32_t ReadArchiveBinary::read_enum() {
		return read->read_ubyte();
	}

	bool ReadArchiveBinary::read_bool() {
		return read->read_ubyte() != 0;
	}

	glm::u8vec4 ReadArchiveBinary::read_color() {
		auto b = read->read_ubyte();
		auto g = read->read_ubyte();
		auto r = read->read_ubyte();
		auto a = read->read_ubyte();

		return {r, g, b, a};
	}

	glm::vec3 ReadArchiveBinary::read_vec3() {
		return read->read_vec3();
	}

	glm::vec2 ReadArchiveBinary::read_vec2() {
		return read->read_vec2();
	}

	AxisAlignedBoundingBox ReadArchiveBinary::read_bbox() {
		AxisAlignedBoundingBox aabb {};
		aabb.load(read);
		return aabb;
	}

	glm::mat3x3 ReadArchiveBinary::read_mat3x3() {
		return read->read_mat3();
	}

	std::unique_ptr<Read> ReadArchiveBinary::read_raw(std::size_t size) {
		std::vector bytes(size, std::byte {});
		read->read(bytes.data(), bytes.size());
		return Read::from(std::move(bytes));
	}

	void ReadArchiveBinary::skip_entry() {
		throw ParserError {"archive_reader", "cannot skip entry in binary archive"};
	}

	void ReadArchiveBinary::skip_object(bool skip_current) {
		if (skip_current) {
			read->seek(static_cast<ssize_t>(_m_object_end.top()), Whence::BEG);
			_m_object_end.pop();
		} else {
			read->seek(read->read_uint() - 4, Whence::CUR);
		}
	}

	WriteArchiveBinary::WriteArchiveBinary(Write* w) : _m_write(w) {
		this->_m_head = this->_m_write->tell();
		this->write_header();
	}

	std::uint32_t WriteArchiveBinary::write_object_begin(std::string_view object_name,
	                                                     std::string_view class_name,
	                                                     std::uint16_t version) {
		_m_objects.push(this->_m_write->tell());
		bool empty_class = class_name.empty() || class_name == "%";

		this->_m_write->write_uint(static_cast<uint32_t>(6 + object_name.length() + class_name.length() + 2));
		this->_m_write->write_ushort(version);
		this->_m_write->write_uint(empty_class ? 0 : _m_index);
		this->_m_write->write_string0(object_name);
		this->_m_write->write_string0(class_name);

		return empty_class ? 0 : _m_index++;
	}

	void WriteArchiveBinary::write_object_end() {
		auto cur = this->_m_write->tell();

		auto prev = this->_m_objects.top();
		this->_m_objects.pop();

		this->_m_write->seek(static_cast<ssize_t>(prev), Whence::BEG);
		this->_m_write->write_uint(static_cast<uint32_t>(cur - prev));
		this->_m_write->seek(static_cast<ssize_t>(cur), Whence::BEG);
	}

	void WriteArchiveBinary::write_ref(std::string_view object_name, uint32_t index) {
		auto start = this->_m_write->tell();

		this->_m_write->write_uint(static_cast<uint32_t>(6 + object_name.length() + 1 + 2));
		this->_m_write->write_ushort(0);
		this->_m_write->write_uint(index);
		this->_m_write->write_string0(object_name);
		this->_m_write->write_string0("\xA7");

		auto cur = this->_m_write->tell();
		this->_m_write->seek(static_cast<ssize_t>(start), Whence::BEG);
		this->_m_write->write_uint(static_cast<uint32_t>(cur - start));
		this->_m_write->seek(static_cast<ssize_t>(cur), Whence::BEG);
	}

	void WriteArchiveBinary::write_string(std::string_view, std::string_view v) {
		this->_m_write->write_string0(v);
	}

	void WriteArchiveBinary::write_int(std::string_view, std::int32_t v) {
		this->_m_write->write_int(v);
	}

	void WriteArchiveBinary::write_float(std::string_view, float v) {
		this->_m_write->write_float(v);
	}

	void WriteArchiveBinary::write_byte(std::string_view, std::uint8_t v) {
		this->_m_write->write_ubyte(v);
	}

	void WriteArchiveBinary::write_word(std::string_view, std::uint16_t v) {
		this->_m_write->write_ushort(v);
	}

	void WriteArchiveBinary::write_enum(std::string_view, std::uint32_t v) {
		this->_m_write->write_ubyte(static_cast<uint8_t>(v));
	}

	void WriteArchiveBinary::write_bool(std::string_view, bool v) {
		this->_m_write->write_ubyte(v ? 1 : 0);
	}

	void WriteArchiveBinary::write_color(std::string_view, glm::u8vec4 v) {
		this->_m_write->write_ubyte(v.b);
		this->_m_write->write_ubyte(v.g);
		this->_m_write->write_ubyte(v.r);
		this->_m_write->write_ubyte(v.a);
	}

	void WriteArchiveBinary::write_vec3(std::string_view, glm::vec3 const& v) {
		this->_m_write->write(glm::value_ptr(v), 3 * sizeof(float));
	}

	void WriteArchiveBinary::write_vec2(std::string_view, glm::vec2 v) {
		this->_m_write->write_vec2(v);
	}

	void WriteArchiveBinary::write_bbox(std::string_view, AxisAlignedBoundingBox const& v) {
		v.save(this->_m_write);
	}

	void WriteArchiveBinary::write_mat3x3(std::string_view, glm::mat3x3 const& v) {
		this->_m_write->write_mat3(v);
	}

	void WriteArchiveBinary::write_raw(std::string_view, std::vector<std::byte> const& v) {
		this->_m_write->write(v.data(), v.size());
	}

	void WriteArchiveBinary::write_raw(std::string_view, std::byte const* v, std::uint16_t length) {
		this->_m_write->write(v, length);
	}

	void WriteArchiveBinary::write_raw_float(std::string_view, float const* v, std::uint16_t length) {
		this->_m_write->write(v, length * 4);
	}

	void WriteArchiveBinary::write_header() {
		auto cur = this->_m_write->tell();
		this->_m_write->seek(static_cast<ssize_t>(_m_head), Whence::BEG);

		char const* username = getenv("USER");
		if (username == nullptr) username = getenv("USERNAME");
		if (username == nullptr) username = "Anonymous";

		char date_buffer[20];
		std::time_t clk;
		std::time(&clk);
		auto* time = std::localtime(&clk);
		strftime(date_buffer, 20, "%d.%m.%Y %H:%M:%S", time);

		this->_m_write->write_line("ZenGin Archive");
		this->_m_write->write_line("ver 1");
		this->_m_write->write_line("zCArchiverGeneric");
		this->_m_write->write_line("BINARY");
		this->_m_write->write_line(is_save_game() ? "saveGame 1" : "saveGame 0");
		this->_m_write->write_string("date ");
		this->_m_write->write_line(date_buffer);
		this->_m_write->write_string("user ");
		this->_m_write->write_line(username);
		this->_m_write->write_line("END");

		memset(date_buffer, ' ', 20);
		date_buffer[10] = '\0';
		std::to_chars(date_buffer, date_buffer + 9, _m_index);

		this->_m_write->write_string("objects ");
		this->_m_write->write_line(date_buffer);
		this->_m_write->write_line("END\n");

		if (cur != _m_head) {
			this->_m_write->seek(static_cast<ssize_t>(cur), Whence::BEG);
		}
	}
} // namespace zenkit
