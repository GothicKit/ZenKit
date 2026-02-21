// Copyright © 2023-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Vfs.hh"
#include "zenkit/Misc.hh"

#include "Internal.hh"
#include "zenkit/Error.hh"
#include "zenkit/Stream.hh"

#ifdef _ZK_WITH_ZIPPED_VDF
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#include <miniz.h>
#endif

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stack>

namespace zenkit {
	static constexpr std::string_view VFS_DISK_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
	static constexpr std::string_view VFS_DISK_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";
	static constexpr std::string_view VFS_DISK_SIGNATURE_VDFSTOOL = "PSVDSC_V2.00\x1A\x1A\x1A\x1A";

	/// Volume header flags (VolumeHeader.Flags in Union VDFS).
	/// These indicate whether file data within the VDF is stored compressed.
	/// The catalog/file table is always uncompressed regardless of this flag;
	/// only the actual file contents at each entry's offset are affected.
	static constexpr uint32_t VFS_VOLUME_FLAG_NORMAL = 0x50; ///< 80 — file data is stored uncompressed
	static constexpr uint32_t VFS_VOLUME_FLAG_ZIPPED = 0xA0; ///< 160 — file data is stored as ZippedStream blocks

	VfsBrokenDiskError::VfsBrokenDiskError(std::string const& signature)
	    : Error("VFS disk signature not recognized: \"" + signature + "\"") {}

	VfsFileExistsError::VfsFileExistsError(std::string const& name) : Error("file exists: \"" + name + "\"") {}

	VfsNotFoundError::VfsNotFoundError(std::string const& name) : Error("not found: \"" + name + "\"") {}

	VfsFileDescriptor::VfsFileDescriptor(std::byte const* mem, size_t len, bool del, bool zip, size_t raw)
	    : memory(mem), size(len), raw_size(raw == 0 ? len : raw), zipped(zip), refcnt(del ? new size_t(1) : nullptr) {}

	VfsFileDescriptor::VfsFileDescriptor(VfsFileDescriptor const& cpy)
	    : memory(cpy.memory), size(cpy.size), raw_size(cpy.raw_size), zipped(cpy.zipped), refcnt(cpy.refcnt) {
		if (this->refcnt == nullptr) return;
		*this->refcnt += 1;
	}

	VfsFileDescriptor::~VfsFileDescriptor() noexcept {
		if (this->refcnt == nullptr) return;
		*this->refcnt -= 1;

		if (*this->refcnt == 0) {
			delete[] memory;
			delete this->refcnt;
		}
	}

	bool VfsNodeComparator::operator()(VfsNode const& a, VfsNode const& b) const noexcept {
		return icompare(a.name(), b.name());
	}

	bool VfsNodeComparator::operator()(VfsNode const& a, std::string_view b) const noexcept {
		return icompare(a.name(), b);
	}

	bool VfsNodeComparator::operator()(std::string_view a, VfsNode const& b) const noexcept {
		return icompare(a, b.name());
	}

	VfsNode::VfsNode(std::string_view name, time_t ts) : _m_name(name), _m_time(ts), _m_data(ChildContainer {}) {}

	VfsNode::VfsNode(std::string_view name, VfsFileDescriptor dev, time_t ts)
	    : _m_name(name), _m_time(ts), _m_data(dev) {}

	VfsNode::ChildContainer const& VfsNode::children() const {
		return std::get<ChildContainer>(_m_data);
	}

	std::string_view trim_trailing_whitespace(std::string_view s) {
		while (!s.empty() && std::isspace(s.back())) {
			s = s.substr(0, s.size() - 1);
		}

		return s;
	}

	VfsNode const* VfsNode::child(std::string_view name) const {
		auto& children = std::get<ChildContainer>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = children.find(name);
		if (it == children.end() || !iequals(it->name(), name)) return nullptr;
		return &*it;
	}

	VfsNode* VfsNode::child(std::string_view name) {
		auto& children = std::get<ChildContainer>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = children.find(name);
		if (it == children.end() || !iequals(it->name(), name)) return nullptr;
		return const_cast<VfsNode*>(&*it);
	}

	VfsNode* VfsNode::create(VfsNode node) {
		this->remove(node.name());

		auto& children = std::get<ChildContainer>(_m_data);
		auto it = children.insert(std::move(node));
		return const_cast<VfsNode*>(&*it.first);
	}

