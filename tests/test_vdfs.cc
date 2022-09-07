// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vdfs.hh>

#include <doctest/doctest.h>

using namespace phoenix;

TEST_SUITE("vdfs") {
	TEST_CASE("vdf files are read correctly") {
		auto vdf = vdf_file::open("./samples/basic.vdf");

		struct tm creationtime {};
		creationtime.tm_sec = 58;
		creationtime.tm_min = 24;
		creationtime.tm_hour = 11;
		creationtime.tm_mday = 27;
		creationtime.tm_mon = 3;
		creationtime.tm_year = 121;

		CHECK(vdf.header().comment() == "Sample VDF for openzen. Create on 2021-04-27 13:24:59.");
		CHECK(vdf.header().timestamp() == mktime(&creationtime));
		CHECK(vdf.header().entry_count() == 7);
		CHECK(vdf.header().file_count() == 5);
		CHECK(vdf.header().size() == 43804);
		CHECK(vdf.header().version() == 0x50);

		// Checks if all entries are here

		const auto& roots = vdf.entries();
		CHECK(roots.size() == 3);

		const auto* config_yml = vdf.find_entry("config.yml");
		CHECK(config_yml != nullptr);
		CHECK(config_yml->is_file());

		const auto* readme_md = vdf.find_entry("readme.md");
		CHECK(readme_md != nullptr);
		CHECK(readme_md->is_file());

		const auto* licenses_dir = vdf.find_entry("licenses");
		CHECK(licenses_dir != nullptr);
		CHECK(licenses_dir->is_directory());
		CHECK(licenses_dir->children().size() == 2);

		const auto* mit_md = vdf.find_entry("MIT.MD");
		CHECK(mit_md != nullptr);
		CHECK(mit_md->is_file());

		const auto* gpl_dir = licenses_dir->find_child("gpl");
		CHECK(gpl_dir != nullptr);
		CHECK(gpl_dir->is_directory());
		CHECK(gpl_dir->children().size() == 2);

		const auto* lgpl_md = gpl_dir->find_child("lgpl-3.0.md");
		CHECK(gpl_dir->find_child("lgpl") == nullptr);
		CHECK(lgpl_md != nullptr);
		CHECK(lgpl_md->is_file());

		const auto* gpl_md = vdf.find_entry("gpl-3.0.MD");
		CHECK(gpl_md != nullptr);
		CHECK(gpl_md->is_file());
	}

	TEST_CASE("entries in VDFs are correctly resolved") {
		auto vdf = vdf_file::open("./samples/basic.vdf");

		CHECK(vdf.find_entry("lGpL-3.0.Md") != nullptr);
		CHECK(vdf.find_entry("nonexistent") == nullptr);
		CHECK(vdf.find_entry("liceNSES") != nullptr);
		CHECK(vdf.find_entry("") == nullptr);

		CHECK(vdf.resolve_path("licEnSES/GPL/gpl-3.0.md") != nullptr);
		CHECK(vdf.resolve_path("licEnSES/GPL/nonexistent") == nullptr);
		CHECK(vdf.resolve_path("LICENSES") != nullptr);
		CHECK(vdf.resolve_path("") == nullptr);
	}
}
