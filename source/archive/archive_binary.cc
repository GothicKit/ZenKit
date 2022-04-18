// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "archive_binary.hh"

#include <phoenix/detail/error.hh>
#include <stdexcept>

namespace phoenix {
	void archive_reader_binary::read_header() {
		{
			std::string objects = input.get_line();
			if (!objects.starts_with("objects ")) {
				throw parser_error("not an archive: objects missing");
			}
			_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
		}

		if (input.get_line() != "END") {
			throw parser_error("not an archive: END(2) missing");
		}
	}

	bool archive_reader_binary::read_object_begin(archive_object& obj) {
		(void) input.get_uint(); // chunksize including itself
		obj.version = input.get_ushort();
		obj.index = input.get_uint();
		obj.object_name = input.get_line();
		obj.class_name = input.get_line();
		return true;
	}

	bool archive_reader_binary::read_object_end() {
		return true;
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
		return input.get() == 1;
	}

	glm::u8vec4 archive_reader_binary::read_color() {
		return glm::u8vec4 {input.get(), input.get(), input.get(), input.get()};
	}

	glm::vec3 archive_reader_binary::read_vec3() {
		return input.get_vec3();
	}

	glm::vec2 archive_reader_binary::read_vec2() {
		return input.get_vec2();
	}

	void archive_reader_binary::skip_entry() {}

	void archive_reader_binary::skip_object(bool skip_current) {
		// FIXME: skip_current is ignored here
		(void) skip_current;
		input.skip(input.get_uint());
	}

	bounding_box archive_reader_binary::read_bbox() {
		return bounding_box::parse(input);
	}

	glm::mat3x3 archive_reader_binary::read_mat3x3() {
		return input.get_mat3x3();
	}

	buffer archive_reader_binary::read_raw_bytes() {
		return input.slice();
	}
} // namespace phoenix
