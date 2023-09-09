// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>

#include <iostream>

void print_entries(std::unique_ptr<phoenix::archive_reader>& reader) {
	int level = 0;
	phoenix::archive_object obj {};

	do {
		if (reader->read_object_begin(obj)) {
			level++;
			std::cout << "    Object(class=" << obj.class_name << ", name=" << obj.object_name
			          << ", index=" << obj.index << ", version=" << obj.version << ")\n";
		} else {
			if (level == 0)
				break;
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

	auto buf = phoenix::buffer::mmap(argv[1]);
	auto archive = phoenix::archive_reader::open(buf);
	auto& header = archive->get_header();

	std::cout << "Archiver: " << header.archiver << "\n"
	          << "User: " << header.user << "\n"
	          << "Timestamp: " << header.date << "\n"
	          << "Entries:\n";

	print_entries(archive);
	return 0;
}
