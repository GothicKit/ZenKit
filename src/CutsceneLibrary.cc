// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/CutsceneLibrary.hh"
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

#include "phoenix/buffer.hh"
#include "zenkit/vobs/Misc.hh"

#include <algorithm>

namespace zenkit {
	void ConversationMessageEvent::load(ReadArchive& r, GameVersion version) {
		this->type = r.read_enum(); // subType

		if (r.is_save_game()) {
			this->high_priority = r.read_bool(); // highPriority
			this->used = r.read_bool();          // inUse
			this->deleted = r.read_bool();       // deleted
		}

		this->text = r.read_string(); // text
		this->name = r.read_string(); // name
	}

	void ConversationMessageEvent::save(WriteArchive& w, GameVersion version) const {
		w.write_enum("subType", this->type);

		if (w.is_save_game()) {
			w.write_bool("highPriority", this->high_priority);
			w.write_bool("inUse", this->used);
			w.write_bool("deleted", this->deleted);
		}

		w.write_string("text", this->text);
		w.write_string("name", this->name);
	}

	void CutsceneAtomicBlock::load(ReadArchive& r, GameVersion version) {
		this->message = r.read_object<ConversationMessageEvent>(version);
	}

	void CutsceneAtomicBlock::save(WriteArchive& w, GameVersion version) const {
		w.write_object(this->message, version);
	}

	std::shared_ptr<ConversationMessageEvent> CutsceneBlock::get_message() const {
		auto const* cur = this;
		while (std::holds_alternative<std::shared_ptr<CutsceneBlock>>(cur->block)) {
			cur = std::get<std::shared_ptr<CutsceneBlock>>(cur->block).get();
			if (cur == nullptr) {
				return nullptr;
			}
		}

		return std::get<std::shared_ptr<CutsceneAtomicBlock>>(cur->block)->message;
	}

	void CutsceneBlock::set_message(std::shared_ptr<ConversationMessageEvent> msg) {
		auto const* cur = this;
		while (std::holds_alternative<std::shared_ptr<CutsceneBlock>>(cur->block)) {
			cur = std::get<std::shared_ptr<CutsceneBlock>>(cur->block).get();
			if (cur == nullptr) {
				return;
			}
		}

		std::get<std::shared_ptr<CutsceneAtomicBlock>>(cur->block)->message = std::move(msg);
	}

	void CutsceneBlock::load(ReadArchive& r, GameVersion version) {
		this->name = r.read_string(); // blockName

		auto block_count = r.read_int(); // numOfBlocks
		(void) r.read_float();           // subBlock0

		if (block_count != 1) {
			throw ParserError {"CutsceneLibrary",
			                   "expected only one block but got " + std::to_string(block_count) + " for " + this->name};
		}

		auto block = r.read_object(version);
		if (block->get_object_type() == ObjectType::zCCSAtomicBlock) {
			this->block = std::static_pointer_cast<CutsceneAtomicBlock>(block);
		} else if (block->get_object_type() == ObjectType::zCCSBlock) {
			this->block = std::static_pointer_cast<CutsceneBlock>(block);
		} else {
			throw ParserError {"CutsceneLibrary",
			                   "Unexpected block type: " + std::to_string(static_cast<int>(block->get_object_type())) +
			                       " for " + this->name};
		}
	}

	void CutsceneBlock::save(WriteArchive& w, GameVersion version) const {
		w.write_string("blockName", this->name);
		w.write_int("numOfBlocks", 1);
		w.write_float("subBlock0", 0);

		if (std::holds_alternative<std::shared_ptr<CutsceneBlock>>(this->block)) {
			w.write_object(std::get<std::shared_ptr<CutsceneBlock>>(this->block), version);
		} else {
			w.write_object(std::get<std::shared_ptr<CutsceneAtomicBlock>>(this->block), version);
		}
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
		this->blocks.reserve(static_cast<std::uint64_t>(item_count));

		for (auto i = 0; i < item_count; ++i) {
			this->blocks.push_back(r.read_object<CutsceneBlock>(GameVersion::GOTHIC_1));
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

	void CutsceneProps::load(ReadArchive& r, GameVersion version) {
		this->name = r.read_string();                    // name
		this->global = r.read_bool();                    // globalCutscene
		this->loop = r.read_bool();                      // csLoop
		this->has_to_be_triggered = r.read_bool();       // hasToBeTriggerd
		this->distance = r.read_float();                 // distance
		this->range = r.read_float();                    // range
		this->locked_block_count = r.read_int();         // numLockedBlocks
		this->run_behaviour = r.read_enum();             // runBehaviour
		this->run_behaviour_value = r.read_int();        // runBehaviourValue
		this->stage_name = r.read_string();              // StageName
		this->script_function_on_stop = r.read_string(); // scriptFuncOnStop
	}

	void CutsceneProps::save(WriteArchive& w, GameVersion version) const {
		w.write_string("name", this->name);
		w.write_bool("globalCutscene", this->global);
		w.write_bool("csLoop", this->loop);
		w.write_bool("hasToBeTriggerd", this->has_to_be_triggered);
		w.write_float("distance", this->distance);
		w.write_float("range", this->range);
		w.write_int("numLockedBlocks", this->locked_block_count);
		w.write_enum("runBehaviour", this->run_behaviour);
		w.write_int("runBehaviourValue", this->run_behaviour_value);
		w.write_string("StageName", this->stage_name);
		w.write_string("scriptFuncOnStop", this->script_function_on_stop);
	}

	Cutscene::Cutscene() {
		this->block = std::make_shared<CutsceneBlock>();
	}

	void Cutscene::load(ReadArchive& r, GameVersion version) {
		CutsceneBlock::load(r, version);
	}

	void Cutscene::save(WriteArchive& w, GameVersion version) const {
		CutsceneBlock::save(w, version);
	}

	void CutsceneContext::load(ReadArchive& r, GameVersion version) {
		Cutscene::load(r, version);

		this->props = r.read_object<CutsceneProps>(version);
		this->role_count = r.read_int();     // numOfRoles
		this->role_vob_count = r.read_int(); // numOfRoleVobs
		this->npc = r.read_object<VNpc>(version);
		this->main_role = r.read_object<VNpc>(version); // mainRoleVob
		this->is_cutscene = r.read_bool();              // isCutscene
		this->reference = r.read_int();                 // reference
		this->actual_block = r.read_int();              // actualBlock
		this->alt_stage = r.read_object(version);       // alt_stage
		this->stage = r.read_object(version);           // stage
		this->was_triggered = r.read_bool();            // wasTriggered
	}

	void CutsceneContext::save(WriteArchive& w, GameVersion version) const {
		Cutscene::save(w, version);

		w.write_object(this->props, version);
		w.write_int("numOfRoles", this->role_count);
		w.write_int("numOfRoleVobs", this->role_vob_count);
		w.write_object(this->npc.lock(), version);
		w.write_object("mainRoleVob", this->main_role.lock(), version);
		w.write_bool("isCutscene", this->is_cutscene);
		w.write_int("reference", this->reference);
		w.write_int("actualBlock", this->actual_block);
		w.write_object("alt_stage", this->alt_stage, version);
		w.write_object("stage", this->stage, version);
		w.write_bool("wasTriggered", this->was_triggered);
	}
} // namespace zenkit