	bool VfsNode::remove(std::string_view name) {
		auto& children = std::get<ChildContainer>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = children.find(name);
		if (it == children.end() || !iequals(it->name(), name)) return false;

		children.erase(it);
		return true;
	}

	std::unique_ptr<Read> VfsNode::open_read() const {
		auto fd = std::get<VfsFileDescriptor>(_m_data);
		auto reader = Read::from(fd.memory, fd.size);

#ifdef _ZK_WITH_ZIPPED_VDF
		if (fd.zipped) {
			auto zipped = Read::from_zipped(std::move(reader));
			if (zipped != nullptr) {
				return zipped;
			}

			// ZippedStream header validation failed (e.g. raw Ogg Vorbis audio).
			// Fall back to raw data using the catalog entry size.
			return Read::from(fd.memory, fd.raw_size);
		}
#endif

		return reader;
	}

	VfsNode VfsNode::directory(std::string_view name) {
		return directory(name, -1);
	}

	VfsNode VfsNode::file(std::string_view name, VfsFileDescriptor dev) {
		return file(name, dev, -1);
	}

	VfsNode VfsNode::directory(std::string_view name, std::time_t ts) {
		return VfsNode(name, ts);
	}

	VfsNode VfsNode::file(std::string_view name, VfsFileDescriptor dev, std::time_t ts) {
		return VfsNode(name, dev, ts);
	}

	VfsNodeType VfsNode::type() const noexcept {
		return std::holds_alternative<VfsFileDescriptor>(_m_data) ? VfsNodeType::FILE : VfsNodeType::DIRECTORY;
	}

	std::time_t VfsNode::time() const noexcept {
		return _m_time;
	}

	std::string const& VfsNode::name() const noexcept {
		return _m_name;
	}

	Vfs::Vfs() : _m_root(VfsNode::directory("/")) {}

	VfsNode const* Vfs::resolve(std::string_view path) const noexcept {
		auto* context = &_m_root;

		while (context != nullptr && !path.empty()) {
			auto next = path.find('/');
			if (next == 0) {
				path = path.substr(next + 1);
				continue;
			}

			auto name = path.substr(0, next);
			context = context->child(name);

			if (next == std::string_view::npos) break;

			path = path.substr(next + 1);
		}

		return context;
	}

	VfsNode const* Vfs::find(std::string_view name) const noexcept {
		std::stack<VfsNode const*> tree {{&_m_root}};

		while (!tree.empty()) {
			auto* node = tree.top();
			tree.pop();

			if (auto* child = node->child(name); child != nullptr) return child;

			auto& children = node->children();
			for (auto const& x : children) {
				if (x.type() == VfsNodeType::FILE) continue;

				tree.push(&x);
			}
		}

		return nullptr;
	}

	VfsNode* Vfs::resolve(std::string_view path) noexcept {
		return const_cast<VfsNode*>(const_cast<Vfs const*>(this)->resolve(path));
	}

	VfsNode* Vfs::find(std::string_view name) noexcept {
		return const_cast<VfsNode*>(const_cast<Vfs const*>(this)->find(name));
	}

	static uint32_t count_nodes(VfsNode const* node) {
		uint32_t count = 1; /* self */

		if (node->type() == VfsNodeType::DIRECTORY) {
			for (auto& child : node->children()) {
				count += count_nodes(&child);
			}
		}

		return count;
	}

	static std::uint32_t vfs_unix_to_dos_time(std::time_t unix_) noexcept {
		tm t = *gmtime(&unix_);

		uint32_t dos = 0;
		dos |= (t.tm_year - 80) << 25;
		dos |= (t.tm_mon + 1) << 21;
		dos |= (t.tm_mday) << 16;
		dos |= (t.tm_hour) << 11;
		dos |= (t.tm_min) << 5;
		dos |= (t.tm_sec / 2);

		return dos;
	}

#ifdef _ZK_WITH_ZIPPED_VDF
	/// Default ZippedStream block size (8 KB), matching Union's default.
	static constexpr uint32_t VFS_ZIPPED_BLOCK_SIZE = 8192;

	/// Returns true if the file name has a .WAV or .OGG extension (case-insensitive).
	/// These files are stored uncompressed in zipped VDFs, following Union's convention.
	static bool vfs_is_wave_file(std::string_view name) {
		if (name.size() < 4) return false;
		auto ext = name.substr(name.size() - 4);
		return iequals(ext, ".wav") || iequals(ext, ".ogg");
	}

