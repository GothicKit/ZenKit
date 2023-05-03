// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include <phoenix/buffer.hh>

#include <cstdint>
#include <ctime>
#include <set>
#include <string>
#include <string_view>

namespace phoenix {
	static constexpr std::string_view VDF_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
	static constexpr std::string_view VDF_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";
	static constexpr std::uint32_t VDF_MASK_DIRECTORY = 0x80'00'00'00;
	static constexpr std::uint32_t VDF_MASK_LAST = 0x40'00'00'00;
	static constexpr std::uint32_t VDF_VERSION = 0x50;
	static constexpr std::uint32_t VDF_COMMENT_LENGTH = 256;
	static constexpr std::uint32_t VDF_SIGNATURE_LENGTH = 16;
	static constexpr std::uint32_t VDF_ENTRY_NAME_LENGTH = 64;

	/// \brief Converts a ``DOS`` timestamp into a unix timestamp.
	///
	/// This date format does not seem to be documented anywhere, so this implementation
	/// is based on the implementation in PhysicsFS
	/// ([here](https://github.com/icculus/physfs/blob/0d4e9aac4575744ddaae56b146f1be19f064f0e5/src/physfs_archiver_vdf.c#l37)).
	///
	/// \param dos The timestamp to convert.
	/// \return The converted unix timestamp.
	/// \note Assumes the ``DOS`` timestamp is in the ``GMT`` timezone.
	PHOENIX_API std::time_t dos_to_unix_time(std::uint32_t dos) noexcept;

	/// \brief Converts a unix timestamp (std::time_t) to a ``DOS`` timestamp.
	/// \param tm The unix timestamp to convert
	/// \return The unix timestamp as a ``DOS`` timestamp.
	/// \note This will convert to a ``DOS`` timestamp in the ``GMT`` timezone.
	PHOENIX_API std::uint32_t unix_time_to_dos(std::time_t tm) noexcept;

	/// \brief An exception thrown if the signature of a VDF file is not recognized.
	///
	/// If the signature is not recognized, this means that the VDF file was created using a 3rd-party tool
	/// like Union and is not necessarily compatible with the VDFS spec. Accepted signatures are
	/// phoenix::VDF_SIGNATURE_G1 and phoenix::VDF_SIGNATURE_G2.
	class vdfs_signature_error : public error {
	public:
		PHOENIX_INTERNAL explicit vdfs_signature_error(const std::string& signature);
	};

	/// \brief Represents the header of a VDF.
	class vdf_header {
	public:
		/// \brief Constructs a new header with the given comment and timestamp.
		/// \param comment The comment to set. Note that comments are trimmed to 256 characters.
		/// \param timestamp The timestamp to set.#
		PHOENIX_API explicit vdf_header(std::string_view comment, std::time_t timestamp = -1);

		PHOENIX_API vdf_header() = default;

		/// \brief Reads a vdf_header from the given buffer.
		/// \param in The reader to read from.
		/// \return The header read.
		PHOENIX_INTERNAL static vdf_header read(buffer& in);

		static constexpr const auto packed_size = VDF_COMMENT_LENGTH + VDF_SIGNATURE_LENGTH + 6 * sizeof(std::uint32_t);

	public:
		/// \brief The comment of the VDF.
		std::string comment;

		/// \brief The signature of the VDF.
		std::string signature {VDF_SIGNATURE_G2};

		/// \brief The number of file entries in the VDF.
		std::uint32_t entry_count {0};

		/// \brief The total number of entries in the VDF.
		std::uint32_t file_count {0};

		/// \brief The timestamp of the VDF.
		std::time_t timestamp {0};

		/// \brief The total size of the VDF in bytes.
		std::uint32_t size {0};

		/// \brief The offset of the entry catalog in bytes.
		std::uint32_t catalog_offset {0};

		/// \brief The version of the VDF.
		std::uint32_t version {VDF_VERSION};
	};

	class vdf_entry;

	struct vdf_entry_comparator {
	public:
		using is_transparent = std::true_type;

		PHOENIX_INTERNAL bool operator()(const vdf_entry& a, const vdf_entry& b) const;
		PHOENIX_INTERNAL bool operator()(const vdf_entry& a, std::string_view b) const;
		PHOENIX_INTERNAL bool operator()(std::string_view a, const vdf_entry& b) const;
	};

	/// \brief Represents an entry of a VDF.
	class vdf_entry {
	public:
		/// \brief Creates a new directory entry with the given name and attributes.
		/// \param name The name of the entry to create.
		/// \param attributes Attributes to set on the entry.
		PHOENIX_API explicit vdf_entry(std::string_view name, std::uint32_t attributes = 0);

		PHOENIX_API vdf_entry() = default;

		/// \brief Reads a vdf_entry and all it's children from the given buffer.
		/// \param in The buffer to read from.
		/// \param catalog_offset The offset of the entry catalog.
		/// \return The entry read.
		PHOENIX_INTERNAL static vdf_entry read(buffer& in, std::uint32_t catalog_offset);

