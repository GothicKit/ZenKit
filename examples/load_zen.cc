// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Vfs.hh"
#include "zenkit/World.hh"

#include <zenkit/Archive.hh>

#include <iostream>

void print_entries(std::unique_ptr<zenkit::ReadArchive>& reader) {
	int level = 0;
	zenkit::ArchiveObject obj {};

	do {
		if (reader->read_object_begin(obj)) {
			level++;
			std::cout << "    Object(class=" << obj.class_name << ", name=" << obj.object_name
			          << ", index=" << obj.index << ", version=" << obj.version << ")\n";
		} else {
			if (level == 0) break;
			// Reading the entries requires knowing their type. See `phoenix/source/world/vob_tree.cc for examples.
			reader->skip_object(true);
			level--;
		}
	} while (true);
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Please provide an input file.";
		return -1;
	}

	zenkit::Vfs vfs;
	vfs.mount_disk(argv[1]);

	zenkit::World wld;

	auto rd = vfs.find("NEWWORLD.ZEN")->open_read();
	wld.load(rd.get());
	return 0;
}
