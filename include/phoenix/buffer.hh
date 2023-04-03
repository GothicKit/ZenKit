// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/phoenix.hh>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace phoenix {
	/// \brief Base class for exceptions thrown by a phoenix::buffer.
	class buffer_error : public error {
	public:
		using error::error;
	};

	/// \brief Exception thrown when reading too many bytes from a buffer.
	///
	/// This exception is thrown if the number of bytes read from a buffer
	/// is more than the number of bytes remaining.
	class buffer_underflow : public buffer_error {
	public:
		PHOENIX_API buffer_underflow(std::uint64_t byte, std::uint64_t size);
		PHOENIX_API buffer_underflow(std::uint64_t byte, std::uint64_t size, std::string&& context);
		PHOENIX_API buffer_underflow(std::uint64_t byte, std::string&& context);

	public:
		const std::uint64_t byte, size;
		const std::optional<std::string> context;
	};

	/// \brief Exception thrown when writing too many bytes to a buffer.
	///
	/// This exception is thrown if the number of bytes written to a buffer
	/// is more than the number of bytes remaining.
	class buffer_overflow : public buffer_error {
	public:
		PHOENIX_API buffer_overflow(std::uint64_t byte, std::uint64_t size);
		PHOENIX_API buffer_overflow(std::uint64_t byte, std::uint64_t size, std::string&& context);

	public:
		const std::uint64_t byte, size;
		const std::optional<std::string> context;
	};

	/// \brief Exception thrown if a write is attempted on a readonly buffer.
	class buffer_readonly : public buffer_error {
	public:
		PHOENIX_API explicit buffer_readonly() : buffer_error("buffer is not readonly") {}
	};

	/// \brief Base class for all buffer backings.
	///
	/// A buffer backing contains the data behind a buffer. It can be shared by multiple buffers possibly with
	/// each referencing a subsection of the backing. For this reason, buffer backings should be stateless.
	class buffer_backing {
	public:
		PHOENIX_API virtual ~buffer_backing() = default;

		/// \brief Returns whether this backing considered direct or not.
		///
		/// A backing is considered direct if reading from or writing to it has a side effect like causing I/O on
		/// a file. An example of a direct backing would be a backing which reads from or writes to a memory-mapped
		/// file.
		///
		/// \return `true` if this backing is direct and `false` if not.
		[[nodiscard]] PHOENIX_API virtual bool direct() const noexcept = 0;

		/// \brief Returns whether or not this backing is readonly or not.
		///
		/// A readonly backing is a backing which can not be written to.
		///
		/// \return `true` if this backing is read-only and `false` if not.
		[[nodiscard]] PHOENIX_API virtual bool readonly() const noexcept = 0;

		/// \brief Returns the number of bytes available in this backing.
		/// \return The number of bytes available in this backing.
		[[nodiscard]] PHOENIX_API virtual std::uint64_t size() const noexcept = 0;

		/// \brief Retrieves a read-only raw byte array of this backing.
		/// \return A read-only raw byte array into this backing.
		[[nodiscard]] PHOENIX_API virtual const std::byte* array() const = 0;

		/// \brief Fills the given \p buf with bytes from this backing starting at \p offset.
		///
		/// This function is idempotent; calling it never alters any internal state.
		///
		/// \param buf A buffer to read into.
		/// \param size The number of bytes to read.
		/// \param offset The offset at which to start reading bytes into \p buf.
		/// \throws buffer_underflow if filling \p buf with bytes starting at \p offset fails.
		PHOENIX_API virtual void read(std::byte* buf, std::uint64_t size, std::uint64_t offset) const = 0;

		/// \brief Writes all bytes from \p buf into this backing beginning at \p offset.
		///
		/// This function is idempotent; calling it never alters any internal state.
		///
		/// \param buf The data to write.
		/// \param size The number of bytes to write.
		/// \param offset The offset at which to start writing.
		/// \throws buffer_overflow if writing all bytes of \p buf starting at \p offset fails.
		/// \throws buffer_readonly if this backing is readonly.
		PHOENIX_API virtual void write([[maybe_unused]] const std::byte* buf,
		                               [[maybe_unused]] std::uint64_t size,
		                               [[maybe_unused]] std::uint64_t offset) {
			throw buffer_readonly {};
		}
	};

	/// \brief A buffer implementation inspired by Java's ByteBuffer
	class buffer {
	private:
		PHOENIX_INTERNAL buffer(std::shared_ptr<buffer_backing> backing, std::uint64_t begin, std::uint64_t end);
		PHOENIX_INTERNAL buffer(std::shared_ptr<buffer_backing> backing,
		                        std::uint64_t begin,
		                        std::uint64_t end,
		                        std::uint64_t capacity,
		                        std::uint64_t position,
		                        std::optional<std::uint64_t> mark);

		/// \brief Reads a scalar of type \p T at the current #position.
		///
		/// After reading the value, the #position is increased by ``sizeof(T)``.
		///
		/// \tparam T The type of scalar to read. Must be a std::integral or a std::floating_point type.
		/// \return The value read.
		/// \throws buffer_underflow if there are not enough bytes #remaining to read the value.
		/// \see buffer_backing::read
		template <
		    typename T,
		    typename = typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type>
		[[nodiscard]] PHOENIX_INTERNAL T _get_t();

		/// \brief Reads a scalar of type \p T at the given \p pos.
		/// \tparam T The type of scalar to read. Must be a std::integral or a std::floating_point type.
		/// \return The value read.
		/// \throws buffer_underflow if there are not enough bytes in the backing to read the value.
		/// \see buffer_backing::read
		template <
		    typename T,
		    typename = typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type>
		[[nodiscard]] PHOENIX_INTERNAL T _get_t(std::uint64_t pos) const;

		/// \brief Writes a scalar of type \p T at the current #position.
		///
		/// After writing the value, the #position is increased by ``sizeof(T)``.
		///
		/// \tparam T The type of scalar to write. Must be a std::integral or a std::floating_point type.
		/// \throws buffer_overflow if there are not enough bytes #remaining to write the value.
		/// \see buffer_backing::write
		template <
		    typename T,
		    typename = typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value>::type>
		PHOENIX_INTERNAL void _put_t(T value);

	public:
		/// \brief Constructs a new buffer from the given backing.
		///
		/// The new buffer will encompass all bytes available in the backing.
		/// To shrink the buffer, see #slice or #extract.
		///
		/// \param backing The buffer backing to use.
		PHOENIX_API explicit buffer(std::shared_ptr<buffer_backing> backing);

		/// \brief Gets the current position of this buffer.
		/// \return The current position of this buffer.
		[[nodiscard]] PHOENIX_API inline std::uint64_t position() const noexcept {
			return _m_position;
		}

		/// \brief Sets this buffer's position.
		/// \param pos The new position value.
		/// \throws buffer_underflow if \p pos is greater than #limit.
		PHOENIX_API void position(std::uint64_t pos);

		/// \brief Returns the number of bytes available in this buffer.
		/// \return The limit of this buffer.
		[[nodiscard]] PHOENIX_API inline std::uint64_t limit() const noexcept {
			return _m_backing_end - _m_backing_begin;
		}

		/// \brief Sets this buffer's limit.
		///
		/// If the position is larger than the new limit then it is set to the new limit.
		///
		/// \param limit The new limit to set.
		/// \throws buffer_underflow if \p limit is greater than #capacity.
		PHOENIX_API void limit(std::uint64_t limit);

		/// \brief Rewinds this buffer by setting the position to 0.
		///
		/// This operation discards the #mark if it is set.
		PHOENIX_API inline void rewind() {
			_m_position = 0;
			_m_mark.reset();
		}

		/// \brief Skips \p count bytes in this buffer.
		/// \param count The number of bytes to skip.
		/// \throws buffer_underflow if #position + \p count > #limit
		/// \see #position
		PHOENIX_API inline void skip(std::uint64_t count) {
			return this->position(this->position() + count);
		}

		/// \brief Returns the number of bytes remaining in this buffer.
		///
		/// The number of remaining bytes is equal to #limit - #position.
		///
		/// \return The number of bytes remaining in this buffer.
		[[nodiscard]] PHOENIX_API inline std::uint64_t remaining() const noexcept {
			return this->limit() - this->position();
		}

		/// \brief Returns the capacity of this buffer.
		///
		/// The capacity is the number of bytes allocated to the buffer when it is constructed, not
		/// the total number of bytes available in the backing.
		///
		/// \return The capacity of this buffer.
		[[nodiscard]] PHOENIX_API inline std::uint64_t capacity() const noexcept {
			return _m_capacity;
		}

		/// \brief Returns whether this buffer is considered to be direct or not.
		/// \return `true` if the backing of this buffer is considered to be direct.
		/// \see buffer_backing::direct
		[[nodiscard]] PHOENIX_API inline bool direct() const noexcept {
			return _m_backing->direct();
		}

		/// \brief Returns whether this buffer is read-only or not.
		/// \return `true` if this buffer is read-only.
		/// \see buffer_backing::readonly
		[[nodiscard]] PHOENIX_API inline bool readonly() const noexcept {
			return _m_backing->readonly();
		}

		/// \brief Clears this buffer by resetting #position and #limit.
		///
		/// #limit is set to #capacity and #position is set to 0. The mark is discarded if
		/// it is set.
		PHOENIX_API void clear() noexcept;

		/// \brief Flips this buffer.
		///
		/// Its limit is set to the current position and its current position is set to 0.
		PHOENIX_API void flip() noexcept;

		/// \brief Sets this buffer's mark at its position.
		/// \return This buffer.
		PHOENIX_API inline void mark() noexcept {
			_m_mark = position();
		}

		/// \brief Resets this buffer's position to the previously-marked position.
		/// \return This buffer.
		PHOENIX_API inline void reset() {
			if (_m_mark) {
				position(*_m_mark);
			}
		}

		/// \brief Creates a new buffer which shares its content with this buffer.
		///
		/// The new buffers position, capacity and limit will be the same as this buffer's current position,
		/// capacity and limit.
		///
		/// \return The newly created buffer.
		[[nodiscard]] PHOENIX_API buffer duplicate() const noexcept;

		/// \brief Creates a new buffer which shares a subsequence of this buffer.
		///
		/// The shared subsequence starts at the current position and ends at this buffer's limit.
		///
		/// \return The newly created buffer.
		[[nodiscard]] PHOENIX_API buffer slice() const noexcept;

		/// \brief Creates a new buffer which shares a subsequence of this buffer.
		///
		/// The shared subsequence starts at the given index in this buffer and contains the following size bytes.
		///
		/// \param index The index in this buffer at which to start the new buffer.
		/// \param size The number of bytes the new buffer will encompass.
		/// \return The newly created buffer.
		/// \throws buffer_underflow if \p index + \p size > #limit.
		[[nodiscard]] PHOENIX_API buffer slice(std::uint64_t index, std::uint64_t size) const;

		/// \brief Creates a new buffer which shares a subsequence of this buffer.
		///
		/// Slices off a buffer like `slice(position(), size)` and advances the
		/// position of the original buffer by `size` bytes (like `skip(size)`).
		///
		/// \param size The number of bytes to extract.
		/// \return The newly created buffer.
		/// \throws buffer_underflow if #position + \p size > #limit.
		[[nodiscard]] PHOENIX_API buffer extract(std::uint64_t size) {
			auto sl = this->slice(position(), size);
			_m_position += size;
			return sl;
		}

		/// \return A read-only view into the raw contents of this buffer.
		[[nodiscard]] PHOENIX_API inline const std::byte* array() const noexcept {
			return _m_backing->array() + _m_backing_begin;
		}

		/// \brief Get bytes from the buffer, put them into buf and advance the position accordingly.
		/// \param buf The buffer to write into.
		/// \param size The number of bytes to get.
		/// \throws buffer_underflow if the size of \p buf > #remaining.
		PHOENIX_API void get(std::byte* buf, std::uint64_t size);

		/// \brief Get bytes from the buffer, put them into buf and advance the position accordingly.
		/// \param buf The buffer to write into.
		/// \param size The number of bytes to get.
		/// \throws buffer_underflow if the size of \p buf > #remaining.
		PHOENIX_API inline void get(std::uint8_t* buf, std::uint64_t size) {
			return this->get((std::byte*) buf, size);
		}

		/// \brief Get a value of type std::uint8_t from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API std::uint8_t get();

		/// \brief Get a value of type ``char`` from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API char get_char();

		/// \brief Get a value of type std::int16_t from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API std::int16_t get_short();

		/// \brief Get a value of type std::uint16_t from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API std::uint16_t get_ushort();

		/// \brief Get a value of type std::int32_t from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API std::int32_t get_int();

		/// \brief Get a value of type std::uint32_t from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API std::uint32_t get_uint();

		/// \brief  Get a value of type std::int64_t from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API std::int64_t get_long();

		/// \brief Get a value of type std::uint64_t from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API std::uint64_t get_ulong();

		/// \brief Get a value of type float from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API float get_float();

		/// \brief Get a value of type double from the buffer and advance the position accordingly.
		/// \return The value just read.
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API double get_double();

		/// \brief Get a string of the given size from the buffer and advance the position accordingly
		/// \param size The number of characters to read.
		/// \return The string just read.
		/// \throws buffer_underflow if the string can't be read.
		[[nodiscard]] PHOENIX_API std::string get_string(std::uint64_t size);

		/// \brief Get a line from the buffer and advance the position accordingly.
		/// \param skip_whitespace Set to `true` to skip whitespace characters immediately following the line.
		/// \return The line just read.
		/// \throws buffer_underflow if the string can't be read.
		/// \see isspace
		[[nodiscard]] PHOENIX_API std::string get_line(bool skip_whitespace = true);

		/// \brief Get a line from the buffer and advance the position accordingly.
		/// \param skip_whitespace Set to `true` to skip whitespace characters immediately following the line.
		/// \return The line just read.
		/// \throws buffer_underflow if the string can't be read.
		/// \see isspace
		[[nodiscard]] PHOENIX_API std::string get_line_and_ignore(std::string_view whitespace);

		/// \brief Get a line from the buffer, unescape all relevant escape sequences, and
		///        advance the position accordingly.
		/// \param skip_whitespace Set to `true` to skip whitespace characters immediately following the line.
		/// \return The line just read.
		/// \throws buffer_underflow if the string can't be read.
		/// \see isspace
		[[nodiscard]] PHOENIX_API std::string get_line_escaped(bool skip_whitespace = true);

		/// \brief Get a 2D-vector from the buffer.
		/// \return The vector just read
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API glm::vec2 get_vec2();

		/// \brief Get a 3D-vector from the buffer.
		/// \return The vector just read
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API glm::vec3 get_vec3();

		/// \brief Get a 3x3 column-major matrix from the buffer.
		/// \return The vector just read
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API glm::mat3x3 get_mat3x3();

		/// \brief Get a 4x4 column-major matrix from the buffer.
		/// \return The vector just read
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API glm::mat4x4 get_mat4x4();

		/// \brief Get a 4D-vector from the buffer.
		/// \return The vector just read
		/// \throws buffer_underflow if the value can't be read.
		[[nodiscard]] PHOENIX_API glm::vec4 get_vec4();

		/// \brief Put bytes from buf into the buffer and advance the position accordingly.
		/// \param buf The data to write.
		/// \param size The number of bytes to write.
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put(const std::byte* buf, std::uint64_t size);

		/// \brief Put bytes from buf into the buffer and advance the position accordingly.
		/// \param buf The data to write.
		/// \param size The number of bytes to write.
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put(const std::uint8_t* buf, std::uint64_t size);

		/// \brief Put a value of type std::uint8_t into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put(std::uint8_t value);

		/// \brief Put a value of type char into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_char(char value);

		/// \brief Put a value of type std::int16_t into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_short(std::int16_t value);

		/// \brief Put a value of type std::uint16_t into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_ushort(std::uint16_t value);

		/// \brief Put a value of type std::int32_t into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_int(std::int32_t value);

		/// \brief Put a value of type std::uint32_t into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_uint(std::uint32_t value);

		/// \brief Put a value of type std::int64_t into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_long(std::int64_t value);

		/// \brief Put a value of type std::uint64_t into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_ulong(std::uint64_t value);

		/// \brief Put a value of type float into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_float(float value);

		/// \brief Put a value of type double into the buffer and advance the position accordingly
		/// \param value The value to put into the buffer
		/// \return This buffer.
		/// \throws buffer_overflow if the value can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_double(double value);

		/// \brief Put string into the buffer and advance the position accordingly
		/// \param str The string to put into the buffer.
		/// \return This buffer.
		/// \throws buffer_overflow if the string can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_string(std::string_view str);

		/// \brief Put string followed by <LF> into the buffer and advance the position accordingly
		/// \param str The string to put into the buffer.
		/// \return This buffer.
		/// \throws buffer_overflow if the string can't be written.
		/// \throws buffer_readonly if the buffer is read-only.
		PHOENIX_API void put_line(std::string_view str);

		/// \brief Allocates a new buffer with the given size.
		///
		/// The allocated buffer will be backed by a vector and thus it will reside on the heap. This backing
		/// vector can be accessed through #array at any time.
		///
		/// \param size The number of bytes to allocate.
		/// \return The newly allocated buffer.
		PHOENIX_API static buffer allocate(std::uint64_t size);

		/// \brief Creates a new buffer from the given vector.
		///
		/// The allocated buffer will be backed by the given vector and thus it will reside on the heap.
		/// The backing vector can be accessed through #array at any time.
		///
		/// \param buf A vector containing the data to be wrapped into a buffer.
		/// \param readonly Set to `false` to be able to write to the buffer.
		/// \return The newly created buffer.
		PHOENIX_API static buffer of(std::vector<std::byte>&& buf, bool readonly = true);

		/// \brief Opens the given file as a direct buffer.
		///
		/// The file is memory-mapped.
		///
		/// \param path The path of the file to mmap.
		/// \param readonly Set to `false` to be able to write to the buffer.
		/// \return The newly created buffer.
		PHOENIX_API static buffer mmap(const std::filesystem::path& path, bool readonly = true);

		/// \brief Opens the given file as an indirect buffer.
		///
		/// The whole file is read into memory.
		///
		/// \param path The path of the file to read.
		/// \param readonly Set to `false` to be able to write to the buffer.
		/// \note Creating a buffer using this method is generally discouraged. It might cause a small performance
		///       gain for small files but it is generally slower and uses more memory. You should probably use #mmap
		///       instead.
		/// \return The newly created buffer.
		PHOENIX_API static buffer read(const std::filesystem::path& path, bool readonly = true);

		/// \brief Returns a duplicate of the empty buffer.
		/// \return The empty buffer.
		PHOENIX_API static buffer empty();

	private:
		static std::unique_ptr<buffer> _m_empty;
		PHOENIX_API friend bool operator==(const buffer&, const buffer&);

		std::shared_ptr<buffer_backing> _m_backing;
		std::uint64_t _m_backing_begin, _m_backing_end;
		std::uint64_t _m_capacity;
		std::uint64_t _m_position;

		std::optional<std::uint64_t> _m_mark;
	};

	PHOENIX_API bool operator==(const buffer&, const buffer&);
} // namespace phoenix
