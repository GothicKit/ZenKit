// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vdfs.hh>

namespace phoenix {
	std::time_t dos_to_unix_time(std::uint32_t dos) noexcept {
		struct tm t {};

		t.tm_year = ((int) ((dos >> 25) & 0x7F)) + 80;
		t.tm_mon = ((int) ((dos >> 21) & 0xF)) - 1;
		t.tm_mday = (int) ((dos >> 16) & 0x1F);
		t.tm_hour = (int) ((dos >> 11) & 0x1F);
		t.tm_min = (int) ((dos >> 5) & 0x3F);
		t.tm_sec = ((int) ((dos >> 0) & 0x1F)) * 2;

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

	vdf_header::vdf_header(std::string_view comment, std::time_t timestamp) : comment(comment), timestamp(timestamp) {}

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

	vdf_entry::vdf_entry(std::string_view name, std::uint32_t attributes)
	    : name(name), type(VDF_MASK_DIRECTORY), attributes(attributes) {}

	const vdf_entry* vdf_entry::resolve_path(std::string_view path) const {
		auto it = path.find('/');
		auto name = path.substr(0, it);

		for (const auto& child : children) {
			if (iequals(child.name, name)) {
				if (it != std::string_view::npos) {
					return child.resolve_path(path.substr(it + 1));
				}

				return &child;
			}
		}

		return nullptr;
	}

	const vdf_entry* vdf_entry::find_child(std::string_view name) const {
		for (const auto& child : children) {
			if (iequals(child.name, name)) {
				return &child;
			} else if (const auto* v = child.find_child(name); v != nullptr) {
				return v;
			}
		}

		return nullptr;
	}

	vdf_entry* vdf_entry::find_child(std::string_view name) {
		for (auto& child : children) {
			if (iequals(child.name, name)) {
				return &child;
			} else if (auto* v = child.find_child(name); v != nullptr) {
				return v;
			}
		}

		return nullptr;
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

			vdf_entry* child = nullptr;
			do {
				child = &entry.children.emplace_back(read(in, catalog_offset));
			} while (!child->is_last());

			in.position(self_offset);
		} else {
			entry._m_data = in.slice(entry.offset, entry.size);
		}

		return entry;
	}

	void vdf_entry::merge(const vdf_entry& itm, bool override_existing) {
		for (auto it = children.begin(); it != children.end(); ++it) {
			if (phoenix::iequals(it->name, itm.name)) {
				if (itm.is_file() || it->is_file()) {
					if (!override_existing) {
						return;
					}

					// If an entry with the same name is found and either is a file,
					// replace the entry with the new one.
					children.erase(it);
					children.push_back(itm);
				} else {
					// Otherwise, the entry is a directory, so we just continue the merge.
					for (const auto& child : itm.children) {
						it->merge(child, override_existing);
					}
				}
				return;
			}
		}

		// If no matching entry was found, this is a new one.
		// Just add it to the children of this entry.
		children.push_back(itm);
	}

	vdf_file::vdf_file(std::string_view comment, std::time_t timestamp) : header(comment, timestamp) {}

	const vdf_entry* vdf_file::find_entry(std::string_view name) const {
		for (const auto& entry : entries) {
			if (iequals(entry.name, name)) {
				return &entry;
			} else if (const auto* v = entry.find_child(name); v != nullptr) {
				return v;
			}
		}

		return nullptr;
	}

	const vdf_entry* vdf_file::resolve_path(std::string_view path) const {
		auto it = path.find('/');
		auto name = path.substr(0, it);

		for (const auto& child : entries) {
			if (iequals(child.name, name)) {
				if (it != std::string_view::npos) {
					return child.resolve_path(path.substr(it + 1));
				}

				return &child;
			}
		}

		return nullptr;
	}

	vdf_entry* vdf_file::find_entry(std::string_view name) {
		for (auto& entry : entries) {
			if (iequals(entry.name, name)) {
				return &entry;
			} else if (auto* v = entry.find_child(name); v != nullptr) {
				return v;
			}
		}

		return nullptr;
	}

	vdf_file vdf_file::open(const std::filesystem::path& path) {
		auto in = buffer::mmap(path);
		return open(in);
	}

	vdf_file vdf_file::open(phoenix::buffer& buf) {
		vdf_file vdf {};

		vdf.header = vdf_header::read(buf);
		buf.position(vdf.header.catalog_offset);

		vdf_entry* entry = nullptr;
		do {
			entry = &vdf.entries.emplace_back(vdf_entry::read(buf, vdf.header.catalog_offset));
		} while (!entry->is_last());

		return vdf;
	}

	void vdf_file::merge(const vdf_file& file, bool override_existing) {
		for (const auto& child : file.entries) {
			bool child_found = false;

			for (auto it = entries.begin(); it != entries.end(); ++it) {
				if (phoenix::iequals(it->name, child.name)) {
					if (child.is_file() || it->is_file()) {
						// If an entry with the same name is found and either is a file,
						// replace the entry with the new one.
						if (override_existing) {
							entries.erase(it);
							entries.push_back(child);
						}
					} else {
						// Otherwise, the entry is a directory, so we just continue the merge.
						for (const auto& sub_child : child.children) {
							it->merge(sub_child, override_existing);
						}
					}

					child_found = true;
				}
			}

			if (!child_found) {
				entries.push_back(child);
			}
		}
	}

} // namespace phoenix
