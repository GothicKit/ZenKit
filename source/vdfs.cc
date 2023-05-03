// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vdfs.hh>

namespace phoenix {
	std::time_t dos_to_unix_time(std::uint32_t dos) noexcept {
		struct tm t {};

		t.tm_year = ((int32_t) ((dos >> 25) & 0x7F)) + 80;
		t.tm_mon = ((int32_t) ((dos >> 21) & 0xF)) - 1;
		t.tm_mday = (int32_t) ((dos >> 16) & 0x1F);
		t.tm_hour = (int32_t) ((dos >> 11) & 0x1F);
		t.tm_min = (int32_t) ((dos >> 5) & 0x3F);
		t.tm_sec = ((int32_t) ((dos >> 0) & 0x1F)) * 2;

		return mktime(&t);
	}

	std::uint32_t unix_time_to_dos(std::time_t nix) noexcept {
		struct std::tm* t {std::gmtime(&nix)};
		std::uint32_t dos {0};

		dos |= static_cast<std::uint32_t>((t->tm_year - 80) << 25);
		dos |= static_cast<std::uint32_t>((t->tm_mon + 1) << 21);
		dos |= static_cast<std::uint32_t>(t->tm_mday << 16);
		dos |= static_cast<std::uint32_t>(t->tm_hour << 11);
		dos |= static_cast<std::uint32_t>(t->tm_min << 5);
		dos |= static_cast<std::uint32_t>((t->tm_sec / 2) << 0);

		return dos;
	}

	vdfs_signature_error::vdfs_signature_error(const std::string& signature)
	    : error("VDF signature not recognized: \"" + signature + "\"") {}

	bool vdf_entry_comparator::operator()(const vdf_entry& a, const vdf_entry& b) const {
		return icompare(a.name, b.name);
	}

	bool vdf_entry_comparator::operator()(const vdf_entry& a, std::string_view b) const {
		return icompare(a.name, b);
	}

	bool vdf_entry_comparator::operator()(std::string_view a, const vdf_entry& b) const {
		return icompare(a, b.name);
	}

	vdf_header::vdf_header(std::string_view comment_text, std::time_t ts) : comment(comment_text), timestamp(ts) {}

	vdf_header vdf_header::read(buffer& in) {
		vdf_header header {};

		header.comment = in.get_string(VDF_COMMENT_LENGTH);
		header.signature = in.get_string(VDF_SIGNATURE_LENGTH);
		header.entry_count = in.get_uint();
		header.file_count = in.get_uint();
		header.timestamp = dos_to_unix_time(in.get_uint());
		header.size = in.get_uint();
		header.catalog_offset = in.get_uint();
		header.version = in.get_uint();

		if (auto it = header.comment.find('\x1A'); it != std::string::npos) {
			header.comment.resize(it);
		}

		return header;
	}

	vdf_entry::vdf_entry(std::string_view entry_name, std::uint32_t attrs)
	    : name(entry_name), type(VDF_MASK_DIRECTORY), attributes(attrs) {}

	const vdf_entry* vdf_entry::resolve_path(std::string_view path) const {
		auto it = path.find('/');
		auto current = path.substr(0, it);

		auto result = this->children.find(current);
		if (result == this->children.end()) {
			return nullptr;
		}

		if (it != std::string_view::npos) {
			return (*result).resolve_path(path.substr(it + 1));
		}

		return &*result;
	}

	const vdf_entry* vdf_entry::find_child(std::string_view child_name) const {
		auto result = this->children.find(child_name);
		if (result == this->children.end()) {
			// recurse the search
			const vdf_entry* child;

			for (const auto& entry : children) {
				if ((child = entry.find_child(child_name), child != nullptr)) {
					return child;
				}
			}

			return nullptr;
		}

		return &*result;
	}

	vdf_entry* vdf_entry::find_child(std::string_view child_name) {
		auto result = this->children.find(child_name);
		if (result == this->children.end()) {
			// recurse the search
			vdf_entry* child;

			for (const auto& entry : children) {
				if ((child = const_cast<vdf_entry*>(entry.find_child(child_name)), child != nullptr)) {
					return child;
				}
			}

			return nullptr;
		}

		return const_cast<vdf_entry*>(&*result);
	}

	vdf_entry vdf_entry::read(buffer& in, std::uint32_t catalog_offset) {
		vdf_entry entry {};

		entry.name = in.get_string(VDF_ENTRY_NAME_LENGTH);
		entry.offset = in.get_uint();
		entry.size = in.get_uint();
		entry.type = in.get_uint();
		entry.attributes = in.get_uint();

		if (auto it = entry.name.find('\x20'); it != std::string::npos) {
			entry.name.resize(it);
		}

		if (entry.is_directory()) {
			auto self_offset = in.position();
			in.position(catalog_offset + entry.offset * vdf_entry::packed_size);

			const vdf_entry* child = nullptr;
			do {
				child = &*std::get<0>(entry.children.insert(read(in, catalog_offset)));
			} while (!child->is_last());

			in.position(self_offset);
		} else {
			if (entry.offset + entry.size > in.limit()) {
				entry._m_data = in.slice(entry.offset, 0);
				PX_LOGE("failed to parse VDF entry '{}': too big", entry.name);
			} else {
				entry._m_data = in.slice(entry.offset, entry.size);
			}
		}

		return entry;
	}

