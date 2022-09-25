// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vdfs.hh>

#include <iostream>

void print_entries(const std::vector<phoenix::vdf_entry>& entries) {
	for (auto& e : entries) {
		if (e.is_directory()) {
			print_entries(e.children);
		} else {
			std::cout << "    " << e.name << ": " << e.size << " bytes\n";
		}
	}
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Please provide an input file.";
		return -1;
	}

	auto vdf = phoenix::vdf_file::open(argv[1]);
	auto& header = vdf.header;

	std::cout << "Description: " << header.comment << "\n"
	          << "Timestamp (Unix): " << header.timestamp << "\nEntries:\n";

	print_entries(vdf.entries);
	return 0;
}
