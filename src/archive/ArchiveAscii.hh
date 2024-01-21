// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	class ReadArchiveAscii final : public ReadArchive {
	public:
		ReadArchiveAscii(ArchiveHeader&& parent_header, Read* r, std::unique_ptr<Read> owned)
		    : ReadArchive(std::forward<ArchiveHeader>(parent_header), r, std::move(owned)) {}

		ReadArchiveAscii(ArchiveHeader&& parent_header, Read* r)
		    : ReadArchive(std::forward<ArchiveHeader>(parent_header), r) {}

		bool read_object_begin(ArchiveObject& obj) override;
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
		AxisAlignedBoundingBox read_bbox() override;
		glm::mat3x3 read_mat3x3() override;
		ZKREM("Deprecated") phoenix::buffer read_raw_bytes(uint32_t size) override;
		std::unique_ptr<Read> read_raw(std::size_t size) override;

	protected:
		void read_header() override;
		void skip_entry() override;

		std::string read_entry(std::string_view type);

	private:
		int32_t _m_objects {0};
	};

	class WriteArchiveAscii final : public WriteArchive {
	public:
		explicit WriteArchiveAscii(Write* w);

		std::uint32_t
		write_object_begin(std::string_view object_name, std::string_view class_name, std::uint16_t version) override;
		void write_object_end() override;
		void write_ref(std::string_view object_name, uint32_t index) override;

		void write_string(std::string_view name, std::string_view v) override;
		void write_int(std::string_view name, std::int32_t v) override;
		void write_float(std::string_view name, float v) override;
		void write_byte(std::string_view name, std::uint8_t v) override;
		void write_word(std::string_view name, std::uint16_t v) override;
		void write_enum(std::string_view name, std::uint32_t v) override;
		void write_bool(std::string_view name, bool v) override;
		void write_color(std::string_view name, glm::u8vec4 v) override;
		void write_vec3(std::string_view name, glm::vec3 const& v) override;
		void write_vec2(std::string_view name, glm::vec2 v) override;
		void write_bbox(std::string_view name, AxisAlignedBoundingBox const& v) override;
		void write_mat3x3(std::string_view name, glm::mat3x3 const& v) override;
		void write_raw(std::string_view name, std::vector<std::byte> const& v) override;
		void write_raw(std::string_view name, std::byte const* v, std::uint16_t length) override;
		void write_raw_float(std::string_view name, float const* v, std::uint16_t length) override;
		void write_header() override;

		[[nodiscard]] Write* get_stream() const noexcept override {
			return _m_write;
		}

	private:
		void write_indent();
		void write_entry(std::string_view name, std::string_view type, std::string_view value);

		Write* _m_write;
		std::uint32_t _m_index {0};
		std::uint32_t _m_indent {0};
		std::size_t _m_head;
	};
} // namespace zenkit
