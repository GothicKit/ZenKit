// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vdfs.hh>

#include <argh.h>
#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "config.hh"

namespace fs = std::filesystem;

static constexpr const auto HELP_MESSAGE = "Usage: pxvdfs [--version]\n"
										   "       pxvdfs [-h|--help]\n"
										   "       pxvdfs <FILE> [-o|--output <DIR>]\n"
										   "       pxvdfs <FILE> [-x|--extract <NAME_OR_PATH>] [-o|--output <FILE>]\n"
										   "       pxvdfs <FILE> [-l|--list]\n"
										   "\n"
										   "phoenix pxvdfs v{}\n"
										   "Extract contents of VDF files.\n";

static void do_extract(const fs::path& base, const fs::path& self, const std::vector<phoenix::vdf_entry>& entries) {
	for (const auto& entry : entries) {
		auto new_self = self / entry.name();
		auto output = base / new_self;

		if (entry.is_directory()) {
			fs::create_directory(output);
			do_extract(base, new_self, entry.children());
		} else {
			auto content = entry.open();

			std::ofstream out {output};
			out.write((const char*) content.array().data(), content.limit());
			out.close();
		}
	}
}

static void do_list(const fs::path& self, const std::vector<phoenix::vdf_entry>& entries) {
	for (const phoenix::vdf_entry& entry : entries) {
		auto path = self / entry.name();
		fmt::print("{}\n", path.c_str());

		if (entry.is_directory()) {
			do_list(path, entry.children());
		}
	}
}

int main(int argc, const char** argv) {
	argh::parser args {argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION};

	if (args["--version"]) {
		fmt::print("pxvdfs v{}\n", PXVDFS_VERSION);
		return EXIT_SUCCESS;
	}

	if (args[{"-h", "--help"}]) {
		fmt::print(HELP_MESSAGE, PXVDFS_VERSION);
		return EXIT_SUCCESS;
	}

	std::string file;
	if (!(args(1) >> file)) {
		fmt::print(stderr, "please provide an input file.\n");
		return EXIT_FAILURE;
	}

	std::string output;
	if (!(args({"-o", "--output"}) >> output)) {
		output = ".";
	}

	bool list = args[{"-l", "--list"}];

	std::string extract;
	bool extract_one = false;

	if (args({"-x", "--extract-one"}) >> extract) {
		extract_one = true;
	}

	try {
		auto vdf = phoenix::vdf_file::open(file);

		if (list) {
			do_list("", vdf.entries());
		} else if (extract_one) {
			const phoenix::vdf_entry* entry;

			if (auto isdir = extract.find('/'); isdir != std::string::npos) {
				entry = vdf.resolve_path(extract);
			} else {
				entry = vdf.find_entry(extract);
			}

			if (entry == nullptr) {
				fmt::print(stderr, "cannot extract entry {} from {}: not found\n", extract, file);
				return EXIT_FAILURE;
			}

			auto in = entry->open();

			if (output == ".") {
				std::cout.write((const char *) in.array().data(), in.limit());
			} else {
				std::ofstream out {output};
				out.write((const char *) in.array().data(), in.limit());
				out.close();
			}
		} else {
			if (!fs::is_directory(output)) {
				fmt::print(stderr, "the output directory does not exist.\n");
				return EXIT_FAILURE;
			}

			do_extract(output, "", vdf.entries());
		}

	} catch (const std::exception& e) {
		fmt::print(stderr, "cannot extract VDF: {}", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}
