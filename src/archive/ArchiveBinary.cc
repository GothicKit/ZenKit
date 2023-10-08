// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "ArchiveBinary.hh"

#include "phoenix/buffer.hh"
#include "phoenix/phoenix.hh"

#include <stdexcept>

namespace zenkit {
	void ReadArchiveBinary::read_header() {
		{
			std::string objects = read->read_line(true);
			if (objects.find("objects ") != 0) {
				throw zenkit::ParserError {"ReadArchiveBinary", "objects header field missing"};
			}

			try {
				_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
			} catch (std::invalid_argument const& e) {
				throw zenkit::ParserError {"ReadArchiveBinary", e, "reading int"};
			}
		}

		if (read->read_line_then_ignore("\n") != "END") {
			throw zenkit::ParserError {"ReadArchiveBinary", "second END missing"};
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

	phoenix::buffer ReadArchiveBinary::read_raw_bytes(uint32_t size) {
		std::vector<std::byte> bytes(size, std::byte {});
		read->read(bytes.data(), bytes.size());
		return phoenix::buffer::of(std::move(bytes));
	}

	std::unique_ptr<Read> ReadArchiveBinary::read_raw(std::size_t size) {
		std::vector<std::byte> bytes(size, std::byte {});
		read->read(bytes.data(), bytes.size());
		return Read::from(std::move(bytes));
	}

	void ReadArchiveBinary::skip_entry() {
		throw zenkit::ParserError {"archive_reader", "cannot skip entry in binary archive"};
	}

	void ReadArchiveBinary::skip_object(bool skip_current) {
		if (skip_current) {
			read->seek(static_cast<ssize_t>(_m_object_end.top()), Whence::BEG);
			_m_object_end.pop();
		} else {
			read->seek(read->read_uint() - 4, Whence::CUR);
		}
	}
} // namespace zenkit
