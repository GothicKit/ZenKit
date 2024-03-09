// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "Library.hh"
#include "Mmap.hh"

#include "phoenix/buffer.hh"
#include "phoenix/phoenix.hh"

#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace zenkit {
	class VfsBrokenDiskError final : public Error {
	public:
		ZKINT explicit VfsBrokenDiskError(std::string const& signature);
	};

	class VfsFileExistsError final : public Error {
	public:
		ZKINT explicit VfsFileExistsError(std::string const& name);
	};

	class VfsNotFoundError final : public Error {
	public:
		ZKINT explicit VfsNotFoundError(std::string const& name);
	};

	enum class VfsNodeType {
		DIRECTORY = 1,
		FILE = 2,
	};

	struct VfsFileDescriptor {
		std::byte const* memory;
		std::size_t size;

		VfsFileDescriptor(std::byte const* mem, size_t len, bool del);
		VfsFileDescriptor(VfsFileDescriptor const& cpy);
		~VfsFileDescriptor() noexcept;

	private:
		size_t* refcnt;
	};

	class VfsNode;

	struct VfsNodeComparator {
		using is_transparent = std::true_type;

		ZKAPI [[nodiscard]] bool operator()(VfsNode const& a, VfsNode const& b) const noexcept;
		ZKAPI [[nodiscard]] bool operator()(VfsNode const& a, std::string_view b) const noexcept;
		ZKAPI [[nodiscard]] bool operator()(std::string_view a, VfsNode const& b) const noexcept;
	};

	class VfsNode {
		using ChildContainer = std::set<VfsNode, VfsNodeComparator>;

	public:
		[[nodiscard]] ZKAPI VfsNodeType type() const noexcept;
		[[nodiscard]] ZKAPI std::time_t time() const noexcept;
		[[nodiscard]] ZKAPI std::string const& name() const noexcept;

		[[nodiscard]] ZKAPI ChildContainer const& children() const;
		[[nodiscard]] ZKAPI VfsNode const* child(std::string_view name) const;
		[[nodiscard]] ZKAPI VfsNode* child(std::string_view name);

		ZKAPI VfsNode* create(VfsNode node);
		ZKAPI bool remove(std::string_view name);

		[[nodiscard]] ZKREM("use ::open_read()") ZKAPI phoenix::buffer open() const;
		[[nodiscard]] ZKAPI std::unique_ptr<Read> open_read() const;

		[[nodiscard]] ZKAPI static VfsNode directory(std::string_view name);
		[[nodiscard]] ZKREM("Deprecated") ZKAPI static VfsNode file(std::string_view name, phoenix::buffer dev);
		[[nodiscard]] ZKAPI static VfsNode file(std::string_view name, VfsFileDescriptor dev);

		[[nodiscard]] ZKAPI static VfsNode directory(std::string_view name, std::time_t ts);
		[[nodiscard]] ZKREM("Deprecated") ZKAPI static VfsNode
		    file(std::string_view name, phoenix::buffer dev, std::time_t ts);
		[[nodiscard]] ZKAPI static VfsNode file(std::string_view name, VfsFileDescriptor dev, std::time_t ts);

	protected:
		ZKAPI explicit VfsNode(std::string_view name, std::time_t ts);
		ZKREM("Deprecated") ZKAPI explicit VfsNode(std::string_view name, phoenix::buffer dev, std::time_t ts);
		ZKAPI explicit VfsNode(std::string_view name, VfsFileDescriptor dev, std::time_t ts);

	private:
		std::string _m_name;
		std::time_t _m_time;
		std::variant<ChildContainer, VfsFileDescriptor, phoenix::buffer> _m_data;
	};

	enum class VfsOverwriteBehavior {
		NONE = 0,  ///< Overwrite no conflicting nodes.
		ALL = 1,   ///< Overwrite all conflicting nodes.
		NEWER = 2, ///< Overwrite newer conflicting nodes.
		OLDER = 3, ///< Overwrite older conflicting nodes.
	};

	/// \brief An implementation of the virtual file system.
	/// \see https://zk.gothickit.dev/library/api/virtual-file-system/
	class Vfs {
	public:
		ZKAPI Vfs();

		/// \brief Get the root node of the file system structure.
		/// \return The root node of the file system structure.
		[[nodiscard]] ZKAPI VfsNode const& root() const noexcept;

		/// \brief Create all missing directories in the given path.
		/// \param path The path of the directory to create.
		/// \return The newly created directory.
		/// \throws VfsFileExistsError if a part of the given path already exists and is a file.
		ZKAPI VfsNode& mkdir(std::string_view path);

		/// \brief Delete the file or directory at the given path
		/// \param path The path of the node to delete.
		/// \return `true` if removal was successful and `false` if not (ie. the file could not be found).
		ZKAPI bool remove(std::string_view path);

		/// \brief Mount the given file system node into the given directory.
		///
		/// When the given \p node is a directory, it is merged with any existing directory with
		/// the same name in the given \p parent path.
		///
		/// \param node The file system node to mount.
		/// \param parent The path of the parent node to mount into.
		/// \param overwrite The behavior of the system when conflicting files are found.
		/// \throws VfsNotFoundError if the given \p parent node could not be found.
		/// \throws VfsFileExistsError if the given parent node is not a directory.
		ZKAPI void mount(VfsNode node, //
		                 std::string_view parent,
		                 VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::ALL);

		/// \brief Mount the disk file at the given host path into the file system.
		///
		/// The disk contents are mounted at the root node of the file system and existing
		/// directories are merged together.
		///
		/// \param host The path of the disk to mount.
		/// \param overwrite The behavior of the system when conflicting files are found.
		/// \throws VfsBrokenDiskError if the disk file is corrupted or invalid and thus, can't be loaded.
		/// \see #mount_disk(buffer, VfsOverwriteBehavior)
		ZKAPI void mount_disk(std::filesystem::path const& host,
		                      VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::OLDER);

		/// \brief Mount the disk file in the given buffer into the file system.
		///
		/// The disk contents are mounted at the root node of the file system and existing
		/// directories are merged together.
		///
		/// \param buf A buffer containing the disk file contents.
		/// \param overwrite The behavior of the system when conflicting files are found.
		/// \throws VfsBrokenDiskError if the disk file is corrupted or invalid and thus, can't be loaded.
		ZKREM("")
		ZKAPI void mount_disk(phoenix::buffer buf, VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::OLDER);
		ZKAPI void mount_disk(Read* buf, VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::OLDER);

		/// \brief Mount a file or directory from the host file system into the Vfs.
		/// \note If a path to a directory is provided, only its children are mounted, not the directory itself.
		/// \param host The path of the file or directory to mount.
		/// \param parent The path of the parent node to mount into.
		/// \param overwrite The behavior of the system when conflicting files are found.
		/// \throws VfsNotFoundError if the given \p parent node could not be found.
		/// \throws VfsFileExistsError if the given parent node is not a directory.
		ZKAPI void mount_host(std::filesystem::path const& host,
		                      std::string_view parent,
		                      VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::ALL);

		/// \brief Resolve the given path in the Vfs to a file system node.
		/// \param path The path to the node to resolve.
		/// \return The node at the given path or `nullptr` if the path could not be resolved.
		[[nodiscard]] ZKAPI VfsNode const* resolve(std::string_view path) const noexcept;

		/// \brief Resolve the given path in the Vfs to a file system node.
		/// \param path The path to the node to resolve.
		/// \return The node at the given path or `nullptr` if the path could not be resolved.
		[[nodiscard]] ZKAPI VfsNode* resolve(std::string_view path) noexcept;

		/// \brief Find the first node with the given name in the Vfs.
		/// \param name The name of the node to find.
		/// \return The node with the given name or `nullptr` if no node with the given name was found.
		[[nodiscard]] ZKAPI VfsNode const* find(std::string_view name) const noexcept;

		/// \brief Find the first node with the given name in the Vfs.
		/// \param name The name of the node to find.
		/// \return The node with the given name or `nullptr` if no node with the given name was found.
		[[nodiscard]] ZKAPI VfsNode* find(std::string_view name) noexcept;

		ZKAPI void save(Write* w, GameVersion version) const;

	private:
		ZKINT void mount_disk(std::byte const* buf, std::size_t size, VfsOverwriteBehavior overwrite);

		VfsNode _m_root;
		std::vector<std::unique_ptr<std::byte[]>> _m_data;

#ifdef _ZK_WITH_MMAP
		std::vector<Mmap> _m_data_mapped;
#endif
	};
} // namespace zenkit
