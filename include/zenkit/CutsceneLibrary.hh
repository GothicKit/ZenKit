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
	class Read;
	class Write;

	/// \brief A single cutscene message.
	struct CutsceneMessage {
		uint32_t type = 0;

		/// \brief The text associated with the message.
		std::string text;

		/// \brief The name of the WAV file containing the message's audio.
		std::string name;
	};

	/// \brief A block containing a cutscene message and a unique name.
	struct CutsceneBlock {
		/// \brief The unique name of the message.
		std::string name;

		/// \brief The content of the message.
		/// \details It seems like it was at one point possible to specify multiple CutsceneMessage objects for each
		///          CutsceneBlock. This seems to have been abandoned, however, so this implementation only supports
		///          one CutsceneMessage per message block.
		CutsceneMessage message;
	};

	/// \brief Represents a cutscene library.
	///
	/// <p>Cutscene libraries, also called message databases, contain voice lines and a reference to the associated
	/// audio recording of voice actors. These files are used in conjunction with scripts to facilitate PC to NPC
	/// conversations in-game. Cutscene libraries are found within the `_work/data/scripts/content/cutscene/` directory
	/// of Gothic and Gothic II installations. Cutscene libraries are *ZenGin* archives in either binary or ASCII
	/// format. They have either the `.DAT` or `.BIN` extension for binary files or the `.CSL` or `.LSC` extension for
	/// text files</p>
	class CutsceneLibrary {
	public:
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static CutsceneLibrary parse(phoenix::buffer& buf);
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static CutsceneLibrary parse(phoenix::buffer&& buf);

		/// \brief Retrieves a message block by it's name.
		/// \param name The name of the block to get
		/// \return A pointer to the block or `nullptr` if the block was not found.
		[[nodiscard]] ZKAPI CutsceneBlock const* block_by_name(std::string_view name) const;

		ZKAPI void load(Read* r);
		ZKAPI void save(Write* r, ArchiveFormat fmt) const;

		/// \brief A list of all message blocks in the database.
		std::vector<CutsceneBlock> blocks {};
	};
} // namespace zenkit
