// Copyright © 2022 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace phoenix {
	struct buffer_underflow_error : public std::runtime_error {
		buffer_underflow_error() : std::runtime_error("buffer underflow") {}
	};

	struct buffer_overflow_error : public std::runtime_error {
		buffer_overflow_error() : std::runtime_error("buffer overflow") {}
	};

	struct buffer_readonly_error : public std::runtime_error {
		buffer_readonly_error() : std::runtime_error("buffer is readonly") {}
	};

	struct buffer_index_error : public std::runtime_error {
		buffer_index_error() : std::runtime_error("buffer index out-of-bounds") {}
	};

	/**
	 * @brief Base class for all buffer data sinks and sources.
	 */
	class buffer_backing {
	public:
		virtual ~buffer_backing() = default;

		/**
		 * @return `true` if this backing is considered direct and `false` if not.
		 */
		[[nodiscard]] virtual bool direct() const noexcept = 0;

		/**
		 * @return `true` if this backing is read-only and `false` if not.
		 */
		[[nodiscard]] virtual bool readonly() const noexcept = 0;

		/**
		 * @return The number of bytes available in this backing.
		 */
		[[nodiscard]] virtual std::uint64_t size() const noexcept = 0;

		/**
		 * @brief Retrieves a read-only raw byte array of this backing at the specified offset.
		 * @param offset The offset at which to start the returned array.
		 * @param size The number of bytes to include in the returned array.
		 * @return A read-only raw byte array into this backing.
		 */
		[[nodiscard]] virtual std::span<const std::uint8_t> array(std::uint64_t offset, std::uint64_t size) const = 0;

		/**
		 * @brief Reads size bytes beginning at offset into buf.
		 * @param buf A buffer to read into.
		 * @param offset The offset at which to start reading.
		 * @param size The number of bytes to read.
		 */
		virtual void read(void* buf, std::uint64_t offset, std::uint64_t size) const = 0;

		/**
		 * @brief Writes size bytes from buf into this backing beginning at offset.
		 * @param buf The data to write.
		 * @param offset The offset at which to start writing.
		 * @param size The number of bytes to write.
		 */
		virtual void write(const void* buf, std::uint64_t offset, std::uint64_t size) = 0;
	};

	/**
	 * @brief A data buffer implementation modeled after Java's ByteBuffer.
	 */
	class buffer {
	private:
		explicit buffer(std::shared_ptr<buffer_backing> backing);
		buffer(std::shared_ptr<buffer_backing> backing, std::uint64_t begin, std::uint64_t end);
		buffer(std::shared_ptr<buffer_backing> backing,
		       std::uint64_t begin,
		       std::uint64_t end,
		       std::uint64_t capacity,
		       std::uint64_t position);

		template <typename T>
		[[nodiscard]] T _get_t() {
			if (remaining() < sizeof(T))
				throw buffer_underflow_error {};

			T tmp {};
			_m_backing->read(&tmp, _m_backing_begin + _m_position, sizeof(T));
			_m_position += sizeof(T);
			return tmp;
		}

		template <typename T>
		[[nodiscard]] T _get_t(std::uint64_t index) const {
			if (index + sizeof(T) > limit())
				throw buffer_index_error {};

			T tmp {};
			_m_backing->read(&tmp, _m_backing_begin + index, sizeof(T));
			return tmp;
		}

		template <typename T>
		buffer& _put_t(T value) {
			if (remaining() < sizeof(T))
				throw buffer_overflow_error {};

			_m_backing->write(&value, _m_backing_begin + _m_position, sizeof(T));
			_m_position += sizeof(T);
			return *this;
		}

	public:
		/**
		 * @return The current position of this buffer
		 */
		[[nodiscard]] inline std::uint64_t position() const noexcept {
			return _m_position;
		}

		/**
		 * @brief Sets this buffer's position
		 * @param new_position The new position value
		 * @return This buffer.
		 */
		buffer& position(std::uint64_t new_position);

		/**
		 * @return The limit of this buffer
		 */
		[[nodiscard]] inline std::uint64_t limit() const noexcept {
			return _m_backing_end - _m_backing_begin;
		}

		/**
		 * @brief Sets this buffer's limit.
		 *
		 * If the position is larger than the new limit then it is set to the new limit.
		 *
		 * @param new_limit The new limit to set
		 * @return This buffer.
		 */
		buffer& limit(std::uint64_t new_limit);

		/**
		 * @brief Rewinds this buffer by setting the position to 0.
		 * @return This buffer.
		 */
		inline buffer& rewind() noexcept {
			return position(0);
		}

		/**
		 * @return The number of elements remaining in this buffer
		 */
		[[nodiscard]] inline std::uint64_t remaining() const noexcept {
			return limit() - _m_position;
		}

		/**
		 * @return The capacity of this buffer
		 */
		[[nodiscard]] inline std::uint64_t capacity() const noexcept {
			return _m_capacity;
		}

		/**
		 * @return `true` if the backing of this buffer is considered to be direct.
		 */
		[[nodiscard]] inline bool direct() const noexcept {
			return _m_backing->direct();
		}

		/**
		 * @return `true` if this buffer is read-only.
		 */
		[[nodiscard]] inline bool readonly() const noexcept {
			return _m_backing->readonly();
		}

		/**
		 * @brief Clears this buffer by resetting position and limit.
		 * @return This buffer.
		 */
		buffer& clear() noexcept;

		/**
		 * @brief Creates a new buffer which shares its content with this buffer.
		 *
		 * The new buffers position, capacity and limit will be the same as this buffer's current position,
		 * capacity and limit.
		 *
		 * @return The newly created buffer.
		 */
		[[nodiscard]] buffer duplicate() const;

		/**
		 * @brief Flips this buffer.
		 *
		 * Its limit is set to the current position, its current position is set to 0.
		 *
		 * @return This buffer.
		 */
		buffer& flip() noexcept;

		/**
		 * @brief Creates a new buffer which shares a subsequence of this buffer.
		 *
		 * The shared subsequence starts at the current position and ends at this buffer's limit.
		 *
		 * @return The newly created buffer.
		 */
		[[nodiscard]] buffer slice() const;

		/**
		 * @brief Creates a new buffer which shares a subsequence of this buffer.
		 *
		 * The shared subsequence starts at the given index in this buffer and contains the following size bytes.
		 *
		 * @param index The index in this buffer at which to start the new buffer.
		 * @param size The number of bytes the new buffer will encompass.
		 * @return The newly created buffer.
		 */
		[[nodiscard]] buffer slice(std::uint64_t index, std::uint64_t size) const;

		/**
		 * @return A read-only view into the raw contents of this buffer.
		 */
		[[nodiscard]] inline std::span<const std::uint8_t> array() const {
			return _m_backing->array(_m_backing_begin, _m_backing_end);
		}

		/**
		 * @brief Get bytes from the buffer, put them into buf and advance the position accordingly.
		 * @param buf The buffer to write into.
		 * @return This buffer.
		 */
		buffer& get(std::span<std::uint8_t> buf);

		/**
		 * @brief Get a value of type std::uint8_t from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint8_t get() {
			return _get_t<std::uint8_t>();
		}

		/**
		 * @brief Get a value of type char from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline char get_char() {
			return _get_t<char>();
		}

		/**
		 * @brief Get a value of type std::int16_t from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::int16_t get_short() {
			return _get_t<std::int16_t>();
		}

		/**
		 * @brief Get a value of type std::uint16_t from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint16_t get_ushort() {
			return _get_t<std::uint16_t>();
		}

		/**
		 * @brief Get a value of type std::int32_t from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::int32_t get_int() {
			return _get_t<std::int32_t>();
		}

		/**
		 * @brief Get a value of type std::uint32_t from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint32_t get_uint() {
			return _get_t<std::uint32_t>();
		}

		/**
		 *@brief  Get a value of type std::int64_t from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::int64_t get_long() {
			return _get_t<std::int64_t>();
		}

		/**
		 * @brief Get a value of type std::uint64_t from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint64_t get_ulong() {
			return _get_t<std::uint64_t>();
		}

		/**
		 * @brief Get a value of type float from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline float get_float() {
			return _get_t<float>();
		}

		/**
		 * @brief Get a value of type double from the buffer and advance the position accordingly.
		 * @return The value just read.
		 */
		[[nodiscard]] inline double get_double() {
			return _get_t<double>();
		}

		/**
		 * @brief Get bytes from the buffer, put them into buf.
		 * @param index The index at which to start reading.
		 * @param buf The buffer to write into.
		 * @return This buffer.
		 */
		const buffer& get(std::uint64_t index, std::span<std::uint8_t> buf) const;

		/**
		 * @brief Get a value of type std::uint8_t from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint8_t get(std::uint64_t index) const {
			return _get_t<std::uint8_t>(index);
		}

		/**
		 * @brief Get a value of type char from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline char get_char(std::uint64_t index) const {
			return _get_t<char>(index);
		}

		/**
		 * @brief Get a value of type std::int16_t from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::int16_t get_short(std::uint64_t index) const {
			return _get_t<std::int16_t>(index);
		}

		/**
		 * @brief Get a value of type std::uint16_t from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint16_t get_ushort(std::uint64_t index) const {
			return _get_t<std::uint16_t>(index);
		}

		/**
		 * @brief Get a value of type std::int32_t from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::int32_t get_int(std::uint64_t index) const {
			return _get_t<std::int32_t>(index);
		}

		/**
		 * @brief Get a value of type std::uint32_t from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint32_t get_uint(std::uint64_t index) const {
			return _get_t<std::uint32_t>(index);
		}

		/**
		 * @brief  Get a value of type std::int64_t from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::int64_t get_long(std::uint64_t index) const {
			return _get_t<std::int64_t>(index);
		}

		/**
		 * @brief Get a value of type std::uint64_t from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline std::uint64_t get_ulong(std::uint64_t index) const {
			return _get_t<std::uint64_t>(index);
		}

		/**
		 * @brief Get a value of type float from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline float get_float(std::uint64_t index) const {
			return _get_t<float>(index);
		}

		/**
		 * @brief Get a value of type double from the buffer.
		 * @param index The index at which to start reading.
		 * @return The value just read.
		 */
		[[nodiscard]] inline double get_double(std::uint64_t index) const {
			return _get_t<double>(index);
		}

		/**
		 * @brief Get a string of the given size from the buffer and advance the position accordingly
		 * @param size The number of characters to read.
		 * @return The string just read.
		 */
		[[nodiscard]] std::string get_string(std::uint64_t size);

		/**
		 * @brief Get a string of the given size from the buffer.
		 * @param index The index at which to start.
		 * @param size The number of characters to read.
		 * @return The string just read.
		 */
		[[nodiscard]] std::string get_string(std::uint64_t index, std::uint64_t size) const;

		/**
		 * @brief Get a line from the buffer and advance the position accordingly.
		 * @param skip_whitespace Set to `true` to skip whitespace characters immediately following the line.
		 * @return The line just read
		 * @see isspace
		 */
		[[nodiscard]] std::string get_line(bool skip_whitespace = true);

		/**
		 * @brief Get a line from the buffer.
		 * @param index The index at which to start.
		 * @return The line just read
		 * @see isspace
		 */
		[[nodiscard]] std::string get_line_at(std::uint64_t index) const;

		/**
		 * @brief Get a 2D-vector from the buffer.
		 * @return The vector just read
		 */
		[[nodiscard]] inline glm::vec2 get_vec2() {
			return {get_float(), get_float()};
		}

		/**
		 * @brief Get a 2D-vector from the buffer.
		 * @param index The index at which to start.
		 * @return The vector just read
		 */
		[[nodiscard]] inline glm::vec2 get_vec2(std::uint64_t index) const {
			return {get_float(index), get_float(index + sizeof(float))};
		}

		/**
		 * @brief Get a 3D-vector from the buffer.
		 * @return The vector just read
		 */
		[[nodiscard]] inline glm::vec3 get_vec3() {
			return {get_float(), get_float(), get_float()};
		}

		/**
		 * @brief Get a 3D-vector from the buffer.
		 * @param index The index at which to start.
		 * @return The vector just read
		 */
		[[nodiscard]] inline glm::vec3 get_vec3(std::uint64_t index) const {
			return {get_float(index), get_float(index + sizeof(float)), get_float(index + sizeof(float) * 2)};
		}

		/**
		 * @brief Get a 4D-vector from the buffer.
		 * @return The vector just read
		 */
		[[nodiscard]] inline glm::vec4 get_vec4() {
			return {get_float(), get_float(), get_float(), get_float()};
		}

		/**
		 * @brief Get a 4D-vector from the buffer.
		 * @param index The index at which to start.
		 * @return The vector just read
		 */
		[[nodiscard]] inline glm::vec4 get_vec4(std::uint64_t index) const {
			return {get_float(index),
			        get_float(index + sizeof(float)),
			        get_float(index + sizeof(float) * 2),
			        get_float(index + sizeof(float) * 3)};
		}

		/**
		 * @brief Finds and returns the relative index of the first mismatch between this buffer and the given buffer
		 * @param other The byte buffer to be tested for a mismatch with this buffer
		 * @return The relative index of the first mismatch between this and the given buffer, otherwise -1 if no
		 * mismatch.
		 */
		[[nodiscard]] std::int64_t mismatch(const buffer& other) const;

		/**
		 * @brief Finds and returns the relative index of the first mismatch between this buffer and the given callback
		 * @param other A function to be called for each char. If it return `true` it is considered a mismatch.
		 * @return The relative index of the first mismatch between this and the given buffer, otherwise -1 if no
		 * mismatch.
		 */
		[[nodiscard]] std::int64_t mismatch(const std::function<bool(std::uint8_t)>& each) const;

		/**
		 * @brief Finds and returns the relative index of the first mismatch between this buffer and the given buffer at
		 * the given index in this buffer
		 * @param index The index in this buffer to start comparing at.
		 * @param other The byte buffer to be tested for a mismatch with this buffer
		 * @return The relative index of the first mismatch between this and the given buffer, otherwise -1 if no
		 * mismatch.
		 */
		[[nodiscard]] std::int64_t mismatch(std::uint64_t index, const buffer& other) const;

		/**
		 * @brief Finds and returns the relative index of the first mismatch between this buffer and the given callback
		 * at the given index in this buffer
		 * @param index The index in this buffer to start comparing at.
		 * @param other A function to be called for each char. If it return `true` it is considered a mismatch.
		 * @return The relative index of the first mismatch between this and the given buffer, otherwise -1 if no
		 * mismatch.
		 */
		[[nodiscard]] std::int64_t mismatch(std::uint64_t index, const std::function<bool(std::uint8_t)>& each) const;

		/**
		 * @brief Put bytes from buf into the buffer and advance the position accordingly.
		 * @param buf The data to write.
		 * @return This buffer.
		 */
		buffer& put(std::span<const std::uint8_t> buf);

		/**
		 * @brief Put a value of type std::uint8_t into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put(std::uint8_t value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type char into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_char(char value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type std::int16_t into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_short(std::int16_t value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type std::uint16_t into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_ushort(std::uint16_t value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type std::int32_t into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_int(std::int32_t value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type std::uint32_t into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_uint(std::uint32_t value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type std::int64_t into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_long(std::int64_t value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type std::uint64_t into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_ulong(std::uint64_t value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type float into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_float(float value) {
			return _put_t(value);
		}

		/**
		 * @brief Put a value of type double into the buffer and advance the position accordingly
		 * @param value The value to put into the buffer
		 * @return This buffer.
		 */
		inline buffer& put_double(double value) {
			return _put_t(value);
		}

		/**
		 * @brief Put string into the buffer and advance the position accordingly
		 * @param str The string to put into the buffer.
		 * @return This buffer.
		 */
		buffer& put_string(std::string_view str);

		/**
		 * @brief Put string followed by <LF> into the buffer and advance the position accordingly
		 * @param str The string to put into the buffer.
		 * @return This buffer.
		 */
		buffer& put_line(std::string_view str);

		/**
		 * @brief Allocates a new buffer with the given size.
		 *
		 * The allocated buffer will be backed by a vector and thus it will reside on the heap. This backing
		 * vector can be accessed through #array at any time.
		 *
		 * @param size The number of bytes to allocate.
		 * @return The newly allocated buffer.
		 */
		static buffer allocate(std::uint64_t size);

		/**
		 * @brief Creates a new buffer from the given vector.
		 *
		 * The allocated buffer will be backed by the given vector and thus it will reside on the heap.
		 * The backing vector can be accessed through #array at any time.
		 *
		 * @param buf A vector containing the data to be wrapped into a buffer.
		 * @param readonly Set to `false` to be able to write to the buffer.
		 * @return The newly created buffer.
		 */
		static buffer wrap(std::vector<std::uint8_t>&& buf, bool readonly = true);

		/**
		 * @brief Opens the given file as a buffer.
		 *
		 * The backing of this buffer may be a memory mapping or a vector, in which case the file's contents
		 * are copied into memory. The chosen backing type can be queried using #direct. A raw byte view onto
		 * the loaded data is available through #array.
		 *
		 * @param path The path of the file to open.
		 * @param readonly Set to `false` to be able to write to the buffer.
		 * @return The newly created buffer.
		 */
		static buffer open(const std::string& path, bool readonly = true);

		/**
		 * @brief Returns a duplicate of the empty buffer.
		 * @return The empty buffer.
		 */
		static buffer empty();

	private:
		static std::unique_ptr<buffer> _m_empty;
		friend bool operator==(const buffer&, const buffer&);

		std::shared_ptr<buffer_backing> _m_backing;
		std::uint64_t _m_backing_begin, _m_backing_end;
		std::uint64_t _m_capacity;
		std::uint64_t _m_position;
	};
} // namespace phoenix
