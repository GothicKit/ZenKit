// Copyright © 2023 Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT
#include "phoenix/Vfs.hh"
#include "model_script_dsl.hh"
#include "phoenix/ext/daedalus_classes.hh"
#include "phoenix/phoenix.hh"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <random>
#include <stack>

namespace phoenix {
	static constexpr std::string_view VFS_DISK_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
	static constexpr std::string_view VFS_DISK_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";

	VfsBrokenDiskError::VfsBrokenDiskError(const std::string& signature)
	    : error("VFS disk signature not recognized: \"" + signature + "\"") {}

	VfsFileExistsError::VfsFileExistsError(const std::string& name) : error("file exists: \"" + name + "\"") {}

	VfsNotFoundError::VfsNotFoundError(const std::string& name) : error("not found: \"" + name + "\"") {}

	bool VfsNodeComparator::operator()(const VfsNode& a, const VfsNode& b) const noexcept {
		return icompare(a.name(), b.name());
	}

	bool VfsNodeComparator::operator()(const VfsNode& a, std::string_view b) const noexcept {
		return icompare(a.name(), b);
	}

	bool VfsNodeComparator::operator()(std::string_view a, const VfsNode& b) const noexcept {
		return icompare(a, b.name());
	}

	VfsNode::VfsNode(std::string_view name, time_t ts) : _m_name(name), _m_time(ts), _m_data(std::vector<VfsNode> {}) {}

	VfsNode::VfsNode(std::string_view name, buffer dev, time_t ts)
	    : _m_name(name), _m_time(ts), _m_data(std::move(dev)) {}

	std::vector<VfsNode> const& VfsNode::children() const {
		return std::get<std::vector<VfsNode>>(_m_data);
	}

	std::string_view trim_trailing_whitespace(std::string_view s) {
		while (std::isspace(s.back())) {
			s = s.substr(0, s.size() - 1);
		}

		return s;
	}

	VfsNode const* VfsNode::child(std::string_view name) const {
		auto& children = std::get<std::vector<VfsNode>>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = std::lower_bound(children.begin(), children.end(), name, VfsNodeComparator {});
		if (it == children.end() || !iequals(it->name(), name))
			return nullptr;
		return &*it;
	}

	VfsNode* VfsNode::child(std::string_view name) {
		auto& children = std::get<std::vector<VfsNode>>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = std::lower_bound(children.begin(), children.end(), name, VfsNodeComparator {});
		if (it == children.end() || !iequals(it->name(), name))
			return nullptr;
		return &*it;
	}

	VfsNode* VfsNode::create(VfsNode node) {
		auto& children = std::get<std::vector<VfsNode>>(_m_data);
		auto it = children.insert(std::lower_bound(children.begin(), children.end(), node, VfsNodeComparator {}),
		                          std::move(node));
		return &*it;
	}

	bool VfsNode::remove(std::string_view name) {
		auto& children = std::get<std::vector<VfsNode>>(_m_data);

		name = trim_trailing_whitespace(name);
		auto it = std::lower_bound(children.begin(), children.end(), name, VfsNodeComparator {});
		if (it == children.end() || !iequals(it->name(), name))
			return false;

		children.erase(it);
		return true;
	}

	buffer VfsNode::open() const {
		return std::get<buffer>(_m_data).duplicate();
	}

	VfsNode VfsNode::directory(std::string_view name) {
		return directory(name, -1);
	}

	VfsNode VfsNode::file(std::string_view name, buffer dev) {
		return file(name, std::move(dev), -1);
	}

	VfsNode VfsNode::directory(std::string_view name, std::time_t ts) {
		return VfsNode(name, ts);
	}

	VfsNode VfsNode::file(std::string_view name, buffer dev, time_t ts) {
		return VfsNode(name, std::move(dev), ts);
	}

