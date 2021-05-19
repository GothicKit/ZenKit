// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/messages.hh>

namespace phoenix {
	messages messages::parse(const std::string& path) {
		messages msgs {};

		auto in = reader::from(path);
		auto archive = archive_reader::open(in);

		archive_object obj;
		if (!archive->read_object_begin(obj)) {
			throw parser_error("messages: failed to read message database: no begin object found");
		}

		if (obj.class_name != "zCCSLib") {
			throw parser_error("messages: failed to read message database: begin object is not zCCSLib");
		}

		auto item_count = archive->read_int();
		msgs._m_blocks.reserve(item_count);

		for (int i = 0; i < item_count; ++i) {
			if (!archive->read_object_begin(obj) || obj.class_name != "zCCSBlock") {
				throw parser_error("messages: failed to read message database: expected zCCSBlock but didn't find it");
			}

			auto& itm = msgs._m_blocks.emplace_back();
			itm.name = archive->read_string();
			(void) archive->read_int();  /* function unknown at this time */
			(void) archive->read_float(); /* function unknown at this time */

			if (!archive->read_object_begin(obj)) {
				throw parser_error("messages: failed to read message database: expected atomic block - not found");
			}

			if (!archive->read_object_begin(obj)) {
				throw parser_error("messages: failed to read message database: expected message block - not found");
			}

			// Quirk: Binary message dbs have a byte here instead of an enum.
			if (archive->get_header().format == archive_format::binary) {
				itm.message.type = archive->read_byte();
			} else {
				itm.message.type = archive->read_enum();
			}
			itm.message.text = archive->read_string();
			itm.message.name = archive->read_string();
			msgs._m_blocks_by_name[itm.name] = &itm;

			if (!archive->read_object_end()) {
				archive->skip_object(true);
			}

			if (!archive->read_object_end()) {
				archive->skip_object(true);
			}

			if (!archive->read_object_end()) {
				archive->skip_object(true);
			}
		}

		return msgs;
	}

	const message_block* messages::block_by_name(const std::string& name) {
		if (auto it = _m_blocks_by_name.find(name); it != _m_blocks_by_name.end()) {
			return it->second;
		}

		return nullptr;
	}
}// namespace phoenix