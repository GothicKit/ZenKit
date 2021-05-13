// Copyright 2021 Luis Michaelis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
// Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <phoenix/detail/error.hh>
#include <phoenix/archive.hh>

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

	std::unique_ptr<archive_reader> archive_reader::open(reader in) {
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
}// namespace openzen