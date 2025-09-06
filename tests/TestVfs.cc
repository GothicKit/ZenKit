// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <zenkit/Vfs.hh>

#include <doctest/doctest.h>

void check_vfs(zenkit::Vfs const& vdf) {
	// Checks if all entries are here

	auto const& roots = vdf.root().children();
	CHECK_EQ(roots.size(), 3);

	auto const* config_yml = vdf.find("config.yml");
	CHECK_NE(config_yml, nullptr);
	CHECK((config_yml->type() == zenkit::VfsNodeType::FILE));

	auto const* readme_md = vdf.find("readme.md");
	CHECK_NE(readme_md, nullptr);
	CHECK((readme_md->type() == zenkit::VfsNodeType::FILE));

	auto const* licenses_dir = vdf.find("licenses");
	CHECK_NE(licenses_dir, nullptr);
	CHECK((licenses_dir->type() == zenkit::VfsNodeType::DIRECTORY));
	CHECK_EQ(licenses_dir->children().size(), 2);

	auto const* mit_md = vdf.find("MIT.MD");
	CHECK_NE(mit_md, nullptr);
	CHECK((mit_md->type() == zenkit::VfsNodeType::FILE));

	auto const* gpl_dir = licenses_dir->child("gpl");
	CHECK_NE(gpl_dir, nullptr);
	CHECK((gpl_dir->type() == zenkit::VfsNodeType::DIRECTORY));
	CHECK_EQ(gpl_dir->children().size(), 2);

	auto const* lgpl_md = gpl_dir->child("lgpl-3.0.md");
	CHECK_EQ(gpl_dir->child("lgpl"), nullptr);
	CHECK_NE(lgpl_md, nullptr);
	CHECK(lgpl_md->type() == zenkit::VfsNodeType::FILE);

	auto const* gpl_md = vdf.find("gpl-3.0.MD");
	CHECK_NE(gpl_md, nullptr);
	CHECK((gpl_md->type() == zenkit::VfsNodeType::FILE));

	CHECK_NE(vdf.find("lGpL-3.0.Md"), nullptr);
	CHECK_EQ(vdf.find("nonexistent"), nullptr);
	CHECK_NE(vdf.find("liceNSES"), nullptr);
	CHECK_EQ(vdf.find(""), nullptr);

	CHECK_NE(vdf.resolve("licEnSES/GPL/gpl-3.0.md"), nullptr);
	CHECK_EQ(vdf.resolve("licEnSES/GPL/nonexistent"), nullptr);
	CHECK_NE(vdf.resolve("/LICENSES"), nullptr);
	CHECK_NE(vdf.resolve(""), nullptr);
	CHECK_NE(vdf.resolve("/"), nullptr);

	// Ignores trailing whitespace (see #75)
	CHECK_NE(vdf.find("config.yml "), nullptr);
	CHECK_NE(vdf.resolve("licEnSES /GPL/gpl-3.0.md "), nullptr);

	constexpr char expected[] = "# Some random file I guess :D\nname: John Smith\nage: 33";

	auto buf = vdf.find("config.yml")->open_read();
	char data[101] {0};
	buf->read(data, 100);

	CHECK_EQ(std::string(data), expected);
}

TEST_SUITE("Vfs") {
	TEST_CASE("Vfs.mount_disk(GOTHIC?)") {
		auto vdf = zenkit::Vfs {};
		vdf.mount_disk("./samples/basic.vdf");
		check_vfs(vdf);
	}

	TEST_CASE("Vfs.mount_host(GOTHIC?)") {
		auto vdf = zenkit::Vfs {};
		vdf.mount_host("./samples/basic.vdf.dir", "/");
		check_vfs(vdf);
	}
}