	/// Writes file data as a ZippedStream to the output.
	///
	/// ZippedStream layout:
	///   Stream header:  Length (4) | BlockSize (4) | BlocksCount (4)
	///   Per block (interleaved):
	///     Block header:  LengthSource (4) | LengthCompressed (4) | BlockSize (4)
	///     Block data:    [LengthCompressed bytes of zlib-compressed data]
	static void vfs_write_zipped(Write* w, std::byte const* data, size_t size) {
		uint32_t block_size = VFS_ZIPPED_BLOCK_SIZE;
		uint32_t blocks_count = static_cast<uint32_t>((size + block_size - 1) / block_size);

		// Write stream header
		w->write_uint(static_cast<uint32_t>(size)); // Length (uncompressed)
		w->write_uint(block_size);                  // BlockSize
		w->write_uint(blocks_count);                // BlocksCount

		// Write each block: header + compressed data (interleaved)
		std::vector<uint8_t> cmp_buf;
		for (uint32_t i = 0; i < blocks_count; i++) {
			size_t src_offset = static_cast<size_t>(i) * block_size;
			uint32_t src_len = static_cast<uint32_t>(std::min<size_t>(block_size, size - src_offset));

			mz_ulong cmp_len = mz_compressBound(src_len);
			cmp_buf.resize(cmp_len);

			int res =
			    mz_compress(cmp_buf.data(), &cmp_len, reinterpret_cast<uint8_t const*>(data + src_offset), src_len);
			assert(res == MZ_OK && "mz_compress failed with a correctly sized buffer — this is a bug");

			// Block header
			w->write_uint(src_len);                        // LengthSource
			w->write_uint(static_cast<uint32_t>(cmp_len)); // LengthCompressed
			w->write_uint(block_size);                     // BlockSize

			// Block data
			w->write(cmp_buf.data(), cmp_len);
		}
	}

  void Vfs::save_compressed(Write* w, GameVersion version, time_t unix_t) const {
		save_internal(w, version, unix_t, true);
	}
#endif // _ZK_WITH_ZIPPED_VDF

	void Vfs::save(Write* w, GameVersion version, time_t unix_t) const {
		save_internal(w, version, unix_t, false);
	}

	void Vfs::save_internal(Write* w, GameVersion version, time_t unix_t, bool compressed) const {
		std::vector<std::byte> catalog;
		auto write_catalog = Write::to(&catalog);

		// Skip the header, we'll write it at the end.
		unsigned header_size = 256 + 16 + 6 * 4;
		unsigned catalog_size = (count_nodes(&_m_root) - 1) * (64 + 4 * 4); // -1 because the root node is not counted
		w->seek(header_size + catalog_size, Whence::BEG);

		std::vector<std::byte> cache;
		std::string name;
		uint32_t index = 0;
		uint32_t files = 0;

		std::function<void(VfsNode const*)> write_node = [&](VfsNode const* node) {
			unsigned i = 0;
			std::vector<std::pair<uint32_t, VfsNode const*>> dirs;

			for (auto& child : node->children()) {
				name = child.name();
				name.resize(64, '\x20');

				write_catalog->write_string(name);

				if (child.type() == VfsNodeType::FILE) {
					auto rd = child.open_read();
					rd->seek(0, Whence::END);
					auto sz = rd->tell();
					rd->seek(0, Whence::BEG);

					cache.resize(sz);
					rd->read(cache.data(), sz);

					write_catalog->write_uint(w->tell()); // Offset
					write_catalog->write_uint(sz);        // Size (always uncompressed)
					write_catalog->write_uint(i + 1 == node->children().size() ? 0x40000000 : 0); // Type

#ifdef _ZK_WITH_ZIPPED_VDF
					if (compressed && !vfs_is_wave_file(child.name())) {
						vfs_write_zipped(w, cache.data(), sz);
					} else {
						w->write(cache.data(), sz);
					}
#else
					w->write(cache.data(), sz);
#endif

					files += 1;
				} else {
					dirs.emplace_back(write_catalog->tell(), &child);
					write_catalog->write_uint(0);                                                          // Offset
					write_catalog->write_uint(0);                                                          // Size
					write_catalog->write_uint(i + 1 == node->children().size() ? 0xC0000000 : 0x80000000); // Type
				}

				write_catalog->write_uint(0); // Attributes

				i++;
				index++;
			}

			for (auto [off, dir] : dirs) {
				auto here = static_cast<ssize_t>(write_catalog->tell());
				write_catalog->seek(off, Whence::BEG);
				write_catalog->write_uint(index);
				write_catalog->seek(here, Whence::BEG);

				write_node(dir);
			}
		};

		write_node(&_m_root);

		// Write the header
		std::string comment = "Created using ZenKit";
		comment.resize(256, '\x1A');

		auto off = w->tell();
		w->seek(0, Whence::BEG);
		w->write_string(comment);
		w->write_string(version == GameVersion::GOTHIC_1 ? VFS_DISK_SIGNATURE_G1 : VFS_DISK_SIGNATURE_G2);
		w->write_uint(index);
		w->write_uint(files);
		w->write_uint(unix_t == 0 ? vfs_unix_to_dos_time(time(nullptr)) : vfs_unix_to_dos_time(unix_t));
		w->write_uint(off + catalog.size());
		w->write_uint(header_size);
#ifdef _ZK_WITH_ZIPPED_VDF
		w->write_uint(compressed ? VFS_VOLUME_FLAG_ZIPPED : VFS_VOLUME_FLAG_NORMAL);
#else
		w->write_uint(VFS_VOLUME_FLAG_NORMAL);
#endif
		w->seek(static_cast<ssize_t>(header_size), Whence::BEG);
		w->write(catalog.data(), catalog.size());
	}

