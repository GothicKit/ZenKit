// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include <map>
#include <vector>

namespace zenkit {
	static constexpr std::uint8_t type_sizes[] = {
	    0,                        // ?            = 0x00
	    0,                        // bs_string    = 0x01,
	    sizeof(std::int32_t),     // bs_int       = 0x02,
	    sizeof(float),            // bs_float     = 0x03,
	    sizeof(std::uint8_t),     // bs_byte      = 0x04,
	    sizeof(std::uint16_t),    // bs_word      = 0x05,
	    sizeof(std::uint32_t),    // bs_bool      = 0x06,
	    sizeof(float) * 3,        // bs_vec3      = 0x07,
	    sizeof(std::uint8_t) * 4, // bs_color     = 0x08,
	    0,                        // bs_raw       = 0x09,
	    0,                        // ?            = 0x0A
	    0,                        // ?            = 0x0B
	    0,                        // ?            = 0x0C
	    0,                        // ?            = 0x0D
	    0,                        // ?            = 0x0E
	    0,                        // ?            = 0x0F
	    0,                        // bs_raw_float = 0x10,
	    sizeof(std::uint32_t),    // bs_enum      = 0x11,
	    sizeof(std::uint32_t),    // bs_hash      = 0x12,
	};

	struct hash_table_entry {
		std::string key;
		std::uint32_t hash; // TODO: I don't know what this is.
	};

	class ReadArchiveBinsafe final : public ReadArchive {
	public:
		ReadArchiveBinsafe(ArchiveHeader&& parent_header, Read* r, std::unique_ptr<Read> owned)
		    : ReadArchive(std::forward<ArchiveHeader>(parent_header), r, std::move(owned)) {}

		ReadArchiveBinsafe(ArchiveHeader&& parent_header, Read* r)
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
		Color read_color() override;
		Vec3 read_vec3() override;
		Vec2 read_vec2() override;
		AxisAlignedBoundingBox read_bbox() override;
		Mat3 read_mat3x3() override;
		std::unique_ptr<Read> read_raw(std::size_t size) override;

	protected:
		void read_header() override;
		void skip_entry() override;

		std::string const& get_entry_key();

		template <ArchiveEntryType tp>
		std::uint16_t ensure_entry_meta();

	private:
		std::uint32_t _m_object_count {0};
		std::uint32_t _m_bs_version {0};

		std::vector<hash_table_entry> _m_hash_table_entries;
	};

	struct BinsafeEq {
		using is_transparent = std::true_type;

		[[nodiscard]] bool operator()(std::string const& a, std::string_view b) const noexcept {
			return a == b;
		}

		[[nodiscard]] bool operator()(std::string const& a, std::string const& b) const noexcept {
			return a == b;
		}
	};

	class WriteArchiveBinsafe final : public WriteArchive {
	public:
		explicit WriteArchiveBinsafe(Write* w);

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
		void write_color(std::string_view name, Color v) override;
		void write_vec3(std::string_view name, Vec3 const& v) override;
		void write_vec2(std::string_view name, Vec2 v) override;
		void write_bbox(std::string_view name, AxisAlignedBoundingBox const& v) override;
		void write_mat3x3(std::string_view name, Mat3 const& v) override;
		void write_raw(std::string_view name, std::vector<std::byte> const& v) override;
		void write_raw(std::string_view name, std::byte const* v, std::uint16_t length) override;
		void write_raw_float(std::string_view name, float const* v, std::uint16_t length) override;
		void write_header() override;

		[[nodiscard]] Write* get_stream() const noexcept override {
			return _m_write;
		}

	private:
		void write_entry(std::string_view name, ArchiveEntryType type);

		Write* _m_write;
		std::uint32_t _m_index {0};
		std::map<std::string, std::uint16_t, std::less<>> _m_hash_keys;
		std::size_t _m_head;
	};
} // namespace zenkit