	void vdf_entry::merge(const vdf_entry& itm, bool override_existing) {
		auto result = this->children.find(itm.name);
		if (result == this->children.end()) {
			// If no matching entry was found, this is a new one.
			// Just add it to the children of this entry.
			children.insert(itm);
		} else {
			if (itm.is_file() || (*result).is_file()) {
				if (!override_existing) {
					return;
				}

				// If an entry with the same name is found and either is a file,
				// replace the entry with the new one.
				children.erase(result);
				children.insert(itm);
			} else {
				// Otherwise, the entry is a directory, so we just continue the merge.
				for (const auto& child : itm.children) {
					const_cast<vdf_entry&>(*result).merge(child, override_existing);
				}
			}
		}
	}

	vdf_file::vdf_file(std::string_view comment, std::time_t timestamp) : header(comment, timestamp) {}

	const vdf_entry* vdf_file::find_entry(std::string_view name) const {
		auto result = this->entries.find(name);
		if (result == this->entries.end()) {
			// recurse the search
			vdf_entry* child;

			for (const auto& entry : entries) {
				if ((child = const_cast<vdf_entry*>(entry.find_child(name)), child != nullptr)) {
					return child;
				}
			}

			return nullptr;
		}

		return &*result;
	}

	const vdf_entry* vdf_file::resolve_path(std::string_view path) const {
		auto it = path.find('/');
		auto name = path.substr(0, it);

		auto result = this->entries.find(name);
		if (result == this->entries.end()) {
			return nullptr;
		}

		if (it != std::string_view::npos) {
			return (*result).resolve_path(path.substr(it + 1));
		}

		return &*result;
	}

	vdf_entry* vdf_file::find_entry(std::string_view name) {
		auto result = this->entries.find(name);
		if (result == this->entries.end()) {
			// recurse the search
			vdf_entry* child;

			for (const auto& entry : entries) {
				if ((child = const_cast<vdf_entry*>(entry.find_child(name)), child != nullptr)) {
					return child;
				}
			}

			return nullptr;
		}

		return const_cast<vdf_entry*>(&*result);
	}

	vdf_file vdf_file::open(const std::filesystem::path& path) {
		auto buf = buffer::mmap(path);

		vdf_file vdf {};
		vdf.header = vdf_header::read(buf);

		// TODO: Reverse-engineer Union VDF format
		if (vdf.header.signature != VDF_SIGNATURE_G1 && vdf.header.signature != VDF_SIGNATURE_G2) {
			throw vdfs_signature_error {vdf.header.signature};
		}

		buf.position(vdf.header.catalog_offset);

		const vdf_entry* entry = nullptr;
		do {
			entry = &*std::get<0>(vdf.entries.insert(vdf_entry::read(buf, vdf.header.catalog_offset)));
		} while (!entry->is_last());

		return vdf;
	}

	vdf_file vdf_file::open(phoenix::buffer& buf) {
		vdf_file vdf {};

		vdf.header = vdf_header::read(buf);

		// TODO: Reverse-engineer Union VDF format
		if (vdf.header.signature != VDF_SIGNATURE_G1 && vdf.header.signature != VDF_SIGNATURE_G2) {
			throw vdfs_signature_error {vdf.header.signature};
		}

		buf.position(vdf.header.catalog_offset);

		const vdf_entry* entry = nullptr;
		do {
			entry = &*std::get<0>(vdf.entries.insert(vdf_entry::read(buf, vdf.header.catalog_offset)));
		} while (!entry->is_last());

		return vdf;
	}

	void vdf_file::merge(const vdf_file& file, bool override_existing) {
		for (const auto& child : file.entries) {
			auto result = this->entries.find(child.name);
			if (result == this->entries.end()) {
				// If no matching entry was found, this is a new one.
				// Just add it to the children of this entry.
				entries.insert(child);
			} else {
				if (child.is_file() || (*result).is_file()) {
					// If an entry with the same name is found and either is a file,
					// replace the entry with the new one.
					if (override_existing) {
						entries.erase(result);
						entries.insert(child);
					}
				} else {
					// Otherwise, the entry is a directory, so we just continue the merge.
					for (const auto& sub_child : child.children) {
						const_cast<vdf_entry&>(*result).merge(sub_child, override_existing);
					}
				}
			}
		}
	}
} // namespace phoenix