	void Vfs::mount_disk(std::filesystem::path const& host, VfsOverwriteBehavior overwrite) {
#ifdef _ZK_WITH_MMAP
		auto& mem = _m_data_mapped.emplace_back(host);
		this->mount_disk(mem.data(), mem.size(), overwrite);
#else
		std::ifstream stream {host, std::ios::in | std::ios::ate | std::ios::binary};
		auto size = stream.tellg();
		stream.seekg(0);

		auto& data = _m_data.emplace_back(new std::byte[(size_t) size]);
		stream.read((char*) data.get(), size);
		this->mount_disk(data.get(), (size_t) size, overwrite);
#endif
	}

	static std::time_t vfs_dos_to_unix_time(std::uint32_t dos) noexcept {
		tm t {};

		t.tm_year = static_cast<int32_t>((dos >> 25) & 0x7F) + 80;
		t.tm_mon = static_cast<int32_t>((dos >> 21) & 0xF) - 1;
		t.tm_mday = static_cast<int32_t>((dos >> 16) & 0x1F);
		t.tm_hour = static_cast<int32_t>((dos >> 11) & 0x1F);
		t.tm_min = static_cast<int32_t>((dos >> 5) & 0x3F);
		t.tm_sec = static_cast<int32_t>((dos >> 0) & 0x1F) * 2;

		return mktime(&t);
	}

	void Vfs::mount_disk(Read* buf, VfsOverwriteBehavior overwrite) {
		buf->seek(0, Whence::END);
		auto size = buf->tell();
		buf->seek(0, Whence::BEG);

		auto mem = std::make_unique<std::byte[]>(size);
		buf->read(mem.get(), size);

		this->mount_disk(mem.get(), size, overwrite);
		_m_data.push_back(std::move(mem));
	}

	VfsNode const& Vfs::root() const noexcept {
		return _m_root;
	}

	void Vfs::mount(VfsNode node, std::string_view parent, VfsOverwriteBehavior overwrite) {
		VfsNode* pNode = this->resolve(parent);
		if (pNode == nullptr) {
			throw VfsNotFoundError {std::string {parent}};
		}

		if (pNode->type() != VfsNodeType::DIRECTORY) {
			throw VfsFileExistsError {std::string {parent}};
		}

		std::stack<VfsNode*> sourceTree {{&node}};
		std::stack<VfsNode*> destTree {{pNode}};

		while (!sourceTree.empty()) {
			VfsNode* source = sourceTree.top();
			sourceTree.pop();

			VfsNode* dest = destTree.top();
			destTree.pop();

			VfsNode* existing = dest->child(source->name());
			if (existing == nullptr) {
				dest->create(std::move(*source));
				continue;
			}

			if (existing->type() == VfsNodeType::FILE || source->type() == VfsNodeType::FILE) {
				switch (overwrite) {
				case VfsOverwriteBehavior::NONE:
					continue;
				case VfsOverwriteBehavior::ALL:
					break;
				case VfsOverwriteBehavior::NEWER:
					if (existing->time() <= source->time()) continue;
					break;
				case VfsOverwriteBehavior::OLDER:
					if (existing->time() >= source->time()) continue;
					break;
				}

				dest->remove(existing->name());
				dest->create(std::move(*source));
			} else {
				// Both are directories, merge them.
				for (auto& child : source->children()) {
					sourceTree.push(const_cast<VfsNode*>(&child));
					destTree.push(existing);
				}
			}
		}
	}

