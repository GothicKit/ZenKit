// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/CutsceneLibrary.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

#include "phoenix/buffer.hh"

#include <algorithm>

namespace zenkit {
	void ConversationMessageEvent::load(ReadArchive& r, GameVersion version) {
		this->type = r.read_enum();   // subType
		this->text = r.read_string(); // text
		this->name = r.read_string(); // name
	}

	void ConversationMessageEvent::save(WriteArchive& w, GameVersion version) const {
		w.write_enum("subType", this->type);
		w.write_string("text", this->text);
		w.write_string("name", this->name);
	}

	void CutsceneAtomicBlock::load(ReadArchive& r, GameVersion version) {
		this->message = r.read_object<ConversationMessageEvent>(version);
	}

	void CutsceneAtomicBlock::save(WriteArchive& w, GameVersion version) const {
		w.write_object(this->message, version);
	}

	void CutsceneBlock::load(ReadArchive& r, GameVersion version) {
		this->name = r.read_string();// blockName

		auto block_count = r.read_int(); // numOfBlocks
		(void) r.read_float();           // subBlock0

		if (block_count != 1) {
			throw ParserError {"CutsceneLibrary",
			                   "expected only one block but got " + std::to_string(block_count) + " for " + this->name};
		}

		this->atomic = r.read_object<CutsceneAtomicBlock>(version);
	}

	void CutsceneBlock::save(WriteArchive& w, GameVersion version) const {
		w.write_string("blockName", this->name);
		w.write_int("numOfBlocks", 1);
		w.write_float("subBlock0", 0);
		w.write_object(this->atomic, version);
	}

	std::shared_ptr<CutsceneBlock> CutsceneLibrary::block_by_name(std::string_view name) const {

		auto result = std::lower_bound(this->blocks.begin(), this->blocks.end(), name, [](auto const& it, auto n) {
			return it->name < n;
		});

		if (result == this->blocks.end()) {
			return nullptr;
		}

		return *result;
	}

	void CutsceneLibrary::load(ReadArchive& r, GameVersion version) {
		auto item_count = r.read_int(); // NumOfItems
		this->blocks.resize(static_cast<std::uint64_t>(item_count));

		for (auto i = 0; i < item_count; ++i) {
			this->blocks[i] = r.read_object<CutsceneBlock>(GameVersion::GOTHIC_1);
		}

		// Prepare blocks for binary search in block_by_name
		std::sort(this->blocks.begin(), this->blocks.end(), [](auto const& a, auto const& b) {
			return a->name < b->name;
		});
	}

	void CutsceneLibrary::save(WriteArchive& w, GameVersion version) const {
		w.write_int("NumOfItems", static_cast<int32_t>(this->blocks.size()));

		for (auto& block : this->blocks) {
			w.write_object(block, version);
		}
	}
} // namespace zenkit
