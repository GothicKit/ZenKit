// Copyright © 2025. Luis Michaelis <me@lmichaelis.de>
// SPDX-License-Identifier: MIT-Modern-Variant
#include "zenkit/vobs/Camera.hh"

#include <zenkit/Archive.hh>
#include <zenkit/SaveGame.hh>
#include <zenkit/World.hh>
#include <zenkit/Logger.hh>

#include <iostream>

void print_usage() {
	std::cerr << "Usage: zen2zen [-f FORMAT] [-i VERSION_INPUT] [-o VERSION_OUTPUT] INPUT OUTPUT\n\n"
	    << "Converts ZenGin archives from and to different formats.\n\n"
	    << "Options:\n"
	    << "\t-f FORMAT                      \t\tSet the output file format (default: ascii)\n"
	    << "\t-i,--version-in VERSION_INPUT  \t\tSet the game version the archive is from (default: gothic1)\n"
	    << "\t-o,--version-out VERSION_OUTPUT\t\tSet the game version the archive should be converted to (default: gothic1)\n\n"
	    << "Possible values:\n"
	    << "\tFORMAT        \t\tascii, binary, binsafe\n"
	    << "\tVERSION_INPUT \t\t1, g1, gothic1, 2, g2, gothic2\n"
	    << "\tVERSION_OUTPUT\t\t1, g1, gothic1, 2, g2, gothic2\n";
}


int main(int argc, char const** argv) {
	if (argc < 3) {
		print_usage();
		return 1;
	}

	zenkit::Logger::set_default(zenkit::LogLevel::INFO);
	std::vector<char const*> positional;
	auto fmt = zenkit::ArchiveFormat::ASCII;
	auto ver_input = zenkit::GameVersion::GOTHIC_1;
	auto ver_output = zenkit::GameVersion::GOTHIC_1;

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			if (positional.size() >= 2) {
				std::cerr << "Expected only two positionals, ignoring '" << argv[i] << "'\n";
				continue;
			}
			positional.push_back(argv[i]);
			continue;
		}

		std::string_view name = argv[i];
		if (name == "-h") {
			print_usage();
			return 0;
		}

		if (i + 1 >= argc) {
			std::cerr << "Expected value after option '" << name << "'\n";
			return 1;
		}

		std::string_view arg = argv[++i];
		if (name == "-f" || name == "--format") {
			if (arg == "ascii") fmt = zenkit::ArchiveFormat::ASCII;
			else if (arg == "binary") fmt = zenkit::ArchiveFormat::BINARY;
			else if (arg == "binsafe") fmt = zenkit::ArchiveFormat::BINSAFE;
			else {
				std::cerr << "Expected one of 'ascii' (default), 'binary' or 'binsafe' as a format\n";
				return 1;
			}
		} else if (name == "-i" || name == "--version-in") {
			if (arg == "1" || arg == "g1" || arg == "gothic1") ver_input = zenkit::GameVersion::GOTHIC_1;
			else if (arg == "2" || arg == "g2" || arg == "gothic2") ver_input = zenkit::GameVersion::GOTHIC_2;
			else {
				std::cerr << "Expected one of '1', 'g1' or 'gothic1' (default) or '2', 'g2' or 'gothic2' as an input version\n";
				return 1;
			}
		} else if (name == "-o" || name == "--version-out") {
			if (arg == "1" || arg == "g1" || arg == "gothic1") ver_output = zenkit::GameVersion::GOTHIC_1;
			else if (arg == "2" || arg == "g2" || arg == "gothic2") ver_output = zenkit::GameVersion::GOTHIC_2;
			else {
				std::cerr << "Expected one of '1', 'g1' or 'gothic1' (default) or '2', 'g2' or 'gothic2' as an output version\n";
				return 1;
			}
		} else {
			std::cerr << "Unknown option: " << name << "\n";
			return 1;
		}
	}

	try {
		auto a_in = zenkit::Read::from(positional[0]);
		auto a_ar = zenkit::ReadArchive::from(a_in.get());
		auto a = a_ar->read_object(ver_input);

		auto a_out = zenkit::Write::to(positional[1]);
		auto a_ar_o = zenkit::WriteArchive::to(a_out.get(), fmt);
		a_ar_o->write_object(a, ver_output);
	} catch (const std::exception& e) {
		std::cerr << "Error during conversion: " << e.what() << "\n";
		return -1;
	}

    return 0;
}
