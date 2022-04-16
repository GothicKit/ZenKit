// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/compat.hh>
#include <phoenix/vdfs.hh>

namespace phoenix {
	vdf_header::vdf_header(std::string_view comment, std::time_t timestamp)
	    : _m_comment(comment), _m_timestamp(timestamp) {}

	vdf_header vdf_header::read(buffer& in) {
		vdf_header header {};

		header._m_comment = in.get_string(VDF_COMMENT_LENGTH);
		header._m_signature = in.get_string(VDF_SIGNATURE_LENGTH);
		header._m_entry_count = in.get_uint();
		header._m_file_count = in.get_uint();
		header._m_timestamp = dos_to_unix_time(in.get_uint());
		header._m_size = in.get_uint();
		header._m_catalog_offset = in.get_uint();
		header._m_version = in.get_uint();

		if (auto it = header._m_comment.find('\x1A'); it != std::string::npos) {
			header._m_comment.resize(it);
		}

		return header;
	}

	vdf_entry::vdf_entry(std::string_view name, std::uint32_t attributes)
	    : _m_name(name), _m_type(VDF_MASK_DIRECTORY), _m_attributes(attributes) {}

	const vdf_entry* vdf_entry::resolve_path(std::string_view path) const {
		auto it = path.find('/');
		auto name = path.substr(0, it);

		for (const auto& child : _m_children) {
			if (iequals(child.name(), name)) {
				if (it != std::string_view::npos) {
					return child.resolve_path(path.substr(it + 1));
				}

				return &child;
			}
		}

		return nullptr;
	}

	const vdf_entry* vdf_entry::find_child(std::string_view name) const {
		for (const auto& child : _m_children) {
			if (iequals(child.name(), name)) {
				return &child;
			} else if (const auto* v = child.find_child(name); v != nullptr) {
				return v;
			}
		}

		return nullptr;
	}

	vdf_entry* vdf_entry::find_child(std::string_view name) {
		for (auto& child : _m_children) {
			if (iequals(child.name(), name)) {
				return &child;
			} else if (auto* v = child.find_child(name); v != nullptr) {
				return v;
			}
		}

		return nullptr;
	}

	vdf_entry vdf_entry::read(buffer& in, std::uint32_t catalog_offset) {
		vdf_entry entry {};

		entry._m_name = in.get_string(VDF_ENTRY_NAME_LENGTH);
		entry._m_offset = in.get_uint();
		entry._m_size = in.get_uint();
		entry._m_type = in.get_uint();
		entry._m_attributes = in.get_uint();

		if (auto it = entry._m_name.find('\x20'); it != std::string::npos) {
			entry._m_name.resize(it);
		}

		if (entry.is_directory()) {
			auto self_offset = in.position();
			in.position(catalog_offset + entry.offset() * vdf_entry::packed_size);

			vdf_entry* child = nullptr;
			do {
				child = &entry._m_children.emplace_back(read(in, catalog_offset));
			} while (!child->is_last());

			in.position(self_offset);
		} else {
			entry._m_data = in.slice(entry._m_offset, entry._m_size);
		}

		return entry;
	}

	vdf_file::vdf_file(std::string_view comment, std::time_t timestamp) : _m_header(comment, timestamp) {}

	const vdf_entry* vdf_file::find_entry(std::string_view name) const {
		for (const auto& entry : _m_entries) {
			if (iequals(entry.name(), name)) {
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

		for (const auto& child : _m_entries) {
			if (iequals(child.name(), name)) {
				if (it != std::string_view::npos) {
					return child.resolve_path(path.substr(it + 1));
				}

				return &child;
			}
		}

		return nullptr;
	}

	vdf_entry* vdf_file::find_entry(std::string_view name) {
		for (auto& entry : _m_entries) {
			if (iequals(entry.name(), name)) {
				return &entry;
			} else if (auto* v = entry.find_child(name); v != nullptr) {
				return v;
			}
		}

		return nullptr;
	}

	vdf_file vdf_file::open(const std::string& path) {
		auto in = buffer::open(path);
		vdf_file vdf {};

		vdf._m_header = vdf_header::read(in);
		in.position(vdf._m_header._m_catalog_offset);

		vdf_entry* entry = nullptr;
		do {
			entry = &vdf._m_entries.emplace_back(vdf_entry::read(in, vdf._m_header._m_catalog_offset));
		} while (!entry->is_last());

		return vdf;
	}

} // namespace phoenix
