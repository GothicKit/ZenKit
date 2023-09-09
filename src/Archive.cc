// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Archive.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

#include "archive/ArchiveAscii.hh"
#include "archive/ArchiveBinary.hh"
#include "archive/ArchiveBinsafe.hh"

#include "phoenix/buffer.hh"

#include <iostream>

namespace zenkit {
	ReadArchive::ReadArchive(ArchiveHeader head, Read* r) : header(std::move(head)), read(r) {}

	ReadArchive::ReadArchive(ArchiveHeader head, Read* r, std::unique_ptr<Read> owned)
	    : header(std::move(head)), read(r), _m_owned(std::move(owned)) {}

	void ArchiveHeader::load(Read* r) {
		try {
			if (r->read_line(true) != "ZenGin Archive") {
				ZKLOGE("ReadArchive", "Invalid Header");
				throw zenkit::ParserError {"ReadArchive", "magic missing"};
			}

			std::string ver = r->read_line(true);
			if (ver.find("ver ") != 0) {
				throw zenkit::ParserError {"ReadArchive", "ver field missing"};
			}
			this->version = std::stoi(ver.substr(ver.find(' ') + 1));
			this->archiver = r->read_line(true);

			auto fmt = r->read_line(true);
			if (fmt == "ASCII") {
				this->format = ArchiveFormat::ASCII;
			} else if (fmt == "BINARY") {
				this->format = ArchiveFormat::BINARY;
			} else if (fmt == "BIN_SAFE") {
				this->format = ArchiveFormat::BINSAFE;
			}

			std::string save_game = r->read_line(true);
			if (save_game.find("saveGame ") != 0) {
				throw zenkit::ParserError {"ReadArchive", "saveGame field missing"};
			}
			this->save = std::stoi(save_game.substr(save_game.find(' ') + 1)) != 0;

			std::string optional = r->read_line(true);
			if (optional.find("date ") == 0) {
				this->date = optional.substr(optional.find(' ') + 1);
				optional = r->read_line(true);
			}

			if (optional.find("user ") == 0) {
				this->user = optional.substr(optional.find(' ') + 1);
				optional = r->read_line(true);
			}

			if (optional != "END") {
				throw zenkit::ParserError {"ReadArchive", "first END missing"};
			}
		} catch (std::invalid_argument const& e) {
			throw zenkit::ParserError {"ReadArchive", e, "reading int"};
		}
	}

	std::unique_ptr<ReadArchive> ReadArchive::open(phoenix::buffer& in) {
		auto read = zenkit::Read::from(&in);

		ArchiveHeader header {};
		header.load(read.get());

		std::unique_ptr<ReadArchive> reader;
		if (header.format == ArchiveFormat::ASCII) {
			reader = std::make_unique<ReadArchiveAscii>(std::move(header), read.get(), std::move(read));
		} else if (header.format == ArchiveFormat::BINARY) {
			reader = std::make_unique<ReadArchiveBinary>(std::move(header), read.get(), std::move(read));
		} else if (header.format == ArchiveFormat::BINSAFE) {
			reader = std::make_unique<ReadArchiveBinsafe>(std::move(header), read.get(), std::move(read));
		} else {
			throw zenkit::ParserError {"ReadArchive",
			                           "format '" + std::to_string(static_cast<uint32_t>(header.format)) +
			                               "' is not supported"};
		}

		reader->read_header();
		return reader;
	}

	std::unique_ptr<ReadArchive> ReadArchive::from(Read* r) {
		ArchiveHeader header {};
		header.load(r);

		std::unique_ptr<ReadArchive> reader;
		if (header.format == ArchiveFormat::ASCII) {
			reader = std::make_unique<ReadArchiveAscii>(std::move(header), r);
		} else if (header.format == ArchiveFormat::BINARY) {
			reader = std::make_unique<ReadArchiveBinary>(std::move(header), r);
		} else if (header.format == ArchiveFormat::BINSAFE) {
			reader = std::make_unique<ReadArchiveBinsafe>(std::move(header), r);
		} else {
			throw zenkit::ParserError {"ReadArchive",
			                           "format '" + std::to_string(static_cast<uint32_t>(header.format)) +
			                               "' is not supported"};
		}

		reader->read_header();
		return reader;
	}

	void ReadArchive::skip_object(bool skip_current) {
		ArchiveObject tmp;
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
} // namespace zenkit
