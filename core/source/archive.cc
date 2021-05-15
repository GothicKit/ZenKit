// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/archive.hh>
#include <phoenix/detail/error.hh>

#include "archive/archive_ascii.hh"
#include "archive/archive_binary.hh"
#include "archive/archive_binsafe.hh"

#include <fmt/format.h>

namespace phoenix {
	archive_header archive_header::read(reader& in) {
		archive_header header {};

		if (in.read_line() != "ZenGin Archive") {
			throw parser_error("not an archive: magic missing");
		}

		std::string version = in.read_line();
		if (!version.starts_with("ver ")) { throw parser_error("not an archive: ver missing"); }
		header.version = std::stoi(version.substr(version.find(' ') + 1));

		header.archiver = in.read_line();
		header.format = in.read_line();

		std::string save_game = in.read_line();
		if (!save_game.starts_with("saveGame ")) { throw parser_error("not an archive: saveGame missing"); }
		header.save = std::stoi(save_game.substr(save_game.find(' ') + 1)) != 0;

		std::string optional = in.read_line();
		if (optional.starts_with("date ")) {
			header.date = optional.substr(optional.find(' ') + 1);
			optional = in.read_line();
		}

		if (optional.starts_with("user ")) {
			header.user = optional.substr(optional.find(' ') + 1);
			optional = in.read_line();
		}

		if (optional != "END") {
			throw parser_error("not an archive: END(1) missing");
		}

		return header;
	}

	std::unique_ptr<archive_reader> archive_reader::open(reader& in) {
		auto header = archive_header::read(in);
		std::unique_ptr<archive_reader> reader;

		if (header.format == "ASCII") {
			reader = std::make_unique<archive_reader_ascii>(in, std::move(header));
		} else if (header.format == "BINARY") {
			reader = std::make_unique<archive_reader_binary>(in, std::move(header));
		} else if (header.format == "BIN_SAFE") {
			reader = std::make_unique<archive_reader_binsafe>(in, std::move(header));
		} else {
			throw parser_error(fmt::format("cannot load archive: format '{}' is not supported", header.format));
		}

		reader->read_header();
		return reader;
	}

	bool archive_reader::peek_input(const std::function<bool(reader&)>& fnc) {
		auto before = input.tell();

		if (!fnc(input)) {
			input.seek(before);
			return false;
		}

		return true;
	}

	void archive_reader::skip_object(bool skip_current) {
		archive_object tmp;
		int level = skip_current ? 1 : 0;

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
}// namespace phoenix