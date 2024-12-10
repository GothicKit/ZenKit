// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Archive.hh"
#include "zenkit/Library.hh"

#include <cstdint>
#include <string>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	struct VNpc;

	class Read;
	class Write;

	/// \brief A single cutscene message.
	struct ConversationMessageEvent final : Object {
		ZK_OBJECT(ObjectType::oCMsgConversation);

		uint32_t type = 0;

		/// \brief The text associated with the message.
		std::string text;

		/// \brief The name of the WAV file containing the message's audio.
		std::string name;

		bool high_priority = false;
		bool used = false;
		bool deleted = false;

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	struct CutsceneAtomicBlock final : Object {
		ZK_OBJECT(ObjectType::zCCSAtomicBlock);

		std::shared_ptr<ConversationMessageEvent> message = std::make_shared<ConversationMessageEvent>();
		bool synchronized = false;

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	/// \brief A block containing a cutscene message and a unique name.
	struct CutsceneBlock : Object {
		ZK_OBJECT(ObjectType::zCCSBlock);

		/// \brief The unique name of the message.
		std::string name;

		/// \brief The content of the message.
		/// \details It seems like it was at one point possible to specify multiple CutsceneMessage objects for each
		///          CutsceneBlock. This seems to have been abandoned, however, so this implementation only supports
		///          one CutsceneMessage per message block.
		std::variant<std::shared_ptr<CutsceneAtomicBlock>, std::shared_ptr<CutsceneBlock>> block =
		    std::make_shared<CutsceneAtomicBlock>();

		[[nodiscard]] std::shared_ptr<ConversationMessageEvent> get_message() const;
		void set_message(std::shared_ptr<ConversationMessageEvent> msg);

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	/// \brief Represents a cutscene library.
	///
	/// <p>Cutscene libraries, also called message databases, contain voice lines and a reference to the associated
	/// audio recording of voice actors. These files are used in conjunction with scripts to facilitate PC to NPC
	/// conversations in-game. Cutscene libraries are found within the `_work/data/scripts/content/cutscene/` directory
	/// of Gothic and Gothic II installations. Cutscene libraries are *ZenGin* archives in either binary or ASCII
	/// format. They have either the `.DAT` or `.BIN` extension for binary files or the `.CSL` or `.LSC` extension for
	/// text files</p>
	class CutsceneLibrary final : public Object {
		ZK_OBJECT(ObjectType::zCCSLib);

	public:
		/// \brief Retrieves a message block by it's name.
		/// \param name The name of the block to get
		/// \return A pointer to the block or `nullptr` if the block was not found.
		[[nodiscard]] ZKAPI std::shared_ptr<CutsceneBlock> block_by_name(std::string_view name) const;

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;

		/// \brief A list of all message blocks in the database.
		std::vector<std::shared_ptr<CutsceneBlock>> blocks {};
	};

	struct CutsceneProps final : Object {
		ZK_OBJECT(ObjectType::zCCSProps);

		std::string name;
		bool global;
		bool loop;
		bool has_to_be_triggered;
		float distance;
		float range;
		int locked_block_count;
		uint32_t run_behaviour;
		int run_behaviour_value;
		std::string stage_name;
		std::string script_function_on_stop;

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	struct Cutscene : CutsceneBlock {
		ZK_OBJECT(ObjectType::zCCutscene);

		Cutscene();

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};

	struct CutsceneContext final : Cutscene {
		ZK_OBJECT(ObjectType::zCCSCutsceneContext);

		std::shared_ptr<CutsceneProps> props;
		int role_count;
		int role_vob_count;
		std::weak_ptr<VNpc> npc;
		std::weak_ptr<VNpc> main_role;
		bool is_cutscene;
		int reference;
		int actual_block;
		std::shared_ptr<Object> alt_stage;
		std::shared_ptr<Object> stage;
		bool was_triggered;

		ZKAPI void load(ReadArchive& r, GameVersion version) override;
		ZKAPI void save(WriteArchive& w, GameVersion version) const override;
	};
} // namespace zenkit
