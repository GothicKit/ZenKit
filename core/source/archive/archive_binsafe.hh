// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/archive.hh>

namespace phoenix {
	constexpr u8 type_sizes[] = {
			0,                // ?            = 0x00
			0,                // bs_string    = 0x01,
			sizeof(u32),      // bs_int       = 0x02,
			sizeof(float),    // bs_float     = 0x03,
			sizeof(u8),       // bs_byte      = 0x04,
			sizeof(u16),      // bs_word      = 0x05,
			sizeof(u32),      // bs_bool      = 0x06,
			sizeof(float) * 3,// bs_vec3      = 0x07,
			sizeof(u8) * 4,   // bs_color     = 0x08,
			0,                // bs_raw       = 0x09,
			0,                // ?            = 0x0A
			0,                // ?            = 0x0B
			0,                // ?            = 0x0C
			0,                // ?            = 0x0D
			0,                // ?            = 0x0E
			0,                // ?            = 0x0F
			0,                // bs_raw_float = 0x10,
			sizeof(u32),      // bs_enum      = 0x11,
			sizeof(u32),      // bs_hash      = 0x12,
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
		s32 read_int() override;
		float read_float() override;
		u8 read_byte() override;
		u16 read_word() override;
		u32 read_enum() override;
		bool read_bool() override;
		color read_color() override;
		glm::vec3 read_vec3() override;
		glm::vec2 read_vec2() override;

	protected:
		void read_header() override;

		void skip_optional_hash();
		u16 assure_entry(archive_binsafe_type tp);

	private:
		u32 _m_object_count {0};
		u32 _m_hash_table_offset {0};
		u32 _m_bs_version {0};
	};
}// namespace phoenix
