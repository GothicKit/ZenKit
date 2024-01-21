// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/CutsceneLibrary.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

#include "phoenix/buffer.hh"

namespace zenkit {
	CutsceneLibrary CutsceneLibrary::parse(phoenix::buffer& buf) {
		CutsceneLibrary msgs {};

		auto r = Read::from(&buf);
		msgs.load(r.get());

		return msgs;
	}

	CutsceneLibrary CutsceneLibrary::parse(phoenix::buffer&& buf) {
		return parse(buf);
	}

	CutsceneBlock const* CutsceneLibrary::block_by_name(std::string_view name) const {
		auto result = std::lower_bound(this->blocks.begin(), this->blocks.end(), name, [](auto const& it, auto n) {
			return it.name < n;
		});

		if (result == this->blocks.end()) {
			return nullptr;
		}

		return &*result;
	}

	void CutsceneLibrary::load(Read* r) {
		auto archive = ReadArchive::from(r);

		ArchiveObject obj;
		if (!archive->read_object_begin(obj)) {
			throw ParserError {"CutsceneLibrary", "root object missing"};
		}

		if (obj.class_name != "zCCSLib") {
			throw ParserError {"CutsceneLibrary", "root object is not 'zCCSLib'"};
		}

		auto item_count = archive->read_int(); // NumOfItems
		this->blocks.resize(static_cast<std::uint64_t>(item_count));

		for (auto& itm : this->blocks) {
			if (!archive->read_object_begin(obj) || obj.class_name != "zCCSBlock") {
				throw ParserError {"CutsceneLibrary", "expected 'zCCSBlock' but didn't find it"};
			}

			itm.name = archive->read_string();      // blockName
			auto block_count = archive->read_int(); // numOfBlocks
			(void) archive->read_float();           // subBlock0

			if (block_count != 1) {
				throw ParserError {"CutsceneLibrary",
				                   "expected only one block but got " + std::to_string(block_count) + " for " +
				                       itm.name};
			}

			if (!archive->read_object_begin(obj) || obj.class_name != "zCCSAtomicBlock") {
				throw ParserError {"CutsceneLibrary", "Expected atomic block not found for " + itm.name};
			}

			if (!archive->read_object_begin(obj) || obj.class_name != "oCMsgConversation:oCNpcMessage:zCEventMessage") {
				throw ParserError {"CutsceneLibrary", "Expected oCMsgConversation not found for " + itm.name};
			}

			itm.message.type = archive->read_enum();   // subType
			itm.message.text = archive->read_string(); // text
			itm.message.name = archive->read_string(); // name

			if (!archive->read_object_end()) {
				archive->skip_object(true);
				ZKLOGW("CutsceneLibrary", "oCMsgConversation(\"%s\") not fully parsed", itm.name.c_str());
			}

			if (!archive->read_object_end()) {
				// FIXME: in Gothic I cutscene libraries, there is a `synchronized` attribute here
				archive->skip_object(true);
				ZKLOGW("CutsceneLibrary", "zCCSAtomicBlock(\"%s\") not fully parsed", itm.name.c_str());
			}

			if (!archive->read_object_end()) {
				archive->skip_object(true);
				ZKLOGW("CutsceneLibrary", "zCCSBlock(\"%s\") not fully parsed", itm.name.c_str());
			}
		}

		if (!archive->read_object_end()) {
			ZKLOGW("CutsceneLibrary", "Not fully parsed");
		}

		// Prepare blocks for binary search in block_by_name
		std::sort(this->blocks.begin(), this->blocks.end(), [](auto const& a, auto const& b) {
			return a.name < b.name;
		});
	}

	void CutsceneLibrary::save(Write* r, ArchiveFormat fmt) const {
		auto archive = WriteArchive::to(r, fmt);
		archive->write_object_begin("%", "zCCSLib", 0);
		archive->write_int("NumOfItems", static_cast<int32_t>(this->blocks.size()));

		for (auto& block : this->blocks) {
			archive->write_object_begin("%", "zCCSBlock", 0);
			archive->write_string("blockName", block.name);
			archive->write_int("numOfBlocks", 1);
			archive->write_float("subBlock0", 0.0f); // TODO(lmichaelis): Purpose unknown.

			archive->write_object_begin("%", "zCCSAtomicBlock", 0);
			archive->write_object_begin("%", "oCMsgConversation:oCNpcMessage:zCEventMessage", 0);

			archive->write_enum("subType", block.message.type);
			archive->write_string("text", block.message.text);
			archive->write_string("name", block.message.name);

			archive->write_object_end();
			archive->write_object_end();

			archive->write_object_end();
		}

		archive->write_object_end();
		archive->write_header();
	}
} // namespace zenkit
