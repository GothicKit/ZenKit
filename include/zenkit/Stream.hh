// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Logger.hh"

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstddef>
#include <filesystem>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {

	// Windows does not define ssize_t
#ifdef _WIN32
	typedef std::ptrdiff_t ssize_t;
#endif

	enum class Whence { BEG = 0x00, CUR = 0x01, END = 0x02 };

	class Read ZKAPI {
	public:
		virtual ~Read() noexcept = default;

		[[nodiscard]] char read_char() noexcept;
		[[nodiscard]] int8_t read_byte() noexcept;
		[[nodiscard]] uint8_t read_ubyte() noexcept;
		[[nodiscard]] int16_t read_short() noexcept;
		[[nodiscard]] uint16_t read_ushort() noexcept;
		[[nodiscard]] int32_t read_int() noexcept;
		[[nodiscard]] uint32_t read_uint() noexcept;
		[[nodiscard]] float read_float() noexcept;
		[[nodiscard]] std::string read_string(size_t len) noexcept;
		[[nodiscard]] std::string read_line(bool skipws) noexcept;
		[[nodiscard]] glm::vec2 read_vec2() noexcept;
		[[nodiscard]] glm::vec3 read_vec3() noexcept;
		[[nodiscard]] glm::mat3 read_mat3() noexcept;
		[[nodiscard]] glm::mat4 read_mat4() noexcept;

		[[nodiscard]] virtual std::string read_line_then_ignore(std::string_view chars) noexcept;

		virtual size_t read(void* buf, size_t len) noexcept = 0;
		virtual void seek(ssize_t off, Whence whence) noexcept = 0;
		[[nodiscard]] virtual size_t tell() const noexcept = 0;
		[[nodiscard]] virtual bool eof() const noexcept = 0;

		[[nodiscard]] static std::unique_ptr<Read> from(FILE* stream);
		[[nodiscard]] static std::unique_ptr<Read> from(std::istream* stream);
		[[nodiscard]] static std::unique_ptr<Read> from(std::byte const* bytes, size_t len);
		[[nodiscard]] static std::unique_ptr<Read> from(std::vector<std::byte> const* vector);
		[[nodiscard]] static std::unique_ptr<Read> from(std::vector<std::byte> vector);
		[[nodiscard]] static std::unique_ptr<Read> from(std::filesystem::path const& path);
		[[nodiscard]] ZKREM("deprecated") static std::unique_ptr<Read> from(phoenix::buffer* buf);
	};

	class Write ZKAPI {
	public:
		virtual ~Write() noexcept = default;

		void write_char(char v) noexcept;
		void write_byte(int8_t v) noexcept;
		void write_ubyte(uint8_t v) noexcept;
		void write_short(int16_t v) noexcept;
		void write_ushort(uint16_t v) noexcept;
		void write_int(int32_t v) noexcept;
		void write_uint(uint32_t v) noexcept;
		void write_float(float v) noexcept;
		void write_string(std::string_view v) noexcept;
		void write_string0(std::string_view v) noexcept;
		void write_line(std::string_view v) noexcept;
		void write_vec2(glm::vec2 const& v) noexcept;
		void write_vec3(glm::vec3 const& v) noexcept;
		void write_mat3(glm::mat3 const& v) noexcept;
		void write_mat4(glm::mat4 const& v) noexcept;

		virtual size_t write(void const* buf, size_t len) noexcept = 0;
		virtual void seek(ssize_t off, Whence whence) noexcept = 0;
		[[nodiscard]] virtual size_t tell() const noexcept = 0;

		[[nodiscard]] static std::unique_ptr<Write> to(::FILE* stream);
		[[nodiscard]] static std::unique_ptr<Write> to(std::ostream* stream);
		[[nodiscard]] static std::unique_ptr<Write> to(std::byte* bytes, size_t len);
		[[nodiscard]] static std::unique_ptr<Write> to(std::vector<std::byte>* vector);
	};

	namespace proto {
		template <typename T>
		inline std::enable_if_t<std::is_enum_v<T>, void>
		read_chunked(Read* r, char const* name, std::function<bool(Read*, T)> cb) {
			do {
				auto type = static_cast<T>(r->read_ushort());
				auto size = r->read_uint();
				auto target = r->tell() + size;

				auto done = cb(r, type);

				auto pos = r->tell();
				if (pos < target) {
					Logger::log(LogLevel::WARNING,
					            name,
					            "%zu bytes remaining in section %hx",
					            target - pos,
					            static_cast<std::uint16_t>(type));
				} else if (pos > target) {
					Logger::log(LogLevel::ERROR,
					            name,
					            "%zu bytes overflowed in section %hx",
					            pos - target,
					            static_cast<std::uint16_t>(type));
				}

				// Make sure we always place the cursor at the position of the next chunk.
				r->seek(static_cast<ssize_t>(target), Whence::BEG);

				if (done) {
					break;
				}
			} while (!r->eof());
		}

		template <typename T>
		inline std::enable_if_t<std::is_enum_v<T>, void>
		read_chunked(Read* r, char const* name, std::function<bool(Read*, T, size_t&)> cb) {
			do {
				auto type = static_cast<T>(r->read_ushort());
				auto size = r->read_uint();
				auto target = r->tell() + size;

				auto done = cb(r, type, target);

				auto pos = r->tell();
				if (pos < target) {
					Logger::log(LogLevel::WARNING, name, "%zu bytes remaining in section %hx", target - pos, type);
				} else if (r->tell() > target) {
					Logger::log(LogLevel::ERROR, name, "%zu bytes overflowed in section %hx", target - pos, type);
				}

				// Make sure we always place the cursor at the position of the next chunk.
				r->seek(static_cast<ssize_t>(target), Whence::BEG);

				if (done) {
					break;
				}
			} while (!r->eof());
		}

		template <typename T>
		inline std::enable_if_t<std::is_enum_v<T>, void> write_chunk(Write* w, T v, std::function<void(Write*)> cb) {
			w->write_ushort(static_cast<uint16_t>(v));

			auto size_off = w->tell();
			w->write_uint(0);

			cb(w);

			auto len = w->tell() - size_off;
			w->seek(size_off, Whence::BEG);
			w->write_uint(len - sizeof(uint32_t));
			w->seek(len, Whence::CUR);
		}

	} // namespace proto
} // namespace zenkit