	VfsNode& Vfs::mkdir(std::string_view path) {
		auto* context = &_m_root;

		while (!path.empty()) {
			auto next = path.find('/');
			if (next == 0) {
				path = path.substr(next + 1);
				continue;
			}

			auto name = path.substr(0, next);

			if (auto it = context->child(name); it == nullptr) {
				auto now = std::chrono::system_clock::now();
				context = context->create(VfsNode::directory(name, std::chrono::system_clock::to_time_t(now)));
			} else if (it->type() == VfsNodeType::FILE) {
				throw VfsFileExistsError {std::string {name}};
			} else {
				context = it;
			}

			if (next == std::string_view::npos) break;
			path = path.substr(next + 1);
		}

		return *context;
	}

	bool Vfs::remove(std::string_view path) {
		auto lastSlash = path.rfind('/');
		auto parentPath = path.substr(0, lastSlash == std::string_view::npos ? 0 : lastSlash);
		auto childName = path.substr(lastSlash + 1);

		VfsNode* pNode = this->resolve(parentPath);
		if (pNode == nullptr) return false;
		return pNode->remove(childName);
	}

	void Vfs::mount_host(std::filesystem::path const& sourcePath,
	                     std::string_view mountPoint,
	                     VfsOverwriteBehavior overwrite) {
		auto root = VfsNode::directory(sourcePath.filename().string());

		std::function<void(VfsNode*, std::filesystem::path const&)> load_directory =
		    [this, &load_directory](VfsNode* parent, std::filesystem::path const& host) {
			    for (auto const& ref : std::filesystem::directory_iterator(host)) {
				    auto const& path = ref.path();
				    auto time =
				        std::chrono::duration_cast<std::chrono::seconds>(ref.last_write_time().time_since_epoch());

				    if (ref.is_directory()) {
					    VfsNode* newP = parent->create(VfsNode::directory(path.filename().string(), time.count()));
					    load_directory(newP, path);
				    } else if (ref.file_size() > 0) {
#ifdef _ZK_WITH_MMAP
					    auto& mem = this->_m_data_mapped.emplace_back(path);
					    parent->create(VfsNode::file(path.filename().string(),
					                                 VfsFileDescriptor {mem.data(), mem.size(), false},
					                                 time.count()));
#else
					    std::ifstream stream {path, std::ios::in | std::ios::ate | std::ios::binary};
					    auto size = stream.tellg();
					    stream.seekg(0);

					    auto& data = _m_data.emplace_back(new std::byte[(size_t) size]);
					    stream.read((char*) data.get(), size);

					    parent->create(VfsNode::file(path.filename().string(),
					                                 VfsFileDescriptor {data.get(), static_cast<size_t>(size), false},
					                                 time.count()));
#endif
				    }
			    }
		    };

		load_directory(&root, sourcePath);

		for (auto& child : root.children()) {
			this->mount(child, mountPoint, overwrite);
		}
	}

