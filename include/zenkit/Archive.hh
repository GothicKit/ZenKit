// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Boxes.hh"
#include "zenkit/Library.hh"
#include "zenkit/Stream.hh"

#include "phoenix/buffer.hh"

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

	enum class ArchiveFormat {
		BINARY = 0,
		BINSAFE = 1,
		ASCII = 2,

		binary ZKREM("renamed to ArchiveFormat::BINARY") = BINARY,
		binsafe ZKREM("renamed to ArchiveFormat::BINSAFE") = BINSAFE,
		ascii ZKREM("renamed to ArchiveFormat::ASCII") = ASCII,
	};

	/// \brief Represents the header of a ZenGin archive.
	///
	/// <p>The header contains general information about the archive, some of which is used during selection of the
	/// backend parser used to load the archive. Potentially interesting are the #save, #user and #date fields which
	/// can be used to attribute game saves.</p>
	///
	/// <p>If you only want to parse an archive's header without constructing an entire zenkit::ReadArchive, you can
	/// use #load to manually do so. It is recommended however, to prefer ReadArchive#load instead since the behaviour
	/// of #load might change without warning due to it being an internal API.</p>
	struct ArchiveHeader {
		/// \brief The format version of the archive.
		///
		/// <p>The version is always "1". Any other version identifiers are rejected.</p>
		int32_t version;

		/// \brief The type of archiver used to create the archive.
		///
		/// <p>Original Gothic archives contain either `"zCArchiverGeneric"` or `"zCArchiverBinSafe"`. Was originally
		/// used to determine which class should be used to load the archive, however *ZenKit* uses the
		/// #format to determine the backend parser implementation, which is more reliable.</p>
		std::string archiver;

		/// \brief The format of the archive.
		ArchiveFormat format;

		/// \brief Whether the archive contains a save-game or not.
		bool save;

		/// \brief The user who created the archive.
		///
		/// <p>Originally, this contained the name of the Windows user logged in while creating the archive. Save-games,
		/// for example, contain the name of the Windows user account which performed the save.</p>
		std::string user;

		/// \brief The date the archive was created at.
		///
		/// <p>This date is formatted according to German date formatting rules. The `strftime` format string would be
		/// `"%Y.%m.%d %H:%M:%S"`, however leading zeroes are stripped out.</p>
		std::string date;

		/// \brief Load an archive header from a stream.
		///
		/// <p>After loading finishes, the stream position of \p r is left at the end of the header. Parsing the
		/// header is a fallible operation which will throw an exception on failure (see below).</p>
		///
		/// \param[in] r The stream to read from.
		/// \throws zenkit::ParserError The header does not have a valid format.
		ZKAPI void load(Read* r);
	};

	/// \brief Represents the header of an object stored in a ZenGin archive.
	struct ArchiveObject {
		/// \brief The name of the sub-object used for storing this object in the ZenGin.
		std::string object_name;

		/// \brief The original class name of the object in the ZenGin. Used to identify the type of object.
		std::string class_name;

		/// \brief A version identifier for the object.
		std::uint16_t version;

		/// \brief The index of the object in the archive. Unique for each object in an archive.
		std::uint32_t index;
	};

	enum class ArchiveEntryType : uint8_t {
		STRING = 0x1,
		INTEGER = 0x2,
		FLOAT = 0x3,
		BYTE = 0x4,
		WORD = 0x5,
		BOOL = 0x6,
		VEC3 = 0x7,
		COLOR = 0x8,
		RAW = 0x9,
		RAW_FLOAT = 0x10,
		ENUM = 0x11,
		HASH = 0x12,

		string ZKREM("renamed to ArchiveEntryType::STRING") = STRING,
		int_ ZKREM("renamed to ArchiveEntryType::INTEGER") = INTEGER,
		float_ ZKREM("renamed to ArchiveEntryType::FLOAT") = FLOAT,
		byte ZKREM("renamed to ArchiveEntryType::BYTE") = BYTE,
		word ZKREM("renamed to ArchiveEntryType::WORD") = WORD,
		bool_ ZKREM("renamed to ArchiveEntryType::BOOL") = BOOL,
		vec3 ZKREM("renamed to ArchiveEntryType::VEC3") = VEC3,
		color ZKREM("renamed to ArchiveEntryType::COLOR") = COLOR,
		raw ZKREM("renamed to ArchiveEntryType::RAW") = RAW,
		raw_float ZKREM("renamed to ArchiveEntryType::RAW_FLOAT") = RAW_FLOAT,
		enum_ ZKREM("renamed to ArchiveEntryType::ENUM") = ENUM,
		hash ZKREM("renamed to ArchiveEntryType::HASH") = HASH,
	};

	/// \brief A reader for ZenGin archives.
	class ZKAPI ReadArchive {
	public:
		virtual ~ReadArchive() = default;

		/// \brief Create a new archive reader from the given buffer.
		/// \param[in] in The buffer to use.
		/// \return A unique pointer containing an archive reader.
		/// \throws zenkit::ParserError If the archive's header is invalid.
		/// \sa ArchiveHeader#load
		ZKREM("use ::from") static std::unique_ptr<ReadArchive> open(phoenix::buffer& in);

		/// \brief Creates a new archive_reader from the given buffer.
		/// \param[in] r The buffer to use.
		/// \return The new archive_reader.
		/// \throws zenkit::ParserError
		static std::unique_ptr<ReadArchive> from(Read* r);

		/// \brief Tries to read the begin of a new object from the archive.
		///
		/// If a beginning of an object could not be read, the internal buffer is reverted to the state
		/// before the call of the method and the reader may be used as normal.
		///
		/// \param[out] obj The object to store the data in.
		/// \return `true` if the begin of an object was read successfully, `false` if not.
		virtual bool read_object_begin(ArchiveObject& obj) = 0;

		/// \brief Tries to read the end of an object from the archive.
		///
		/// If a end of an object could not be read, the internal buffer is reverted to the state
		/// before the call of the method and the reader may be used as normal.
		///
		/// \return `true` if the end of an object was read successfully, `false` if not.
		virtual bool read_object_end() = 0;

		/// \brief Reads a string value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a string
		virtual std::string read_string() = 0;

		/// \brief Reads an integer value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not an integer
		virtual std::int32_t read_int() = 0;

		/// \brief Reads a float value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a float
		virtual float read_float() = 0;

		/// \brief Reads a byte value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a byte
		virtual std::uint8_t read_byte() = 0;

		/// \brief Reads a word (`uint16_t`) value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a word
		virtual std::uint16_t read_word() = 0;

		/// \brief Reads a enum (`uint32_t`) value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a enum
		virtual std::uint32_t read_enum() = 0;

		/// \brief Reads a bool value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a bool
		virtual bool read_bool() = 0;

		/// \brief Reads a RGBA color value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a color
		virtual glm::u8vec4 read_color() = 0;

		/// \brief Reads a vec3 value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a vec3
		virtual glm::vec3 read_vec3() = 0;

		/// \brief Reads a vec2 value from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a vec2
		virtual glm::vec2 read_vec2() = 0;

		/// \brief Reads a bounding box consisting of two consecutive vec3's from the reader.
		/// \return The value read.
		/// \throws zenkit::ParserError if the value actually present is not a bounding box
		virtual AxisAlignedBoundingBox read_bbox() = 0;

		/// \brief Reads a 3-by-3 matrix from the reader.
		/// \return The matrix.
		virtual glm::mat3x3 read_mat3x3() = 0;

		/// \brief Reads a raw entry and returns the raw bytes stored within.
		/// \param size The number of bytes to read (checked at runtime for ASCII and BIN_SAFE archives)
		/// \return A vector containing the raw bytes of the entry.
		/// \throws zenkit::ParserError if the value actually present is not raw
		ZKREM("use ::read_raw()") virtual phoenix::buffer read_raw_bytes(uint32_t size) = 0;

		virtual std::unique_ptr<Read> read_raw(std::size_t size) = 0;

		/// \brief Skips the next object in the reader and all it's children
		/// \param skip_current If `false` skips the next object in this buffer, otherwise skip the object
		///                     currently being read.
		virtual void skip_object(bool skip_current);

		/// \return The header of the archive
		[[nodiscard]] ArchiveHeader const& get_header() const noexcept {
			return header;
		}

		/// \return Whether or not this archive represents a save-game.
		[[nodiscard]] inline bool is_save_game() const noexcept {
			return header.save;
		}

	protected:
		ReadArchive(ArchiveHeader head, Read* read);
		ReadArchive(ArchiveHeader head, Read* read, std::unique_ptr<Read> owned);

		/// \brief Read the header of the specific archive format.
		virtual void read_header() = 0;

		/// \brief Skips the next entry in the reader.
		virtual void skip_entry() = 0;

	protected:
		ArchiveHeader header;
		Read* read;

	private:
		std::unique_ptr<Read> _m_owned;
	};
} // namespace zenkit
