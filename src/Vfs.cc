// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Vfs.hh"
#include "zenkit/Error.hh"
#include "zenkit/Stream.hh"

#include "phoenix/buffer.hh"

#include "Internal.hh"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stack>

namespace zenkit {
	static constexpr std::string_view VFS_DISK_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
	static constexpr std::string_view VFS_DISK_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";

	class ZKINT RawBufferBacking : public phoenix::buffer_backing {
	public:
		RawBufferBacking(std::byte const* bytes, uint64_t length) : _m_buffer(bytes), _m_size(length) {}

		[[nodiscard]] bool direct() const noexcept override {
			return false;
		}

		[[nodiscard]] bool readonly() const noexcept override {
			return false;
		}

		[[nodiscard]] uint64_t size() const noexcept override {
			return _m_size;
		}

		[[nodiscard]] std::byte const* array() const override {
			return _m_buffer;
		}

		void read(std::byte* buf, std::uint64_t size, std::uint64_t offset) const override {
			if (this->readonly()) {
				throw phoenix::buffer_readonly {};
			}

			if (offset + size > this->size()) {
				throw phoenix::buffer_overflow {offset, size, "in backing"};
			}

			std::copy_n(_m_buffer + static_cast<long>(offset), size, buf);
		}

	private:
		std::byte const* _m_buffer;
		uint64_t _m_size;
	};

	VfsBrokenDiskError::VfsBrokenDiskError(std::string const& signature)
	    : Error("VFS disk signature not recognized: \"" + signature + "\"") {}

	VfsFileExistsError::VfsFileExistsError(std::string const& name) : Error("file exists: \"" + name + "\"") {}

	VfsNotFoundError::VfsNotFoundError(std::string const& name) : Error("not found: \"" + name + "\"") {}

	bool VfsNodeComparator::operator()(VfsNode const& a, VfsNode const& b) const noexcept {
		return phoenix::icompare(a.name(), b.name());
	}

	bool VfsNodeComparator::operator()(VfsNode const& a, std::string_view b) const noexcept {
		return phoenix::icompare(a.name(), b);
	}

	bool VfsNodeComparator::operator()(std::string_view a, VfsNode const& b) const noexcept {
		return phoenix::icompare(a, b.name());
	}

	VfsNode::VfsNode(std::string_view name, time_t ts) : _m_name(name), _m_time(ts), _m_data(ChildContainer {}) {}

	VfsNode::VfsNode(std::string_view name, phoenix::buffer dev, time_t ts)
	    : _m_name(name), _m_time(ts), _m_data(std::move(dev)) {}

	VfsNode::VfsNode(std::string_view name, VfsFileDescriptor dev, time_t ts)
	    : _m_name(name), _m_time(ts), _m_data(dev) {}

	VfsNode::ChildContainer const& VfsNode::children() const {
		return std::get<ChildContainer>(_m_data);
	}

	std::string_view trim_trailing_whitespace(std::string_view s) {
		while (s.length() > 0 && std::isspace(s.back())) {
			s = s.substr(0, s.size() - 1);
		}

		return s;
	}

	VfsNode const* VfsNode::child(std::string_view name) const {
		auto& children = std::get<ChildContainer>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = children.find(name);
		if (it == children.end() || !phoenix::iequals(it->name(), name)) return nullptr;
		return &*it;
	}

	VfsNode* VfsNode::child(std::string_view name) {
		auto& children = std::get<ChildContainer>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = children.find(name);
		if (it == children.end() || !phoenix::iequals(it->name(), name)) return nullptr;
		return const_cast<VfsNode*>(&*it);
	}

	VfsNode* VfsNode::create(VfsNode node) {
		auto& children = std::get<ChildContainer>(_m_data);
		auto it = children.insert(node);
		return const_cast<VfsNode*>(&*it.first);
	}

	bool VfsNode::remove(std::string_view name) {
		auto& children = std::get<ChildContainer>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = children.find(name);
		if (it == children.end() || !phoenix::iequals(it->name(), name)) return false;

		children.erase(it);
		return true;
	}

	phoenix::buffer VfsNode::open() const {
		if (std::holds_alternative<phoenix::buffer>(_m_data)) {
			return std::get<phoenix::buffer>(_m_data).duplicate();
		}

		auto fd = std::get<VfsFileDescriptor>(_m_data);
		return phoenix::buffer {std::make_shared<RawBufferBacking>(fd.memory, fd.size)};
	}

	std::unique_ptr<Read> VfsNode::open_read() const {
		if (std::holds_alternative<phoenix::buffer>(_m_data)) {
			auto buf = std::get<phoenix::buffer>(_m_data);
			return Read::from(buf.array(), buf.limit());
		}

		auto fd = std::get<VfsFileDescriptor>(_m_data);
		return Read::from(fd.memory, fd.size);
	}