	void Vfs::mount_disk(std::byte const* buf, std::size_t size, VfsOverwriteBehavior overwrite) {
		auto r = Read::from(buf, size);

		auto comment = r->read_string(256);
		auto signature = r->read_string(16);
		[[maybe_unused]] auto entry_count = r->read_uint();
		[[maybe_unused]] auto file_count = r->read_uint();
		auto timestamp = vfs_dos_to_unix_time(r->read_uint());
		[[maybe_unused]] auto archive_size = r->read_uint();
		auto catalog_offset = r->read_uint();
		auto volume_flags = r->read_uint();

		bool zipped = false;
		if (volume_flags == VFS_VOLUME_FLAG_NORMAL) {
			zipped = false;
		} else if (volume_flags == VFS_VOLUME_FLAG_ZIPPED) {
#ifdef _ZK_WITH_ZIPPED_VDF
			zipped = true;
#else
			throw VfsBrokenDiskError {"Detected compressed VDF (build ZenKit with ZK_ENABLE_ZIPPED_VDF=ON to enable support)"};
#endif
		} else {
			ZKLOGW("Vfs", "Unknown volume flags: 0x%X (%u), assuming uncompressed", volume_flags, volume_flags);
		}

		if (signature == VFS_DISK_SIGNATURE_VDFSTOOL) {
			ZKLOGI("Vfs", "VDFS tool disk detected");
		} else if (signature == VFS_DISK_SIGNATURE_G1) {
			ZKLOGD("Vfs", "Gothic 1 disk detected");
		} else if (signature == VFS_DISK_SIGNATURE_G2) {
			ZKLOGD("Vfs", "Gothic 2 disk detected");
		} else {
			throw VfsBrokenDiskError {signature};
		}

		if (auto it = comment.find('\x1A'); it != std::string::npos) {
			comment.resize(it);
		}

		if (catalog_offset == 0) {
			catalog_offset = r->tell();
		}

		std::function<bool(VfsNode*)> load_entry =
		    [&load_entry, overwrite, catalog_offset, timestamp, zipped, &r, buf, size](VfsNode* parent) {
			    auto e_name = r->read_string(64);
			    auto e_offset = r->read_uint();
			    auto e_size = r->read_uint();
			    auto e_type = r->read_uint();
			    [[maybe_unused]] auto attributes = r->read_uint();

			    // Find the first non-space char from the end (refer #77)
			    auto it = std::find_if(e_name.rbegin(), e_name.rend(), [](char c) {
				    return !std::isspace(static_cast<unsigned char>(c));
			    });

			    if (it != e_name.rend()) {
				    auto n = e_name.rend() - it;
				    e_name.resize(static_cast<unsigned long>(n));
			    }

			    VfsNode* existing = parent->child(e_name);
			    bool dir = (e_type & 0x80000000) != 0;
			    bool last = (e_type & 0x40000000) != 0;

			    ZKLOGT("Vfs",
			           "Parsing node name='%s' offset=%x size=%x dir=%d last=%d existing='%s'",
			           e_name.c_str(),
			           e_offset,
			           e_size,
			           dir,
			           last,
			           existing == nullptr ? "(NIL)" : existing->name().c_str());

			    if (dir) {
				    if (existing == nullptr) {
					    existing = parent->create(VfsNode::directory(e_name, timestamp));
				    } else if (existing->type() != VfsNodeType::DIRECTORY) {
					    switch (overwrite) {
					    case VfsOverwriteBehavior::NONE:
						    return last;
					    case VfsOverwriteBehavior::NEWER:
						    if (existing->time() <= timestamp) {
							    return last;
						    }
						    break;
					    case VfsOverwriteBehavior::OLDER:
						    if (existing->time() >= timestamp) {
							    return last;
						    }
						    break;
					    case VfsOverwriteBehavior::ALL:
						    break;
					    }

					    parent->remove(e_name);
					    existing = parent->create(VfsNode::directory(e_name, timestamp));
				    }

				    auto self_offset = r->tell();
				    r->seek(catalog_offset + e_offset * 80, Whence::BEG);
				    while (!load_entry(existing))
					    ;
				    r->seek(static_cast<ssize_t>(self_offset), Whence::BEG);
			    } else {
				    // For zipped VDFs, entry.Size is the uncompressed size; the actual
				    // compressed data at e_offset is smaller. Only check offset validity.
				    // For normal VDFs, the full extent must fit within the archive.
				    if (zipped ? (e_offset >= size) : (e_offset + e_size > size)) {
					    return last;
				    }

				    if (existing != nullptr) {
					    switch (overwrite) {
					    case VfsOverwriteBehavior::NONE:
						    return last;
					    case VfsOverwriteBehavior::NEWER:
						    if (existing->time() <= timestamp) {
							    return last;
						    }
						    break;
					    case VfsOverwriteBehavior::OLDER:
						    if (existing->time() >= timestamp) {
							    return last;
						    }
						    break;
					    case VfsOverwriteBehavior::ALL:
						    break;
					    }

					    parent->remove(e_name);
				    }

				    // For zipped files, the descriptor gets the full remaining buffer
				    // so ReadZipped can read the compressed stream. raw_size preserves
				    // the catalog entry size for fallback (e.g. raw audio files).
				    auto desc_size = zipped ? (size - e_offset) : static_cast<std::size_t>(e_size);
				    (void) parent->create(
				        VfsNode::file(e_name,
				                      VfsFileDescriptor {buf + e_offset, desc_size, false, zipped, e_size},
				                      timestamp));
			    }

			    return last;
		    };

		r->seek(catalog_offset, Whence::BEG);
		while (!load_entry(&_m_root))
			;
	}
} // namespace zenkit
