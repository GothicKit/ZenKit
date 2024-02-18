// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <zenkit/Logger.hh>
#include <zenkit/daedalus/Compiler.hh>

#include <iostream>

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Please provide a path to a script.";
		return -1;
	}

	zenkit::Logger::set_default(zenkit::LogLevel::DEBUG);
	zenkit::daedalus::Compiler compiler {};

	compiler.add_src(argv[1]);
	compiler.add("/mnt/projects/GothicKit/phoenix/assets/extern.d");

	// compiler.add("/mnt/projects/GothicKit/phoenix/assets/test.d");
	// compiler.add(argv[1]);
	compiler.compile();
	return 0;
}
