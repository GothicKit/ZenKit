// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>
#include <phoenix/math/bbox.hh>
#include <phoenix/phoenix.hh>

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <string>

namespace phoenix {
	enum class archive_format { binary, binsafe, ascii };

	struct archive_header {
		int version;
		std::string archiver;
		archive_format format;
		bool save {false};
		std::string user;
		std::string date;

		/**
		 * @brief Reads in an archive_header from the given reader.
		 * @param in The reader to read from.
		 * @return The header read.
		 * @throws parser_error if parsing fails.
		 */
		static archive_header parse(buffer& in);
	};

	struct archive_object {
		std::string object_name;
		std::string class_name;
		std::uint16_t version;
		std::uint32_t index;
	};

	/**
	 * @brief Reader for ZEN archives (`.ZEN` files).
	 *
	 * The reference implementation can be found on GitHub:
	 * https://github.com/ataulien/ZenLib/blob/e1a5e1b12e71690a5470f3be2aa3d0d6419f5191/zenload/zenParser.h and an
	 * adapted version used by OpenGothic was also referenced:
	 * https://github.com/Try/ZenLib/blob/732077c82589f5060d1762839293b996c8222c18/zenload/zenParser.cpp
	 *
	 * Thanks to the original author, Andre Taulien as well as Try for additional work on their
	 * ZenLib fork!
	 *
	 * @note To instantiate an archive_reader use #open,
	 */
	class archive_reader {
	public:
		/*
		 * @brief Constructs a new archive_reader from the given reader and header.
		 * @param in The reader to read from
		 * @param header The header of the archive.
		 */
		inline archive_reader(buffer& in, archive_header&& header) : header(std::move(header)), input(in) {}
		virtual ~archive_reader() = default;

		/**
		 * @brief Creates a new archive_reader from the given input reader.
		 * @param in The reader to use.
		 * @return The new archive_reader.
		 * @throws phoenix::parser_error
		 */
		static std::unique_ptr<archive_reader> open(buffer& in);

		/**
		 * @brief Tries to read the begin of a new object from the archive.
		 *
		 * If a beginning of an object could not be read, the internal reader is reverted to the state
		 * before the call of the method and the reader may be used as normal.
		 *
		 * @param[out] obj The object to store the data in.
		 * @return `true` if the begin of an object was read successfully, `false` if not.
		 */
		virtual bool read_object_begin(archive_object& obj) = 0;

		/**
		 * @brief Tries to read the end of an object from the archive.
		 *
		 * If a end of an object could not be read, the internal reader is reverted to the state
		 * before the call of the method and the reader may be used as normal.
		 *
		 * @return `true` if the end of an object was read successfully, `false` if not.
		 */
		virtual bool read_object_end() = 0;

		/**
		 * @brief Reads a string value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a string
		 */
		virtual std::string read_string() = 0;

		/**
		 * @brief Reads an integer value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not an integer
		 */
		virtual std::int32_t read_int() = 0;

		/**
		 * @brief Reads a float value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a float
		 */
		virtual float read_float() = 0;

		/**
		 * @brief Reads a byte value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a byte
		 */
		virtual std::uint8_t read_byte() = 0;

		/**
		 * @brief Reads a word (`uint16_t`) value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a word
		 */
		virtual std::uint16_t read_word() = 0;

		/**
		 * @brief Reads a enum (`uint32_t`) value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a enum
		 */
		virtual std::uint32_t read_enum() = 0;

		/**
		 * @brief Reads a bool value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a bool
		 */
		virtual bool read_bool() = 0;

		/**
		 * @brief Reads a glm::u8vec4 value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a color
		 */
		virtual glm::u8vec4 read_color() = 0;

		/**
		 * @brief Reads a vec3 value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a vec3
		 */
		virtual glm::vec3 read_vec3() = 0;

		/**
		 * @brief Reads a vec2 value from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a vec2
		 */
		virtual glm::vec2 read_vec2() = 0;

		/**
		 * @brief Reads a bounding box consisting of two consecutive vec3's from the reader.
		 * @return The value read.
		 * @throws parser_error if the value actually present is not a bounding box
		 */
		virtual bounding_box read_bbox() = 0;

		/**
		 * @brief Reads a 3-by-3 matrix from the reader.
		 * @return The matrix.
		 */
		virtual glm::mat3x3 read_mat3x3() = 0;

		/**
		 * @brief Reads a raw entry and returns the raw bytes stored within.
		 * @return A vector containing the raw bytes of the entry.
		 * @throws parser_error if the value actually present is not raw
		 */
		virtual buffer read_raw_bytes() = 0;

		/**
		 * @brief Skips the next object in the reader and all it's children
		 * @param skip_current Assume that the object to be skipped is the one currently read. If set to `false`
		 * assume to skip the object beginning at the current read offset.
		 */
		virtual void skip_object(bool skip_current);

		/**
		 * @return The header of the archive
		 */
		[[nodiscard]] const archive_header& get_header() const noexcept {
			return header;
		};

	protected:
		/**
		 * @brief Read the header of the specific archive format.
		 */
		virtual void read_header() = 0;

		/**
		 * @brief Skips the next entry in the reader.
		 */
		virtual void skip_entry() = 0;

	protected:
		archive_header header;
		buffer& input;
	};

	using archive_reader_ref = std::unique_ptr<archive_reader>;
} // namespace phoenix
