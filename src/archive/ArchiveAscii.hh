// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

namespace zenkit {
	class ReadArchiveAscii final : public ReadArchive {
	public:
		inline ReadArchiveAscii(ArchiveHeader&& parent_header, Read* r, std::unique_ptr<Read> owned)
		    : ReadArchive(std::forward<ArchiveHeader>(parent_header), r, std::move(owned)) {}

		inline ReadArchiveAscii(ArchiveHeader&& parent_header, Read* r)
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
		std::unique_ptr<Read> read_raw(uint32_t size) override;

	protected:
		void read_header() override;
		void skip_entry() override;

		std::string read_entry(std::string_view type);

	private:
		int32_t _m_objects {0};
	};

} // namespace phoenix
