// Copyright 2021 Luis Michaelis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
// Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

	int32_t archive_reader_binary::read_int() {
		return input.read_s32();
	}

	float archive_reader_binary::read_float() {
		return input.read_f32();
	}

	uint8_t archive_reader_binary::read_byte() {
		return input.read_u8();
	}

	uint16_t archive_reader_binary::read_word() {
		return input.read_u16();
	}

	uint32_t archive_reader_binary::read_enum() {
		return input.read_u32();
	}

	bool archive_reader_binary::read_bool() {
		return input.read_u8() == 1;
	}

	color archive_reader_binary::read_color() {
		return color {input.read_u8(), input.read_u8(), input.read_u8(), input.read_u8()};
	}

	glm::vec3 archive_reader_binary::read_vec3() {
		return glm::vec3 {input.read_f32(), input.read_f32(), input.read_f32()};
	}

	glm::vec2 archive_reader_binary::read_vec2() {
		return glm::vec2 {input.read_f32(), input.read_f32()};
	}
}// namespace phoenix