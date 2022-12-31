// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/messages.hh>

namespace phoenix {
	messages messages::parse(buffer& buf) {
		auto archive = archive_reader::open(buf);
		messages msgs {};

		archive_object obj;
		if (!archive->read_object_begin(obj)) {
			throw parser_error {"messages", "root object missing"};
		}

		if (obj.class_name != "zCCSLib") {
			throw parser_error {"messages", "root object is not 'zCCSLib'"};
		}

		auto item_count = archive->read_int(); // NumOfItems
		msgs.blocks.reserve(static_cast<std::uint64_t>(item_count));

		for (int32_t i = 0; i < item_count; ++i) {
			if (!archive->read_object_begin(obj) || obj.class_name != "zCCSBlock") {
				throw parser_error {"messages", "expected 'zCCSBlock' but didn't find it"};
			}

			auto& itm = msgs.blocks.emplace_back();
			itm.name = archive->read_string();      // blockName
			auto block_count = archive->read_int(); // numOfBlocks
			(void) archive->read_float();           // subBlock0

			if (block_count != 1) {
				throw parser_error {"messages",
				                    "expected only one block but got " + std::to_string(block_count) + " for " +
				                        itm.name};
			}

			if (!archive->read_object_begin(obj) || obj.class_name != "zCCSAtomicBlock") {
				throw parser_error {"messages", "expected atomic block not found for " + itm.name};
			}

			if (!archive->read_object_begin(obj) || obj.class_name != "oCMsgConversation:oCNpcMessage:zCEventMessage") {
				throw parser_error {"messages", "expected oCMsgConversation not found for " + itm.name};
			}

			// Quirk: Binary message dbs have a byte here instead of an enum.
			if (archive->get_header().format == archive_format::binary) {
				itm.message.type = archive->read_byte();
			} else {
				itm.message.type = archive->read_enum();
			}

			itm.message.text = archive->read_string();
			itm.message.name = archive->read_string();

			if (!archive->read_object_end()) {
				// FIXME: in binary archives this might skip whole sections of the file due to faulty object
				//        extents in the archive. This might be due to encoding errors the version of ZenGin
				//        used with Gothic I
				archive->skip_object(true);
				PX_LOGW("messages: oCMsgConversation(\"", itm.name, "\") not fully parsed");
			}

			if (!archive->read_object_end()) {
				// FIXME: in Gothic I cutscene libraries, there is a `synchronized` attribute here
				archive->skip_object(true);
				PX_LOGW("messages: zCCSAtomicBlock(\"", itm.name, "\") not fully parsed");
			}

			if (!archive->read_object_end()) {
				archive->skip_object(true);
				PX_LOGW("messages: zCCSBlock(\"", itm.name, "\") not fully parsed");
			}
		}

		if (!archive->read_object_end()) {
			PX_LOGW("messages: not fully parsed");
		}

		// Prepare blocks for binary search in block_by_name
		std::sort(msgs.blocks.begin(), msgs.blocks.end(), [](const auto& a, const auto& b) { return a.name < b.name; });
		return msgs;
	}

	const message_block* messages::block_by_name(std::string_view name) const {
		auto result = std::lower_bound(this->blocks.begin(), this->blocks.end(), name, [](const auto& it, auto n) {
			return it.name < n;
		});

		if (result == this->blocks.end()) {
			return nullptr;
		}

		return &*result;
	}
} // namespace phoenix