		/// \brief Searches the entry for the first child with the given name.
		/// \param name The name of the child to search for.
		/// \return The child with the given name or `nullptr` if no entry was found.
		[[nodiscard]] PHOENIX_API const vdf_entry* find_child(std::string_view name) const;

		/// \brief Resolves the given path into the entry.
		/// \param path The path to resolve.
		/// \return The child at the given path or `nullptr` if no entry was found.
		[[nodiscard]] PHOENIX_API const vdf_entry* resolve_path(std::string_view path) const;

		/// \brief Searches the entry for the first child with the given name.
		/// \param name The name of the child to search for.
		/// \return The child with the give name or `nullptr` if no entry was found.
		PHOENIX_DEPRECATED("mutating vdf_entry children is broken!")
		PHOENIX_API vdf_entry* find_child(std::string_view name);

		/// \brief Merges the given VDF entry into this one.
		/// \param itm The entry to merge.
		/// \param override_existing Whether to replace existing files with the new ones.
		PHOENIX_API void merge(const vdf_entry& itm, bool override_existing = true);

		/// \return A new reader for the contents of the entry.
		[[nodiscard]] PHOENIX_API inline buffer open() const noexcept {
			return _m_data.duplicate();
		}

		/// \return `true` if the entry is a file entry, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_file() const noexcept {
			return !is_directory();
		}

		/// \return `true` if the entry is a directory entry, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_directory() const noexcept {
			return (type & VDF_MASK_DIRECTORY) != 0;
		}

		/// \return `true` if the entry is the last entry in a directory, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_last() const noexcept {
			return (type & VDF_MASK_LAST) != 0;
		}

		static constexpr const auto packed_size = VDF_ENTRY_NAME_LENGTH + 4 * sizeof(std::uint32_t);

	public:
		/// \brief The name of the entry.
		std::string name;

		/// \brief A list of child entries of the entry.
		std::set<vdf_entry, vdf_entry_comparator> children {};

		/// \brief The offset of the entry's data in the VDF.
		std::uint32_t offset {0};

		/// \brief The size of the entry in bytes.
		std::uint32_t size {0};

		/// \brief The type of the entry.
		std::uint32_t type {0};

		/// \brief The attributes of the file.
		std::uint32_t attributes {0};

	private:
		buffer _m_data = buffer::empty();
	};

	/// \brief Represents a VDF file (`.VDF` files).
	class vdf_file {
	public:
		/// \brief Opens the file at the given \p path as a VDF file.
		/// \param comment The comment on the file. Note that comments are trimmed to 256 characters.
		/// \param timestamp The timestamp of the archive.
		PHOENIX_DEPRECATED("scheduled for removal; use Vfs instead")
		PHOENIX_API explicit vdf_file(std::string_view comment, std::time_t timestamp = -1);

		/// \brief Reads the header and catalog from a file and creates a vdf_file from it.
		/// \param path The path of the file to read from.
		/// \return The vdf_file.
		PHOENIX_DEPRECATED("scheduled for removal; use Vfs instead")
		PHOENIX_API static vdf_file open(const std::filesystem::path& path);

		/// \brief Reads the header and catalog from a buffer and creates a vdf_file from it.
		/// \param path The buffer to read from.
		/// \return The vdf_file.
		PHOENIX_DEPRECATED("scheduled for removal; use Vfs instead")
		PHOENIX_API static vdf_file open(phoenix::buffer& buf);

		/// \brief Searches the VDF file for the first entry with the given name.
		/// \param name The name of the entry to search for.
		/// \return The entry with the give name or `nullptr` if no entry was found.
		[[nodiscard]] PHOENIX_API const vdf_entry* find_entry(std::string_view name) const;

		/// \brief Resolves the given path into the VDF.
		/// \param path The path to resolve.
		/// \return The child at the given path or `nullptr` if no entry was found.
		[[nodiscard]] PHOENIX_API const vdf_entry* resolve_path(std::string_view path) const;

		/// \brief Searches the VDF file for the first entry with the given name.
		/// \param name The name of the entry to search for.
		/// \return The entry with the give name or `nullptr` if no entry was found.
		PHOENIX_DEPRECATED("mutating vdf_entry children is broken!")
		PHOENIX_API vdf_entry* find_entry(std::string_view name);

		/// \brief Merges the given VDF file into this one.
		/// \param itm The file to merge.
		/// \param override_existing Whether to replace existing files with the new ones.
		PHOENIX_API void merge(const vdf_file& file, bool override_existing = true);

	public:
		/// \brief A list of root entries in the VDF file.
		std::set<vdf_entry, vdf_entry_comparator> entries;

		/// \brief The header data of the VDF file.
		vdf_header header;

	protected:
		vdf_file() = default;
	};

} // namespace phoenix
