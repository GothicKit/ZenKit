// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "archive_binary.hh"

#include <phoenix/detail/error.hh>
#include <stdexcept>

namespace phoenix {
	void archive_reader_binary::read_header() {
		{
			std::string objects = input.read_line();
			if (!objects.starts_with("objects ")) { throw parser_error("not an archive: objects missing"); }
			_m_objects = std::stoi(objects.substr(objects.find(' ') + 1));
		}

		if (input.read_line(true) != "END") {
			throw parser_error("not an archive: END(2) missing");
		}
	}

	bool archive_reader_binary::read_object_begin(archive_object& obj) {
		(void) input.read_u32();// chunksize including itself
		obj.version = input.read_u16();
		obj.index = input.read_u32();
		obj.object_name = input.read_line(true);
		obj.class_name = input.read_line(true);
		return true;
	}

	bool archive_reader_binary::read_object_end() {
		return true;
	}

	std::string archive_reader_binary::read_string() {
		return input.read_line(false);
	}

	s32 archive_reader_binary::read_int() {
		return input.read_s32();
	}

	float archive_reader_binary::read_float() {
		return input.read_f32();
	}

	u8 archive_reader_binary::read_byte() {
		return input.read_u8();
	}

	u16 archive_reader_binary::read_word() {
		return input.read_u16();
	}

	u32 archive_reader_binary::read_enum() {
		return input.read_u32();
	}

	bool archive_reader_binary::read_bool() {
		return input.read_u8() == 1;
	}

	color archive_reader_binary::read_color() {
		return color {input.read_u8(), input.read_u8(), input.read_u8(), input.read_u8()};
	}

	glm::vec3 archive_reader_binary::read_vec3() {
		return input.read_vec3();
	}

	glm::vec2 archive_reader_binary::read_vec2() {
		return input.read_vec2();
	}
}// namespace phoenix