// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/types.hh>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <string_view>

namespace phoenix {
	/**
	 * @brief Facility to extract binary data from a memory buffer.
	 *
	 * Provides safe and simple methods to extract arithmetic types and strings from a binary data
	 * buffer or file. It provides a much simpler and more cohesive interface than a std::istream
	 * and supports all arithmetic types natively.
	 */
	class reader {
	public:
		/**
		 * @brief Creates a new reader with a size of `0`.
		 */
		reader() = default;

		/**
		 * @brief Creates a new reader from the given memory buffer.
		 * @param buffer The buffer to use for the new reader.
		 * @attention No data is copied. Any dynamically allocated data must be managed by the caller.
		 */
		explicit reader(std::string_view buffer);

		/**
		 * @brief Memory-maps the file at @p path and creates a reader from the resulting buffer.
		 * @return The new reader.
		 * @throws io_error if memory-mapping the file fails for any reason.
		 */
		static reader from(const std::string& path);

		/**
		 * @return The current read offset into the internal buffer.
		 */
		[[nodiscard]] inline u64 tell() const noexcept { return _m_offset; }

		/**
		 * @return The size of the internal buffer in bytes.
		 */
		[[nodiscard]] inline u64 size() const noexcept { return _m_buffer.size(); }

		/**
		 * @return The data of the internal buffer.
		 */
		[[nodiscard]] const char* data() const noexcept { return _m_buffer.data(); }

		/**
		 * @brief Set the read offset into the internal buffer to the given @p offset.
		 * @param offset The new read offset to set.
		 * @throws io_error if setting the given @p offset would be out-of-range of the reader.
		 * @see	#tell
		 * @see	#size
		 */
		void seek(u64 offset);

		/**
		 * @brief Ignores (or skips) @p count bytes in the internal buffer.
		 * @param count The number of bytes to ignore.
		 * @throws io_error if ignoring @p count bytes would result in an out-of-range read offset.
		 */
		void ignore(u64 count);

		/**
		 * @brief Creates a new reader from a subsection of this reader.
		 *
		 * If @p offset is set to `-1`, the new reader will be created at the current read offset
		 * and @p size bytes will be #ignore d.
		 *
		 * @param size The size of the subsection for the new reader.
		 * @param offset The offset into this reader to start the new reader at or `-1` (default).
		 * @return The new reader.
		 * @throws io_error if the new reader would be out-of-range of the internal buffer.
		 * @see	#ignore
		 * @see #tell
		 */
		reader fork(u64 size, s64 offset = -1);

		/**
		 * @brief Reads @p size bytes from the internal buffer into the given one.
		 * @param buffer The buffer to read into.
		 * @param size The number of bytes to read.
		 * @throws io_error if reading @p size bytes would be out-of-range.
		 */
		void read(void* buffer, u64 size);

		/**
		 * @brief Reads a string of @p size from the internal buffer.
		 * @param size The number of chars to read.
		 * @return The string read
		 * @throws io_error if reading a string of @p size chars would be out-of-range.
		 */
		std::string read_string(u64 size);

		/**
		 * @brief Reads a line from the internal buffer.
		 * @param skipws Set to `true` (default) to skip all whitespace after the line ends.
		 * @return The line read.
		 * @throws io_error if reading the line or skipping whitespace would be out-of-range.
		 */
		std::string read_line(bool skipws = true);

		/**
		 * @brief Reads a `uint8_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline u8 read_u8() { return _read_t<u8>(); }

		/**
		 * @brief Reads a `uint16_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline u16 read_u16() { return _read_t<u16>(); }

		/**
		 * @brief Reads a `uint32_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline u32 read_u32() { return _read_t<u32>(); }

		/**
		 * @brief Reads a `uint64_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline u64 read_u64() { return _read_t<u64>(); }

		/**
		 * @brief Reads a `int8_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline s8 read_s8() { return _read_t<s8>(); }

		/**
		 * @brief Reads a `int16_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline s16 read_s16() { return _read_t<s16>(); }

		/**
		 * @brief Reads a `int32_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline s32 read_s32() { return _read_t<s32>(); }

		/**
		 * @brief Reads a `int64_t` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline s64 read_s64() { return _read_t<s64>(); }

		/**
		 * @brief Reads a `float` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline f32 read_f32() { return _read_t<f32>(); }

		/**
		 * @brief Reads a `double` from the internal buffer.
		 * @return The value read.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		[[nodiscard]] inline f64 read_f64() { return _read_t<f64>(); }

		/**
		 * @brief Reads two `float`s from the internal buffer as a glm::vec2.
		 * @return Two `float`s as a vec2.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		inline glm::vec2 read_vec2() { return {read_f32(), read_f32()}; }

		/**
		 * @brief Reads three `float`s from the internal buffer as a glm::vec3.
		 * @return Three float`s as a vec3.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		inline glm::vec3 read_vec3() { return {read_f32(), read_f32(), read_f32()}; }

		/**
		 * @brief Reads four `float`s from the internal buffer as a glm::vec4.
		 * @return Four `float`s as a vec4.
		 * @throws io_error if reading the value would be out-of-range.
		 */
		inline glm::vec4 read_vec4() { return {read_f32(), read_f32(), read_f32(), read_f32()}; }

	protected:
		/**
		 * @brief Reads a value of type @p T from the internal buffer.
		 * @tparam T any arithmetic type
		 * @return The value read.
		 * @throws io_error if reading a value of type @p T would be out-of-range.
		 * @see	#read
		 */
		template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
		T _read_t() {
			T tmp {};
			read(&tmp, sizeof(T));
			return tmp;
		}

	private:
		std::string_view _m_buffer {};
		u64 _m_offset {0};
	};
}// namespace phoenix