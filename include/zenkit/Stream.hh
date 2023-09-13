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

	/// \brief Basic input device abstraction for <i>ZenKit</i>.
	///
	/// <p>Provides functions for reading primitives from an underlying datasource in little-endian. May be subclassed
	/// to implement a custom data source though many common sources are built-in. Is is recommended to create a Read
	/// stream by calling Read::from(std::filesystem::path const&) since it is fast and safe in comparison.</p>
	///
	/// <h3>Built-in data sources</h3>
	/// <p>The following built-in data source implementations are available by calling Read::from.</p>
	/// <table>
	/// <thead>
	///   <tr>
	///     <th>Syntax</th>
	///     <th>Description</th>
	///   </tr>
	/// </thead>
	/// <tbody>
	///   <tr>
	///     <td>Read::from(::FILE*)</td>
	///     <td>Uses a C-style FILE* as the data source. Note that the input stream pointed to is required to be
	///     seekable, thus stdin is not fully supported.</td>
	///   </tr>
	///   <tr>
	///     <td>Read::from(std::istream*)</td>
	///     <td>Uses a C++-style input stream as the data source. Note that the input stream is required to be seekable,
	///     thus std::cin is not fully supported.</td>
	///   </tr>
	///   <tr>
	///     <td>Read::from(std::byte const*, size_t)</td>
	///     <td>Uses a raw memory buffer as the data source. The given buffer is never copied and thus must remain valid
	///     as long as the stream is in use.</td>
	///   </tr>
	///   <tr>
	///     <td>Read::from(std::vector<std::byte> const*)</td>
	///     <td>Uses a raw memory buffer as the data source. This function behaves exactly like Read::from(std::byte
	///     const*, size_t) in that is takes the raw memory buffer stored in the std::vector and uses it directly
	///     without making a copy. This means that the vector being passed in may not be changed in any way (including
	///     its destruction) while the stream is in use.</td>
	///   </tr>
	///   <tr>
	///     <td>Read::from(std::vector<std::byte>)</td>
	///     <td>Uses an owned memory buffer as the data source. Stores the given memory buffer inside the Read instance
	///     keeping it alive as long as the instance is. Generally, this is safer than passing a raw memory buffer.</td>
	///   </tr>
	///   <tr>
	///     <td>Read::from(std::filesystem::path const&)</td>
	///     <td>Memory-maps the file at the given path and creates a Read instance from it. The memory mapping is held
	///     until the instance is destroyed.</td>
	///   </tr>
	/// </tbody>
	/// </table>
	///
	/// <h3>Implementing a custom Read</h3>
	/// <p>Implementing a custom Read is as simple as creating a new subclass and implementing the Read::read,
	/// Read::seek, Read::tell and Read::eof member functions.</p>
	class ZKAPI Read {
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

		[[nodiscard]] static std::unique_ptr<Read> from(::FILE* stream);
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
		read_chunked(Read* r, char const* name, std::function<bool(Read*, T)> const& cb) {
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
		read_chunked(Read* r, char const* name, std::function<bool(Read*, T, size_t&)> const& cb) {
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
		inline std::enable_if_t<std::is_enum_v<T>, void>
		write_chunk(Write* w, T v, std::function<void(Write*)> const& cb) {
			w->write_ushort(static_cast<uint16_t>(v));

			auto size_off = static_cast<ssize_t>(w->tell());
			w->write_uint(0);

			cb(w);

			auto len = static_cast<ssize_t>(w->tell()) - size_off;
			w->seek(size_off, Whence::BEG);
			w->write_uint(len - sizeof(uint32_t));
			w->seek(len, Whence::CUR);
		}
	} // namespace proto
} // namespace zenkit
