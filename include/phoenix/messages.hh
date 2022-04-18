// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/archive.hh>

#include <string>
#include <unordered_map>
#include <vector>

namespace phoenix {

	struct atomic_message {
		uint32_t type = 0;
		std::string text;
		std::string name;
	};

	struct message_block {
		std::string name;
		atomic_message message;
	};

	class messages {
	public:
		/**
		 * @brief Parses a message database from a file.
		 * @param path The path of the file to read from
		 * @return A proxy to the message database.
		 */
		static messages parse(buffer& path);

		/**
		 * @return All message blocks in the database.
		 */
		inline const std::vector<message_block>& blocks() const noexcept {
			return _m_blocks;
		}

		/**
		 * @brief Retrieves a message block by it's name.
		 * @param name The name of the block to get
		 * @return A pointer to the block or `nullptr` if the block was not found.
		 */
		const message_block* block_by_name(const std::string& name);

	private:
		std::vector<message_block> _m_blocks;
		std::unordered_map<std::string, message_block*> _m_blocks_by_name;
	};

} // namespace phoenix
