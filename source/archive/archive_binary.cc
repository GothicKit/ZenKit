// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "archive_binary.hh"

#include <stdexcept>

namespace phoenix {
	void archive_reader_binary::read_header() {
		{
			std::string objects = input.get_line();
			if (!objects.starts_with("objects ")) {
				throw parser_error {"archive_reader_binary", "objects header field missing"};
			}
			_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
		}

		if (input.get_line() != "END") {
			throw parser_error {"archive_reader_binary", "second END missing"};
		}
	}

	bool archive_reader_binary::read_object_begin(archive_object& obj) {
		if (input.remaining() < 12)
			return false;

		auto pos = input.position();
		_m_object_end.push(pos + input.get_uint());

		obj.version = input.get_ushort();
		obj.index = input.get_uint();
		obj.object_name = input.get_line();
		obj.class_name = input.get_line();
		return true;
	}

	bool archive_reader_binary::read_object_end() {
		if (input.position() == _m_object_end.top()) {
			_m_object_end.pop();
			return true;
		}

		return false;
	}

	std::string archive_reader_binary::read_string() {
		return input.get_line(false);
	}

	std::int32_t archive_reader_binary::read_int() {
		return input.get_int();
	}

	float archive_reader_binary::read_float() {
		return input.get_float();
	}

	std::uint8_t archive_reader_binary::read_byte() {
		return input.get();
	}

	std::uint16_t archive_reader_binary::read_word() {
		return input.get_ushort();
	}

	std::uint32_t archive_reader_binary::read_enum() {
		return input.get_uint();
	}

	bool archive_reader_binary::read_bool() {
		return input.get() != 0;
	}

	glm::u8vec4 archive_reader_binary::read_color() {
		auto b = input.get();
		auto g = input.get();
		auto r = input.get();
		auto a = input.get();

		return {r, g, b, a};
	}

	glm::vec3 archive_reader_binary::read_vec3() {
		return input.get_vec3();
	}

	glm::vec2 archive_reader_binary::read_vec2() {
		return input.get_vec2();
	}

	void archive_reader_binary::skip_entry() {
		throw parser_error {"archive_reader", "cannot skip entry in binary archive"};
	}

	void archive_reader_binary::skip_object(bool skip_current) {
		if (skip_current) {
			input.position(_m_object_end.top());
			_m_object_end.pop();
		} else {
			input.skip(input.get_uint());
		}
	}

	bounding_box archive_reader_binary::read_bbox() {
		return bounding_box::parse(input);
	}

	glm::mat3x3 archive_reader_binary::read_mat3x3() {
		return glm::transpose(input.get_mat3x3());
	}

	buffer archive_reader_binary::read_raw_bytes() {
		return input.slice();
	}
} // namespace phoenix