	VfsNode VfsNode::directory(std::string_view name) {
		return directory(name, -1);
	}

	VfsNode VfsNode::file(std::string_view name, phoenix::buffer dev) {
		return file(name, std::move(dev), -1);
	}

	VfsNode VfsNode::file(std::string_view name, VfsFileDescriptor dev) {
		return file(name, dev, -1);
	}

	VfsNode VfsNode::directory(std::string_view name, std::time_t ts) {
		return VfsNode(name, ts);
	}

	VfsNode VfsNode::file(std::string_view name, phoenix::buffer dev, time_t ts) {
		return VfsNode(name, std::move(dev), ts);
	}

	VfsNode VfsNode::file(std::string_view name, VfsFileDescriptor dev, std::time_t ts) {
		return VfsNode(name, dev, ts);
	}

	VfsNodeType VfsNode::type() const noexcept {
		return (std::holds_alternative<phoenix::buffer>(_m_data), std::holds_alternative<VfsFileDescriptor>(_m_data))
		    ? VfsNodeType::FILE
		    : VfsNodeType::DIRECTORY;
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

			auto* child = node->child(name);
			if (child != nullptr) return child;

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

	void Vfs::mount_disk(std::filesystem::path const& host, VfsOverwriteBehavior overwrite) {
#ifdef _ZK_WITH_MMAP
		auto& mem = _m_data_mapped.emplace_back(host);
		this->mount_disk(mem.data(), mem.size(), overwrite);
#else
		std::ifstream stream {host, std::ios::ate};
		auto size = stream.tellg();
		stream.seekg(0);

		auto& data = _m_data.emplace_back(new std::byte[(size_t) size]);
		stream.read((char*) data.get(), size);
		this->mount_disk(data.get(), (size_t) size, overwrite);
#endif
	}

	static std::time_t vfs_dos_to_unix_time(std::uint32_t dos) noexcept {
		struct tm t {};

		t.tm_year = ((int32_t) ((dos >> 25) & 0x7F)) + 80;
		t.tm_mon = ((int32_t) ((dos >> 21) & 0xF)) - 1;
		t.tm_mday = (int32_t) ((dos >> 16) & 0x1F);
		t.tm_hour = (int32_t) ((dos >> 11) & 0x1F);
		t.tm_min = (int32_t) ((dos >> 5) & 0x3F);
		t.tm_sec = ((int32_t) ((dos >> 0) & 0x1F)) * 2;

		return mktime(&t);
	}

	void Vfs::mount_disk(phoenix::buffer buf, VfsOverwriteBehavior overwrite) {
		auto mem = std::make_unique<std::byte[]>(buf.limit());
		::memcpy(mem.get(), buf.array(), buf.limit());
		_m_data.push_back(std::move(mem));
		this->mount_disk(mem.get(), buf.limit(), overwrite);
	}

	void Vfs::mount_disk(Read* buf, VfsOverwriteBehavior overwrite) {
		buf->seek(0, Whence::END);
		auto size = buf->tell();
		buf->seek(0, Whence::BEG);

		auto mem = std::make_unique<std::byte[]>(size);
		buf->read(mem.get(), size);

		_m_data.push_back(std::move(mem));
		this->mount_disk(mem.get(), size, overwrite);
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
			auto it = context->child(name);

			if (it == nullptr) {
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
					                                 VfsFileDescriptor {mem.data(), mem.size()},
					                                 time.count()));
#else
					    std::ifstream stream {host, std::ios::ate};
					    auto size = stream.tellg();
					    stream.seekg(0);

					    auto& data = _m_data.emplace_back(new std::byte[(size_t) size]);
					    stream.read((char*) data.get(), size);

					    parent->create(VfsNode::file(path.filename().string(),
					                                 VfsFileDescriptor {data.get(), static_cast<size_t>(size)},
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
		[[maybe_unused]] auto _size = r->read_uint();
		auto catalog_offset = r->read_uint();
		auto version = r->read_uint();

		// Check that we're not loading a compressed Union disk.
		if (version != 80) {
			throw VfsBrokenDiskError {"Detected unsupported Union disk"};
		}

		if (signature != VFS_DISK_SIGNATURE_G1 && signature != VFS_DISK_SIGNATURE_G2) {
			throw VfsBrokenDiskError {signature};
		}

		if (auto it = comment.find('\x1A'); it != std::string::npos) {
			comment.resize(it);
		}

		std::function<bool(VfsNode*)> load_entry =
		    [&load_entry, overwrite, catalog_offset, timestamp, &r, buf, size](VfsNode* parent) {
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
				    if (e_offset + e_size > size) {
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

				    (void) parent->create(VfsNode::file(e_name, VfsFileDescriptor {buf + e_offset, e_size}, timestamp));
			    }

			    return last;
		    };

		while (!load_entry(&_m_root))
			;
	}
} // namespace zenkit
