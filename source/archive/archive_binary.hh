// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/archive.hh>

namespace phoenix {
	class archive_reader_binary final : public archive_reader {
	public:
		inline archive_reader_binary(buffer& in, archive_header&& header) : archive_reader(in, std::move(header)) {}

		bool read_object_begin(archive_object& obj) override;
		bool read_object_end() override;
		std::string read_string() override;
		std::int32_t read_int() override;
		float read_float() override;
		std::uint8_t read_byte() override;
		std::uint16_t read_word() override;
		std::uint32_t read_enum() override;
		bool read_bool() override;
		glm::u8vec4 read_color() override;
		glm::vec3 read_vec3() override;
		glm::vec2 read_vec2() override;
		std::tuple<glm::vec3, glm::vec3> read_bbox() override;
		glm::mat3x3 read_mat3x3() override;
		std::vector<std::uint8_t> read_raw_bytes() override;

	protected:
		void skip_entry() override;

	public:
		void skip_object(bool skip_current) override;

	protected:
		void read_header() override;

	private:
		int _m_objects {0};
	};
} // namespace phoenix
