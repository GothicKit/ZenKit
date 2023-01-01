// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/buffer.hh>
#include <phoenix/math.hh>
#include <phoenix/phoenix.hh>

#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace phoenix {
	enum class archive_format { binary, binsafe, ascii };

	/// \brief Represents the header of a ZenGin archive.
	struct archive_header {
		int32_t version;

		/// \brief The type of archiver used to create the archive. Either `zCArchiverGeneric` or `zCArchiverBinSafe`.
		std::string archiver;

		/// \brief The format of the archive.
		archive_format format;

		/// \brief Whether the archive contains a save-game or not.
		bool save {false};

		/// \brief The user who created the archive.
		std::string user;

		/// \brief The date this archive was created at.
		std::string date;

		/// \brief Reads in an archive_header from the given reader.
		/// \param in The reader to read from.
		/// \return The header read.
		/// \throws parser_error if parsing fails.
		[[nodiscard]] static archive_header parse(buffer& in);
	};

	/// \brief Represents the header of an object stored in a ZenGin archive.
	struct archive_object {
		/// \brief The name of the sub-object used for storing this object in the ZenGin.
		std::string object_name;

		/// \brief The original class name of the object in the ZenGin. Used to identify the type of object.
		std::string class_name;

		/// \brief A version identifier for the object. A way of determining the Gothic version from
		///        this has yet to be discovered.
		std::uint16_t version;

		/// \brief The index of the object in the archive. Unique for each object in an archive.
		std::uint32_t index;
	};

	enum class archive_entry_type : uint8_t {
		string = 0x1,
		int_ = 0x2,
		float_ = 0x3,
		byte = 0x4,
		word = 0x5,
		bool_ = 0x6,
		vec3 = 0x7,
		color = 0x8,
		raw = 0x9,
		raw_float = 0x10,
		enum_ = 0x11,
		hash = 0x12,
	};

	struct archive_entry {
		archive_entry_type type;
		std::string name;
		std::variant<std::string, int, float, uint8_t, uint16_t, bool, glm::vec3, glm::u8vec4, buffer, uint32_t> value;
	};

	struct archive_object_end {};

	using archive_visitor =
	    std::function<void(const std::optional<archive_object>&, const std::optional<archive_entry>&)>;

	/// \brief A reader for ZenGin archives.
	class archive_reader {
	public:
		/// \brief Constructs a new archive_reader from the given reader and header.
		/// \note This constructor should never be called explicitly. Use #open instead!
		/// \param in The reader to read from
		/// \param header The header of the archive.
		inline archive_reader(buffer& in, archive_header&& parent_header)
		    : header(std::move(parent_header)), input(in) {}

		virtual ~archive_reader() = default;

		/// \brief Creates a new archive_reader from the given buffer.
		/// \param[in,out] in The buffer to use.
		/// \return The new archive_reader.
		/// \throws phoenix::parser_error
		static std::unique_ptr<archive_reader> open(buffer& in);

		/// \brief Tries to read the begin of a new object from the archive.
		///
		/// If a beginning of an object could not be read, the internal buffer is reverted to the state
		/// before the call of the method and the reader may be used as normal.
		///
		/// \param[out] obj The object to store the data in.
		/// \return `true` if the begin of an object was read successfully, `false` if not.
		virtual bool read_object_begin(archive_object& obj) = 0;

		/// \brief Tries to read the end of an object from the archive.
		///
		/// If a end of an object could not be read, the internal buffer is reverted to the state
		/// before the call of the method and the reader may be used as normal.
		///
		/// \return `true` if the end of an object was read successfully, `false` if not.
		virtual bool read_object_end() = 0;

		/// \brief Reads a string value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a string
		virtual std::string read_string() = 0;

		/// \brief Reads an integer value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not an integer
		virtual std::int32_t read_int() = 0;

		/// \brief Reads a float value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a float
		virtual float read_float() = 0;

		/// \brief Reads a byte value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a byte
		virtual std::uint8_t read_byte() = 0;

		/// \brief Reads a word (`uint16_t`) value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a word
		virtual std::uint16_t read_word() = 0;

		/// \brief Reads a enum (`uint32_t`) value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a enum
		virtual std::uint32_t read_enum() = 0;

		/// \brief Reads a bool value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a bool
		virtual bool read_bool() = 0;

		/// \brief Reads a RGBA color value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a color
		virtual glm::u8vec4 read_color() = 0;

		/// \brief Reads a vec3 value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a vec3
		virtual glm::vec3 read_vec3() = 0;

		/// \brief Reads a vec2 value from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a vec2
		virtual glm::vec2 read_vec2() = 0;

		/// \brief Reads a bounding box consisting of two consecutive vec3's from the reader.
		/// \return The value read.
		/// \throws parser_error if the value actually present is not a bounding box
		virtual bounding_box read_bbox() = 0;

		/// \brief Reads a 3-by-3 matrix from the reader.
		/// \return The matrix.
		virtual glm::mat3x3 read_mat3x3() = 0;

		/// \brief Reads a raw entry and returns the raw bytes stored within.
		/// \return A vector containing the raw bytes of the entry.
		/// \throws parser_error if the value actually present is not raw
		[[deprecated("unsafe api: use read_raw_bytes(uint32_t) instead")]] virtual buffer read_raw_bytes() = 0;

		/// \brief Reads a raw entry and returns the raw bytes stored within.
		/// \param size The number of bytes to read (checked at runtime for ASCII and BIN_SAFE archives)
		/// \return A vector containing the raw bytes of the entry.
		/// \throws parser_error if the value actually present is not raw
		virtual buffer read_raw_bytes(uint32_t size) = 0;

		/// \brief Skips the next object in the reader and all it's children
		/// \param skip_current If `false` skips the next object in this buffer, otherwise skip the object
		///                     currently being read.
		virtual void skip_object(bool skip_current);

		/// \brief Dumps the current or next object of this reader as XML to standard out.
		/// \param open_object If `false`, dumps out the next object in the reader, otherwise dumps all
		///                    values until the current object closes.
		virtual void print_structure(bool open_object);

		/// \brief Get the next element in the archive.
		///
		/// Parses the next element, either an object begin, object end or entry from the archive and returns
		/// its associated value.
		///
		/// \return The value of the next element in the archive.
		/// \warning This API is unstable and may change at any time!
		virtual std::variant<archive_object, archive_object_end, archive_entry> unstable__next() = 0;

		/// \brief Walks the current or next object of the archive recursively, calling `cb` for each element.
		/// \param open_object If `false`, walks out the next object in the reader, otherwise walks all
		///                    values until the current object closes.
		/// \param cb A callback function to handle each element.
		/// \warning This API is unstable and may change at any time!
		void unstable__visit_objects(bool open_object, const archive_visitor& cb);

		/// \return The header of the archive
		[[nodiscard]] const archive_header& get_header() const noexcept {
			return header;
		}

		/// \return Whether or not this archive represents a save-game.
		[[nodiscard]] inline bool is_save_game() const noexcept {
			return header.save;
		}

	protected:
		/// \brief Read the header of the specific archive format.
		virtual void read_header() = 0;

		/// \brief Skips the next entry in the reader.
		virtual void skip_entry() = 0;

	protected:
		archive_header header;
		buffer& input;
	};
} // namespace phoenix
