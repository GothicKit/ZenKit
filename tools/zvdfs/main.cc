// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vdfs.hh>

#include <flags.h>
#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "config.hh"

namespace fs = std::filesystem;
namespace px = phoenix;

static constexpr const auto HELP_MESSAGE =
    R"(USAGE
    zvdfs -v
    zvdfs -h
    zvdfs -l [-f FILE]
    zvdfs -x [NAME] [-o PATH]


DESCRIPTION
    Extracts or lists files from VDF archives. For machine readable output, see zdump.

OPTIONS
    -v, --version                  Print the version of zvdfs
    -h, --help                     Print this help message
    -f, --file                     Read the VDF from FILE instead of stdin
    -l, --list                     Print a list of all files in the VDF.
	-o PATH, --output PATH         Write data to the given path instead of stdout.
    -x [NAME], --extract [NAME]    Extract one or multiple files from the VDF.
                                   If NAME is not specified, extract all files from the VDF
                                   into the current working directory or the path specified
                                   with -o. If NAME is specified, extract the file with the
                                   same name from the VDF and write its content to stdout or
                                   the file given with -o.

VERSION
    phoenix zvdfs v{}
)";

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

		fmt::print("{}\n", path.string<char>());

		if (entry.is_directory()) {
			do_list(path, entry.children());
		}
	}
}

int main(int argc, char** argv) {
	const flags::args args {argc, argv};

	std::optional<std::string> input = args.get<std::string>("f");
	if (!input)
		input = args.get<std::string>("file");

	try {
		if (args.get<bool>("v", false) || args.get<bool>("version", false)) {
			fmt::print("zvdfs v{}\n", ZVDFS_VERSION);
		} else if (args.get<bool>("h", false) || args.get<bool>("help", false)) {
			fmt::print(HELP_MESSAGE, ZVDFS_VERSION);
		} else {
			auto in = px::buffer::empty();
			if (input) {
				in = px::buffer::mmap(*input);
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

			if (args.get<bool>("l", false) || args.get<bool>("list", false)) {
				auto vdf = phoenix::vdf_file::open(in);
				do_list("", vdf.entries());
			} else if (args.get<bool>("x", false) || args.get<bool>("extract", false)) {
				std::optional<std::string> extract;
				(extract = args.get<std::string>("x")) || (extract = args.get<std::string>("extract"));

				auto vdf = phoenix::vdf_file::open(in);
				if (extract) {
					auto* entry = vdf.find_entry(*extract);
					if (entry == nullptr) {
						fmt::print(stderr, "cannot extract entry {}: not found\n", *extract);
						return EXIT_FAILURE;
					}

					auto buf = entry->open();

					std::optional<std::string> output_arg;
					if ((output_arg = args.get<std::string>("o")) || (output_arg = args.get<std::string>("output"))) {
						std::ofstream out {*output_arg, std::ios::binary};
						out.write((const char*) buf.array().data(), buf.limit());
						out.close();
					} else {
						std::cout.write((const char*) buf.array().data(), buf.limit());
					}
				} else {
					std::optional<std::string> output_arg;
					if ((output_arg = args.get<std::string>("o")) || (output_arg = args.get<std::string>("output"))) {
						if (!fs::is_directory(*output_arg)) {
							fmt::print(stderr, "the output directory does not exist.\n");
							return EXIT_FAILURE;
						}

						do_extract(*output_arg, "", vdf.entries());
					} else {
						do_extract(".", "", vdf.entries());
					}
				}
			}
		}
	} catch (const std::exception& e) {
		fmt::print(stderr, "failed to read from vdf: {}", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}
