// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vdfs.hh>

#include <doctest/doctest.h>

using namespace phoenix;

TEST_SUITE("vdf_file") {
	TEST_CASE("vdf_file(open:?)") {
		auto vdf = vdf_file::open("./samples/basic.vdf");

		struct tm creationtime {};
		creationtime.tm_sec = 58;
		creationtime.tm_min = 24;
		creationtime.tm_hour = 11;
		creationtime.tm_mday = 27;
		creationtime.tm_mon = 3;
		creationtime.tm_year = 121;

		CHECK_EQ(vdf.header.comment, "Sample VDF for openzen. Create on 2021-04-27 13:24:59.");
		CHECK_EQ(vdf.header.timestamp, mktime(&creationtime));
		CHECK_EQ(vdf.header.entry_count, 7);
		CHECK_EQ(vdf.header.file_count, 5);
		CHECK_EQ(vdf.header.size, 43804);
		CHECK_EQ(vdf.header.version, 0x50);

		// Checks if all entries are here

		const auto& roots = vdf.entries;
		CHECK_EQ(roots.size(), 3);

		const auto* config_yml = vdf.find_entry("config.yml");
		CHECK_NE(config_yml, nullptr);
		CHECK(config_yml->is_file());

		const auto* readme_md = vdf.find_entry("readme.md");
		CHECK_NE(readme_md, nullptr);
		CHECK(readme_md->is_file());

		const auto* licenses_dir = vdf.find_entry("licenses");
		CHECK_NE(licenses_dir, nullptr);
		CHECK(licenses_dir->is_directory());
		CHECK_EQ(licenses_dir->children.size(), 2);

		const auto* mit_md = vdf.find_entry("MIT.MD");
		CHECK_NE(mit_md, nullptr);
		CHECK(mit_md->is_file());

		const auto* gpl_dir = licenses_dir->find_child("gpl");
		CHECK_NE(gpl_dir, nullptr);
		CHECK(gpl_dir->is_directory());
		CHECK_EQ(gpl_dir->children.size(), 2);

		const auto* lgpl_md = gpl_dir->find_child("lgpl-3.0.md");
		CHECK_EQ(gpl_dir->find_child("lgpl"), nullptr);
		CHECK_NE(lgpl_md, nullptr);
		CHECK(lgpl_md->is_file());

		const auto* gpl_md = vdf.find_entry("gpl-3.0.MD");
		CHECK_NE(gpl_md, nullptr);
		CHECK(gpl_md->is_file());

		CHECK_NE(vdf.find_entry("lGpL-3.0.Md"), nullptr);
		CHECK_EQ(vdf.find_entry("nonexistent"), nullptr);
		CHECK_NE(vdf.find_entry("liceNSES"), nullptr);
		CHECK_EQ(vdf.find_entry(""), nullptr);

		CHECK_NE(vdf.resolve_path("licEnSES/GPL/gpl-3.0.md"), nullptr);
		CHECK_EQ(vdf.resolve_path("licEnSES/GPL/nonexistent"), nullptr);
		CHECK_NE(vdf.resolve_path("LICENSES"), nullptr);
		CHECK_EQ(vdf.resolve_path(""), nullptr);
	}

	TEST_CASE("vdf_file(open:g1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("vdf_file(open:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}
