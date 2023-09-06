// Copyright © 2023 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include "buffer.hh"
#include "math.hh"

#include <filesystem>
#include <set>
#include <string>
#include <string_view>
#include <variant>

namespace phoenix {
	class VfsBrokenDiskError : public error {
	public:
		PHOENIX_INTERNAL explicit VfsBrokenDiskError(std::string const& signature);
	};

	class VfsFileExistsError : public error {
	public:
		PHOENIX_INTERNAL explicit VfsFileExistsError(std::string const& name);
	};

	class VfsNotFoundError : public error {
	public:
		PHOENIX_INTERNAL explicit VfsNotFoundError(std::string const& name);
	};

	enum class VfsNodeType {
		DIRECTORY = 1,
		FILE = 2,
	};

	class VfsNode;

	struct VfsNodeComparator {
		using is_transparent = std::true_type;

		PHOENIX_API [[nodiscard]] bool operator()(const VfsNode& a, const VfsNode& b) const noexcept;
		PHOENIX_API [[nodiscard]] bool operator()(const VfsNode& a, std::string_view b) const noexcept;
		PHOENIX_API [[nodiscard]] bool operator()(std::string_view a, const VfsNode& b) const noexcept;
	};

	class VfsNode {
	public:
		PHOENIX_API [[nodiscard]] VfsNodeType type() const noexcept;
		PHOENIX_API [[nodiscard]] std::time_t time() const noexcept;
		PHOENIX_API [[nodiscard]] std::string const& name() const noexcept;

		PHOENIX_API [[nodiscard]] std::vector<VfsNode> const& children() const;
		PHOENIX_API [[nodiscard]] VfsNode const* child(std::string_view name) const;
		PHOENIX_API [[nodiscard]] VfsNode* child(std::string_view name);

		PHOENIX_API VfsNode* create(VfsNode node);
		PHOENIX_API bool remove(std::string_view name);

		PHOENIX_API [[nodiscard]] buffer open() const;

		PHOENIX_API [[nodiscard]] static VfsNode directory(std::string_view name);
		PHOENIX_API [[nodiscard]] static VfsNode file(std::string_view name, buffer dev);

		PHOENIX_API [[nodiscard]] static VfsNode directory(std::string_view name, std::time_t ts);
		PHOENIX_API [[nodiscard]] static VfsNode file(std::string_view name, buffer dev, std::time_t ts);

	protected:
		PHOENIX_API explicit VfsNode(std::string_view name, std::time_t ts);
		PHOENIX_API explicit VfsNode(std::string_view name, buffer dev, std::time_t ts);

	private:
		std::string _m_name;
		std::time_t _m_time;
		std::variant<std::vector<VfsNode>, buffer> _m_data;
	};

	enum class VfsOverwriteBehavior {
		NONE = 0,  ///< Overwrite no conflicting nodes.
		ALL = 1,   ///< Overwrite all conflicting nodes.
		NEWER = 2, ///< Overwrite newer conflicting nodes.
		OLDER = 3, ///< Overwrite older conflicting nodes.
	};

	/// \brief An implementation of the virtual file system.
	class Vfs {
	public:
		PHOENIX_API Vfs();

		/// \brief Get the root node of the file system structure.
		/// \return The root node of the file system structure.
		PHOENIX_API [[nodiscard]] VfsNode const& root() const noexcept;

		/// \brief Create all missing directories in the given path.
		/// \param path The path of the directory to create.
		/// \return The newly created directory.
		/// \throws VfsFileExistsError if a part of the given path already exists and is a file.
		PHOENIX_API VfsNode& mkdir(std::string_view path);

		/// \brief Delete the file or directory at the given path
		/// \param path The path of the node to delete.
		/// \return `true` if removal was successful and `false` if not (ie. the file could not be found).
		PHOENIX_API bool remove(std::string_view path);

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
		PHOENIX_API void mount(VfsNode node, //
		                       std::string_view parent,
		                       VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::ALL);

		/// \brief Mount the disk file at the given host path into the file system.
		///
		/// The disk contents are mounted at the root node of the file system and existing
		/// directories are merged together.
		///
		/// \param path The path of the disk to mount.
		/// \param overwrite The behavior of the system when conflicting files are found.
		/// \throws VfsBrokenDiskError if the disk file is corrupted or invalid and thus, can't be loaded.
		/// \see #mount_disk(buffer, VfsOverwriteBehavior)
		PHOENIX_API void mount_disk(std::filesystem::path const& host,
		                            VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::OLDER);

		/// \brief Mount the disk file in the given buffer into the file system.
		///
		/// The disk contents are mounted at the root node of the file system and existing
		/// directories are merged together.
		///
		/// \param buf A buffer containing the disk file contents.
		/// \param overwrite The behavior of the system when conflicting files are found.
		/// \throws VfsBrokenDiskError if the disk file is corrupted or invalid and thus, can't be loaded.
		PHOENIX_API void mount_disk(buffer buf, VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::OLDER);

		/// \brief Mount a file or directory from the host file system into the Vfs.
		/// \note If a path to a directory is provided, only its children are mounted, not the directory itself.
		/// \param host The path of the file or directory to mount.
		/// \param parent The path of the parent node to mount into.
		/// \param overwrite The behavior of the system when conflicting files are found.
		/// \throws VfsNotFoundError if the given \p parent node could not be found.
		/// \throws VfsFileExistsError if the given parent node is not a directory.
		PHOENIX_API void mount_host(std::filesystem::path const& host,
		                            std::string_view parent,
		                            VfsOverwriteBehavior overwrite = VfsOverwriteBehavior::ALL);

		/// \brief Resolve the given path in the Vfs to a file system node.
		/// \param path The path to the node to resolve.
		/// \return The node at the given path or `nullptr` if the path could not be resolved.
		PHOENIX_API [[nodiscard]] VfsNode const* resolve(std::string_view path) const noexcept;

		/// \brief Resolve the given path in the Vfs to a file system node.
		/// \param path The path to the node to resolve.
		/// \return The node at the given path or `nullptr` if the path could not be resolved.
		PHOENIX_API [[nodiscard]] VfsNode* resolve(std::string_view path) noexcept;

		/// \brief Find the first node with the given name in the Vfs.
		/// \param name The name of the node to find.
		/// \return The node with the given name or `nullptr` if no node with the given name was found.
		PHOENIX_API [[nodiscard]] VfsNode const* find(std::string_view name) const noexcept;

		/// \brief Find the first node with the given name in the Vfs.
		/// \param name The name of the node to find.
		/// \return The node with the given name or `nullptr` if no node with the given name was found.
		PHOENIX_API [[nodiscard]] VfsNode* find(std::string_view name) noexcept;

	private:
		VfsNode _m_root;
	};
} // namespace phoenix