	VfsNodeType VfsNode::type() const noexcept {
		return std::holds_alternative<buffer>(_m_data) ? VfsNodeType::FILE : VfsNodeType::DIRECTORY;
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

			if (next == std::string_view::npos)
				break;

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
			if (child != nullptr)
				return child;

			auto& children = node->children();
			for (auto const& x : children) {
				if (x.type() == VfsNodeType::FILE)
					continue;

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

	void Vfs::mount_disk(const std::filesystem::path& host, VfsOverwriteBehavior overwrite) {
		this->mount_disk(buffer::mmap(host), overwrite);
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

	void Vfs::mount_disk(buffer buf, VfsOverwriteBehavior overwrite) {
		auto comment = buf.get_string(256);
		auto signature = buf.get_string(16);
		[[maybe_unused]] auto entry_count = buf.get_uint();
		[[maybe_unused]] auto file_count = buf.get_uint();
		auto timestamp = vfs_dos_to_unix_time(buf.get_uint());
		[[maybe_unused]] auto _size = buf.get_uint();
		auto catalog_offset = buf.get_uint();
		[[maybe_unused]] auto version = buf.get_uint();

		if (signature != VFS_DISK_SIGNATURE_G1 && signature != VFS_DISK_SIGNATURE_G2) {
			throw VfsBrokenDiskError {signature};
		}

		if (auto it = comment.find('\x1A'); it != std::string::npos) {
			comment.resize(it);
		}

		std::function<bool(VfsNode*)> load_entry =
		    [&load_entry, overwrite, catalog_offset, timestamp, &buf](VfsNode* parent) {
			    auto name = buf.get_string(64);
			    auto offset = buf.get_uint();
			    auto size = buf.get_uint();
			    auto type = buf.get_uint();
			    [[maybe_unused]] auto attributes = buf.get_uint();

			    // Find the first non-space char from the end (refer #77)
			    auto it = std::find_if(name.rbegin(), name.rend(), [](char c) {
				    return !std::isspace(static_cast<unsigned char>(c));
			    });

			    if (it != name.rend()) {
				    auto n = name.rend() - it;
				    name.resize(n);
			    }

			    VfsNode* existing = parent->child(name);
			    bool dir = (type & 0x80000000) != 0;
			    bool last = (type & 0x40000000) != 0;

			    if (dir) {
				    if (existing == nullptr) {
					    existing = parent->create(VfsNode::directory(name, timestamp));
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

					    parent->remove(name);
					    existing = parent->create(VfsNode::directory(name, timestamp));
				    }

				    auto self_offset = buf.position();
				    buf.position(catalog_offset + offset * 80);
				    while (!load_entry(existing))
					    ;
				    buf.position(self_offset);
			    } else {
				    if (offset + size > buf.limit()) {
					    PX_LOGE("unable to mount file ", name, ": invalid offset and size");
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

					    parent->remove(name);
				    }

				    (void) parent->create(VfsNode::file(name, buf.slice(offset, size), timestamp));
			    }

			    return last;
		    };

		while (!load_entry(&_m_root))
			;
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
					if (existing->time() <= source->time())
						continue;
					break;
				case VfsOverwriteBehavior::OLDER:
					if (existing->time() >= source->time())
						continue;
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

			if (next == std::string_view::npos)
				break;
			path = path.substr(next + 1);
		}

		return *context;
	}

	bool Vfs::remove(std::string_view path) {
		auto lastSlash = path.rfind('/');
		auto parentPath = path.substr(0, lastSlash == std::string_view::npos ? 0 : lastSlash);
		auto childName = path.substr(lastSlash + 1);

		VfsNode* pNode = this->resolve(parentPath);
		if (pNode == nullptr)
			return false;
		return pNode->remove(childName);
	}

	void Vfs::mount_host(std::filesystem::path const& sourcePath,
	                     std::string_view mountPoint,
	                     VfsOverwriteBehavior overwrite) {
		auto root = VfsNode::directory(sourcePath.filename().string());

		std::function<void(VfsNode*, std::filesystem::path const&)> load_directory =
		    [&load_directory](VfsNode* parent, std::filesystem::path const& host) {
			    for (auto const& ref : std::filesystem::directory_iterator(host)) {
				    auto const& path = ref.path();
				    auto time =
				        std::chrono::duration_cast<std::chrono::seconds>(ref.last_write_time().time_since_epoch());

				    if (ref.is_directory()) {
					    VfsNode* newP = parent->create(VfsNode::directory(path.filename().string(), time.count()));
					    load_directory(newP, path);
				    } else if (ref.file_size() > 0) {
					    parent->create(VfsNode::file(path.filename().string(), buffer::mmap(path), time.count()));
				    }
			    }
		    };

		load_directory(&root, sourcePath);

		for (auto& child : root.children()) {
			this->mount(child, mountPoint, overwrite);
		}
	}

} // namespace phoenix
