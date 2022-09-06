// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/animation.hh>
#include <phoenix/detail/compat.hh>
#include <phoenix/font.hh>
#include <phoenix/mesh.hh>
#include <phoenix/messages.hh>
#include <phoenix/model_hierarchy.hh>
#include <phoenix/model_script.hh>
#include <phoenix/texture.hh>
#include <phoenix/vdfs.hh>

#include <flags.h>
#include <fmt/format.h>

#include <iostream>
#include <regex>

#include "config.hh"
#include "dump.hh"

namespace px = phoenix;

enum class file_format {
	mdh,
	man,
	csl,
	fnt,
	msh,
	tex,
	mds,
	msb,
	unknown,
};

static constexpr const auto HELP_MESSAGE =
    R"(USAGE
    zdump -v
    zdump -h
    zdump [ -f FILE [-e VDF] ] [-j]

DESCRIPTION
    Parses ZenGin data files and prints information about them to stdout. Can also convert ZenGin
    files to JSON text if -j is specified.

OPTIONS
    -v, --version           Print the version of zdump
    -h, --help              Print this help message
    -f FILE, --input FILE   If specified, reads the given file from disk instead of stdin
                            (unless -e is specified).
    -j, --json              Convert the input file to JSON
    -e VDF, --vdf VDF       Instead of reading FILE directly from disk, extract it from VDF instead.

VERSION
    phoenix zdump v{}
)";

int dump(file_format fmt, px::buffer& in, bool json) {
	if (fmt == file_format::mdh) {
		dump(phoenix::model_hierachy::parse(in), json);
	} else if (fmt == file_format::man) {
		dump(phoenix::animation::parse(in), json);
	} else if (fmt == file_format::csl) {
		dump(phoenix::messages::parse(in), json);
	} else if (fmt == file_format::fnt) {
		dump(phoenix::font::parse(in), json);
	} else if (fmt == file_format::msh) {
		dump(phoenix::mesh::parse(in, {}), json);
	} else if (fmt == file_format::tex) {
		dump(phoenix::texture::parse(in), json);
	} else if (fmt == file_format::mds) {
		dump(phoenix::model_script::parse(in), json);
	} else if (fmt == file_format::msb) {
		dump(phoenix::model_script::parse_binary(in), json);
	} else {
		fmt::print(stderr, "format not supported");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

file_format detect_file_format(px::buffer&& buf) {
	buf.mark();
	if (buf.get_string(4) == "ZTEX")
		return file_format::tex;

	buf.reset();
	if (buf.get_ushort() == 0xD100)
		return file_format::mdh;

	buf.reset();
	if (buf.get_ushort() == 0xA000)
		return file_format::man;

	buf.reset();
	if (buf.get_ushort() == 0xB000)
		return file_format::msh;

	buf.reset();
	if (buf.get_ushort() == 0xF000)
		return file_format::msb;

	buf.reset();
	if (std::regex_search(buf.get_line(), std::regex(R"(\s*Model\s*\(\s*"[\w\d]+"\s*\))", std::regex::icase))) {
		return file_format::mds;
	}

	buf.reset();
	if (buf.get_line() == "ZenGin Archive") {
		buf.reset();
		auto copy = buf.duplicate();
		auto reader = phoenix::archive_reader::open(copy);

		px::archive_object obj {};
		if (!reader->read_object_begin(obj))
			return file_format::unknown;

		if (obj.class_name == "zCCSLib")
			return file_format::csl;
	}

	buf.reset();
	if (buf.get_line() == "1") {
		return file_format::fnt;
	}

	return file_format::unknown;
}

int main(int argc, char** argv) {
	const flags::args args {argc, argv};

	if (args.get<bool>("v", false) || args.get<bool>("version", false)) {
		fmt::print("zdump v{}\n", ZDUMP_VERSION);
	} else if (args.get<bool>("h", false) || args.get<bool>("help", false)) {
		fmt::print(HELP_MESSAGE, ZDUMP_VERSION);
	} else {
		try {
			auto in = px::buffer::empty();

			auto input = args.get<std::string>("f");
			if (!input)
				input = args.get<std::string>("input");

			auto vdf = args.get<std::string>("e");
			if (!vdf)
				vdf = args.get<std::string>("vdf");

			if (input) {
				if (vdf) {
					auto container = px::vdf_file::open(*vdf);
					if (auto* entry = container.find_entry(*input); entry != nullptr) {
						in = entry->open();
					} else {
						fmt::print(stderr, "the file named {} was not found in the VDF {}", *input, *vdf);
						return EXIT_FAILURE;
					}
				} else {
					in = phoenix::buffer::mmap(*input);
				}
			} else {
				std::vector<std::byte> data {};

				while (!std::cin.eof()) {
					data.push_back(static_cast<std::byte>(std::cin.get()));
				}

				if (data.empty()) {
					fmt::print(stderr, "no data provided via stdin");
					return EXIT_FAILURE;
				}

				// remove the EOF byte
				data.pop_back();
				in = phoenix::buffer::of(std::move(data));
			}

			return dump(detect_file_format(in.duplicate()), in, args.get<bool>("j") || args.get<bool>("json"));
		} catch (const std::exception& e) {
			fmt::print(stderr, "failed to parse file: {}", e.what());
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
