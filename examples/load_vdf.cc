// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <zenkit/Vfs.hh>

#include <iostream>

void print_entries(const std::set<zenkit::VfsNode, zenkit::VfsNodeComparator>& entries) {
	for (auto& e : entries) {
		if (e.type() == zenkit::VfsNodeType::DIRECTORY) {
			print_entries(e.children());
		} else {
			std::cout << e.name() << "\n";
		}
	}
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Please provide an input file.";
		return -1;
	}

	zenkit::Vfs vfs {};
	vfs.mount_disk(argv[1]);
	print_entries(vfs.root().children());
	return 0;
}
