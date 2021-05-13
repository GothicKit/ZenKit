// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>

#include <cstdint>
#include <ctime>
#include <string>
#include <string_view>
#include <vector>

namespace phoenix {
	static constexpr std::string_view VDF_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
	static constexpr std::string_view VDF_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";
	static constexpr uint32_t VDF_MASK_DIRECTORY = 0x80'00'00'00;
	static constexpr uint32_t VDF_MASK_LAST = 0x40'00'00'00;
	static constexpr uint32_t VDF_VERSION = 0x50;
	static constexpr uint32_t VDF_COMMENT_LENGTH = 256;
	static constexpr uint32_t VDF_SIGNATURE_LENGTH = 16;
	static constexpr uint32_t VDF_ENTRY_NAME_LENGTH = 64;

	/**
	 * @brief \brief Represents the header of a VDF.
	 */
	class vdf_header {
	public:
		/**
		 * @brief Constructs a new header with the given comment and timestamp.
		 * @param comment The comment to set. Note that comments are trimmed to 256 characters.
		 * @param timestamp The timestamp to set.#
		 */
		explicit vdf_header(std::string_view comment, std::time_t timestamp = -1);

		// < getters >

		/**
		 * @return The comment of the VDF.
		 */
		[[nodiscard]] inline const std::string& comment() const noexcept { return _m_comment; }

		/**
		 * @return The signature of the VDF.
		 */
		[[nodiscard]] inline const std::string& signature() const noexcept { return _m_signature; }

		/**
		 * @return The number of file entries in the VDF.
		 */
		[[nodiscard]] inline uint32_t file_count() const noexcept { return _m_file_count; }

		/**
		 * @return The total number of entries in the VDF.
		 */
		[[nodiscard]] inline uint32_t entry_count() const noexcept { return _m_entry_count; }

		/**
		 * @return The timestamp of the VDF.
		 */
		[[nodiscard]] inline std::time_t timestamp() const noexcept { return _m_timestamp; }

		/**
		 * @return The total size of the VDF in bytes.
		 */
		[[nodiscard]] inline uint32_t size() const noexcept { return _m_size; };

		/**
		 * @return The offset of the entry catalog in bytes.
		 */
		[[nodiscard]] inline uint32_t catalog_offset() const noexcept { return _m_catalog_offset; }

		/**
	     * @return The version of the VDF.
	     */
		[[nodiscard]] inline uint32_t version() const noexcept { return _m_version; }

	protected:
		/**
		 * @brief No-op constructor. It is used for #read but should not be used in other contexts.
		 */
		vdf_header() = default;

		/**
		 * @brief Reads a vdf_header from the given reader.
		 * @param in The reader to read from.
		 * @return The header read.
		 */
		static vdf_header read(reader& in);

		static constexpr const auto packed_size = VDF_COMMENT_LENGTH + VDF_SIGNATURE_LENGTH + 6 * sizeof(uint32_t);

	private:
		friend class vdf_file;

		std::string _m_comment;
		std::string _m_signature {VDF_SIGNATURE_G2};
		uint32_t _m_entry_count {0};
		uint32_t _m_file_count {0};
		std::time_t _m_timestamp {0};
		uint32_t _m_size {0};
		uint32_t _m_catalog_offset {0};
		uint32_t _m_version {VDF_VERSION};
	};

	/**
	 * @brief Represents an entry of a VDF.
	 */
	class vdf_entry {
	public:
		/**
		 * @brief Creates a new directory entry with the given name and attributes.
		 * @param name The name of the entry to create.
		 * @param attributes Attributes to set on the entry.
		 */
		explicit vdf_entry(std::string_view name, uint32_t attributes = 0);

		/**
		 * @brief Creates a new file entry with the given name, data and attributes.
		 * @param name The name of the entry to create.
		 * @param data The data of the entry
		 * @param size The number of bytes in the data.
		 * @param attributes Attributes to set on the entry.
		 */
		explicit vdf_entry(std::string_view name, const char* data, uint32_t size, uint32_t attributes = 0);

		/**
	     * @brief Searches the entry for the first child with the given name.
	     * @param name The name of the child to search for.
	     * @return The child with the given name or `nullptr` if no entry was found.
	     */
		[[nodiscard]] const vdf_entry* find_child(std::string_view name) const;

		/**
		 * @brief Resolves the given path into the entry.
		 * @param path The path to resolve.
		 * @return The child at the given path or `nullptr` if no entry was found.
		 */
		[[nodiscard]] const vdf_entry* resolve_path(std::string_view path) const;

		// < modification >

