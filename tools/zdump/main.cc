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

#include "config.hh"
#include "dump.hh"

namespace px = phoenix;

static constexpr const auto HELP_MESSAGE =
    R"(USAGE
    zdump [OPTION...] FILE

DESCRIPTION
    Parses ZenGin data files and prints information about them to stdout. Can also convert ZenGin
    files to JSON text if -j is specified.

OPTIONS
    -v, --version          Print the version of zdump
    -h, --help             Print this help message
    -j, --json             Convert the input file to JSON
    -e VDF, --vdf VDF      Instead of reading FILE directly from disk, extract it from VDF instead.

VERSION
    phoenix zdump v{}
)";

int dump(std::string_view ext, px::buffer& in, bool json) {
	if (px::iequals(ext, "MDH")) {
		dump(phoenix::model_hierachy::parse(in), json);
	} else if (px::iequals(ext, "MAN")) {
		dump(phoenix::animation::parse(in), json);
	} else if (px::iequals(ext, "BIN") || px::iequals(ext, "CSL") || px::iequals(ext, "DAT") ||
	           px::iequals(ext, "LSC")) {
		dump(phoenix::messages::parse(in), json);
	} else if (px::iequals(ext, "FNT")) {
		dump(phoenix::font::parse(in), json);
	} else if (px::iequals(ext, "MSH")) {
		dump(phoenix::mesh::parse(in, {}), json);
	} else if (px::iequals(ext, "TEX")) {
		dump(phoenix::texture::parse(in), json);
	} else if (px::iequals(ext, "MDS")) {
		dump(phoenix::model_script::parse(in), json);
	} else if (px::iequals(ext, "MSB")) {
		dump(phoenix::model_script::parse_binary(in), json);
	} else {
		fmt::print(stderr, "format not supported: {}", ext);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
	const flags::args args {argc, argv};

	if (args.get<bool>("v", false) || args.get<bool>("version", false)) {
		fmt::print("zdump v{}\n", ZDUMP_VERSION);
	} else if (args.get<bool>("h", false) || args.get<bool>("help", false)) {
		fmt::print(HELP_MESSAGE, ZDUMP_VERSION);
	} else {
		auto file = args.get<std::string>(0);
		if (!file) {
			fmt::print(stderr, "no input file given");
			return EXIT_FAILURE;
		}

		try {
			auto json = args.get<bool>("j", false) || args.get<bool>("json", false);
			auto in = px::buffer::empty();
			std::optional<std::string> vdf;
			if ((vdf = args.get<std::string>("e")) || (vdf = args.get<std::string>("vdf"))) {
				auto container = px::vdf_file::open(*vdf);

				if (auto* entry = container.find_entry(*file); entry != nullptr) {
					in = entry->open();
				} else {
					fmt::print(stderr, "the file named {} was not found in the VDF {}", *file, *vdf);
					return EXIT_FAILURE;
				}
			} else {
				in = phoenix::buffer::open(*file);
			}

			return dump(file->substr(file->rfind('.') + 1), in, json);
		} catch (const std::exception& e) {
			fmt::print(stderr, "cannot parse file: {}", e.what());
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
