// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/vdfs.hh>

#include "catch2.hh"

using namespace phoenix;

TEST_CASE("vdf files are read correctly", "[vdfs]") {
	auto vdf = vdf_file::open("./samples/basic.vdf");

	struct tm creationtime {};
	creationtime.tm_sec = 58;
	creationtime.tm_min = 24;
	creationtime.tm_hour = 11;
	creationtime.tm_mday = 27;
	creationtime.tm_mon = 3;
	creationtime.tm_year = 121;

	REQUIRE(vdf.header().comment() == "Sample VDF for openzen. Create on 2021-04-27 13:24:59.");
	REQUIRE(vdf.header().timestamp() == timegm(&creationtime));
	REQUIRE(vdf.header().entry_count() == 7);
	REQUIRE(vdf.header().file_count() == 5);
	REQUIRE(vdf.header().size() == 43804);
	REQUIRE(vdf.header().version() == 0x50);

	// Checks if all entries are here

	const auto& roots = vdf.entries();
	REQUIRE(roots.size() == 3);

	const auto* config_yml = vdf.find_entry("config.yml");
	REQUIRE(config_yml != nullptr);
	REQUIRE(config_yml->is_file());

	const auto* readme_md = vdf.find_entry("readme.md");
	REQUIRE(readme_md != nullptr);
	REQUIRE(readme_md->is_file());

	const auto* licenses_dir = vdf.find_entry("licenses");
	REQUIRE(licenses_dir != nullptr);
	REQUIRE(licenses_dir->is_directory());
	REQUIRE(licenses_dir->children().size() == 2);

	const auto* mit_md = vdf.find_entry("MIT.MD");
	REQUIRE(mit_md != nullptr);
	REQUIRE(mit_md->is_file());

	const auto* gpl_dir = licenses_dir->find_child("gpl");
	REQUIRE(gpl_dir != nullptr);
	REQUIRE(gpl_dir->is_directory());
	REQUIRE(gpl_dir->children().size() == 2);

	const auto* lgpl_md = gpl_dir->find_child("lgpl-3.0.md");
	REQUIRE(gpl_dir->find_child("lgpl") == nullptr);
	REQUIRE(lgpl_md != nullptr);
	REQUIRE(lgpl_md->is_file());

	const auto* gpl_md = vdf.find_entry("gpl-3.0.MD");
	REQUIRE(gpl_md != nullptr);
	REQUIRE(gpl_md->is_file());
}

TEST_CASE("entries in VDFs are correctly resolved", "[vdfs]") {
	auto vdf = vdf_file::open("./samples/basic.vdf");

	REQUIRE(vdf.find_entry("lGpL-3.0.Md") != nullptr);
	REQUIRE(vdf.find_entry("nonexistent") == nullptr);
	REQUIRE(vdf.find_entry("liceNSES") != nullptr);
	REQUIRE(vdf.find_entry("") == nullptr);

	REQUIRE(vdf.resolve_path("licEnSES/GPL/gpl-3.0.md") != nullptr);
	REQUIRE(vdf.resolve_path("licEnSES/GPL/nonexistent") == nullptr);
	REQUIRE(vdf.resolve_path("LICENSES") != nullptr);
	REQUIRE(vdf.resolve_path("") == nullptr);
}