		/**
		 * @brief Searches the entry for the first child with the given name.
		 * @param name The name of the child to search for.
		 * @return The child with the give name or `nullptr` if no entry was found.
		 */
		vdf_entry* find_child(std::string_view name);

		// < getters >

		/**
	     * @return A list of child entries of the entry.
	     */
		[[nodiscard]] inline const std::vector<vdf_entry>& children() const noexcept { return _m_children; }

		/**
		 * @return The name of the entry.
		 */
		[[nodiscard]] inline const std::string& name() const noexcept { return _m_name; }

		/**
		 * @return The offset of the entry.
		 */
		[[nodiscard]] inline uint32_t offset() const noexcept { return _m_offset; }

		/**
	  	 * @return The size of the entry in bytes.
	  	 */
		[[nodiscard]] inline uint32_t size() const noexcept { return _m_size; }

		/**
		 * @return A new reader for the contents of the entry.
		 */
		[[nodiscard]] inline reader open() const noexcept { return _m_data; }

		/**
		 * @return The attributes of the file.
		 */
		[[nodiscard]] inline uint32_t attributes() const noexcept { return _m_attributes; }

		/**
		 * @return `true` if the entry is a file entry, `false` if not.
		 */
		[[nodiscard]] inline bool is_file() const noexcept { return !is_directory(); }

		/**
		 * @return `true` if the entry is a directory entry, `false` if not.
		 */
		[[nodiscard]] inline bool is_directory() const noexcept { return (_m_type & VDF_MASK_DIRECTORY) != 0; }

		/**
		 * @return `true` if the entry is the last entry in a directory, `false` if not.
		 */
		[[nodiscard]] inline bool is_last() const noexcept { return (_m_type & VDF_MASK_LAST) != 0; }

	protected:
		/**
		 * @brief No-op constructor. It is used for #read but should not be used in other contexts.
		 */
		vdf_entry() = default;

		/**
		 * @brief Reads a vdf_entry and all it's children from the given reader.
		 * @param in The reader to read from.
		 * @param catalog_offset The offset of the entry catalog.
		 * @return The entry read.
		 */
		static vdf_entry read(reader& in, uint32_t catalog_offset);

		static constexpr const auto packed_size = VDF_ENTRY_NAME_LENGTH + 4 * sizeof(uint32_t);

	private:
		friend class vdf_file;

		std::vector<vdf_entry> _m_children;
		std::string _m_name;
		reader _m_data;
		uint32_t _m_offset {0};
		uint32_t _m_size {0};
		uint32_t _m_type {0};
		uint32_t _m_attributes {0};
	};

	/**
	 * @brief Represents a VDF file (`.VDF` files).
	*/
	class vdf_file {
	public:
		/**
		 * @brief Opens the file at the given \p path as a VDF file.
		 * @param comment The comment on the file. Note that comments are trimmed to 256 characters.
		 * @param timestamp The timestamp of the archive.
		 */
		explicit vdf_file(std::string_view comment, std::time_t timestamp = -1);

		/**
		 * @brief Reads the header and catalog from a file and creates a vdf_file from it.
		 * @param path The path of the file to read from.
		 * @return The vdf_file.
		 */
		static vdf_file open(const std::string& path);

		/**
		 * @brief Searches the VDF file for the first entry with the given name.
		 * @param name The name of the entry to search for.
		 * @return The entry with the give name or `nullptr` if no entry was found.
		 */
		[[nodiscard]] const vdf_entry* find_entry(std::string_view name) const;

		/**
		 * @brief Resolves the given path into the VDF.
		 * @param path The path to resolve.
		 * @return The child at the given path or `nullptr` if no entry was found.
		 */
		[[nodiscard]] const vdf_entry* resolve_path(std::string_view path) const;

		// < modification >

		/**
		 * @brief Searches the VDF file for the first entry with the given name.
		 * @param name The name of the entry to search for.
		 * @return The entry with the give name or `nullptr` if no entry was found.
		 */
		vdf_entry* find_entry(std::string_view name);

		// < getters >

		/**
		 * @return The header data of the VDF file.
		 */
		[[nodiscard]] inline const vdf_header& header() const noexcept { return _m_header; }

		/**
		 * @return A list of root entries in the VDF file.
		 */
		[[nodiscard]] inline const std::vector<vdf_entry>& entries() const noexcept { return _m_entries; }

	protected:
		/**
		 * @brief No-op constructor. It is used for #open but should not be used in other contexts.
		 */
		vdf_file() = default;

	private:
		std::vector<vdf_entry> _m_entries;
		vdf_header _m_header;
	};

}// namespace phoenix