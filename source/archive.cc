// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>

#include "archive/archive_ascii.hh"
#include "archive/archive_binary.hh"
#include "archive/archive_binsafe.hh"

#include <fmt/format.h>

namespace phoenix {
	archive_header archive_header::parse(buffer& in) {
		try {
			archive_header header {};

			if (in.get_line() != "ZenGin Archive") {
				throw parser_error {"archive_header", "magic missing"};
			}

			std::string version = in.get_line();
			if (!version.starts_with("ver ")) {
				throw parser_error {"archive_header", "ver field missing"};
			}
			header.version = std::stoi(version.substr(version.find(' ') + 1));

			header.archiver = in.get_line();

			auto format = in.get_line();
			if (format == "ASCII") {
				header.format = archive_format::ascii;
			} else if (format == "BINARY") {
				header.format = archive_format::binary;
			} else if (format == "BIN_SAFE") {
				header.format = archive_format::binsafe;
			}

			std::string save_game = in.get_line();
			if (!save_game.starts_with("saveGame ")) {
				throw parser_error {"archive_header", "saveGame field missing"};
			}
			header.save = std::stoi(save_game.substr(save_game.find(' ') + 1)) != 0;

			std::string optional = in.get_line();
			if (optional.starts_with("date ")) {
				header.date = optional.substr(optional.find(' ') + 1);
				optional = in.get_line();
			}

			if (optional.starts_with("user ")) {
				header.user = optional.substr(optional.find(' ') + 1);
				optional = in.get_line();
			}

			if (optional != "END") {
				throw parser_error {"archive_header", "first END missing"};
			}

			return header;
		} catch (const buffer_error& exc) {
			throw parser_error {"archive_header", exc, "eof reached"};
		}
	}

	std::unique_ptr<archive_reader> archive_reader::open(buffer& in) {
		auto header = archive_header::parse(in);
		std::unique_ptr<archive_reader> reader;

		if (header.format == archive_format::ascii) {
			reader = std::make_unique<archive_reader_ascii>(in, std::move(header));
		} else if (header.format == archive_format::binary) {
			reader = std::make_unique<archive_reader_binary>(in, std::move(header));
		} else if (header.format == archive_format::binsafe) {
			reader = std::make_unique<archive_reader_binsafe>(in, std::move(header));
		} else {
			throw parser_error {"archiver_reader",
			                    fmt::format("format '{}' is not supported", std::uint32_t(header.format))};
		}

		reader->read_header();
		return reader;
	}

	void archive_reader::skip_object(bool skip_current) {
		archive_object tmp;
		int32_t level = skip_current ? 1 : 0;

		do {
			if (read_object_begin(tmp)) {
				++level;
			} else if (read_object_end()) {
				--level;
			} else {
				skip_entry();
			}
		} while (level > 0);
	}
} // namespace phoenix
