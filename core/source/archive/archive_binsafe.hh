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
#pragma once
#include <phoenix/archive.hh>

namespace phoenix {
	constexpr uint8_t type_sizes[] = {
			0,                  // ?            = 0x00
			0,                  // bs_string    = 0x01,
			sizeof(uint32_t),   // bs_int       = 0x02,
			sizeof(float),      // bs_float     = 0x03,
			sizeof(uint8_t),    // bs_byte      = 0x04,
			sizeof(uint16_t),   // bs_word      = 0x05,
			sizeof(uint32_t),   // bs_bool      = 0x06,
			sizeof(float) * 3,  // bs_vec3      = 0x07,
			sizeof(uint8_t) * 4,// bs_color     = 0x08,
			0,                  // bs_raw       = 0x09,
			0,                  // ?            = 0x0A
			0,                  // ?            = 0x0B
			0,                  // ?            = 0x0C
			0,                  // ?            = 0x0D
			0,                  // ?            = 0x0E
			0,                  // ?            = 0x0F
			0,                  // bs_raw_float = 0x10,
			sizeof(uint32_t),   // bs_enum      = 0x11,
			sizeof(uint32_t),   // bs_hash      = 0x12,
	};

	enum archive_binsafe_type {
		bs_string = 0x1,
		bs_int = 0x2,
		bs_float = 0x3,
		bs_byte = 0x4,
		bs_word = 0x5,
		bs_bool = 0x6,
		bs_vec3 = 0x7,
		bs_color = 0x8,
		bs_raw = 0x9,
		bs_raw_float = 0x10,
		bs_enum = 0x11,
		bs_hash = 0x12,
	};


	class archive_reader_binsafe final : public archive_reader {
	public:
		inline archive_reader_binsafe(const reader& in, archive_header&& header) : archive_reader(in, std::move(header)) {}

		bool read_object_begin(archive_object& obj) override;
		bool read_object_end() override;
		std::string read_string() override;
		int32_t read_int() override;
		float read_float() override;
		uint8_t read_byte() override;
		uint16_t read_word() override;
		uint32_t read_enum() override;
		bool read_bool() override;
		color read_color() override;
		glm::vec3 read_vec3() override;
		glm::vec2 read_vec2() override;

	protected:
		void read_header() override;

		void skip_optional_hash();
		uint16_t assure_entry(archive_binsafe_type tp);

	private:
		uint32_t _m_object_count {0};
		uint32_t _m_hash_table_offset {0};
		uint32_t _m_bs_version {0};
	};
}// namespace phoenix
