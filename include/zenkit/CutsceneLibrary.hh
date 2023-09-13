// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace phoenix {
	class buffer;
}

namespace zenkit {
	class Read;

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
		/// \details It seems like it was possible to specify multiple atomic_message object for each message_block.
		///          This seems to have been abandoned, however, so this implementation only supports one atomic_message
		///          per message block.
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
		/// \brief Parses a message database from the data in the given buffer.
		///
		/// <p>This implementation is heavily based on the implementation found in
		/// [ZenLib](https://github.com/Try/ZenLib).</p>
		///
		/// \param[in,out] buf The buffer to read from.
		/// \return The parsed message database object.
		/// \note After this function returns the position of \p buf will be at the end of the parsed object.
		///       If you would like to keep your buffer immutable, consider passing a copy of it to #parse(buffer&&)
		///       using buffer::duplicate.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static CutsceneLibrary parse(phoenix::buffer& path);

		/// \brief Parses a message database from the data in the given buffer.
		/// \param[in] buf The buffer to read from (by rvalue-reference).
		/// \return The parsed message database object.
		/// \throws zenkit::ParserError if parsing fails.
		/// \see #parse(buffer&)
		[[nodiscard]] ZKREM("use ::load()") ZKAPI static CutsceneLibrary parse(phoenix::buffer&& path);

		/// \brief Retrieves a message block by it's name.
		/// \param name The name of the block to get
		/// \return A pointer to the block or `nullptr` if the block was not found.
		[[nodiscard]] ZKAPI CutsceneBlock const* block_by_name(std::string_view name) const;

		ZKAPI void load(Read* r);

	public:
		/// \brief A list of all message blocks in the database.
		std::vector<CutsceneBlock> blocks {};
	};
} // namespace zenkit