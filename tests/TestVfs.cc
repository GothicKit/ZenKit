// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/Vfs.hh>

#include <doctest/doctest.h>

using namespace phoenix;

void check_vfs(Vfs const& vdf) {
	// Checks if all entries are here

	const auto& roots = vdf.root().children();
	CHECK_EQ(roots.size(), 3);

	const auto* config_yml = vdf.find("config.yml");
	CHECK_NE(config_yml, nullptr);
	CHECK(config_yml->type() == phoenix::VfsNodeType::FILE);

	const auto* readme_md = vdf.find("readme.md");
	CHECK_NE(readme_md, nullptr);
	CHECK(readme_md->type() == phoenix::VfsNodeType::FILE);

	const auto* licenses_dir = vdf.find("licenses");
	CHECK_NE(licenses_dir, nullptr);
	CHECK(licenses_dir->type() == phoenix::VfsNodeType::DIRECTORY);
	CHECK_EQ(licenses_dir->children().size(), 2);

	const auto* mit_md = vdf.find("MIT.MD");
	CHECK_NE(mit_md, nullptr);
	CHECK(mit_md->type() == phoenix::VfsNodeType::FILE);

	const auto* gpl_dir = licenses_dir->child("gpl");
	CHECK_NE(gpl_dir, nullptr);
	CHECK(gpl_dir->type() == phoenix::VfsNodeType::DIRECTORY);
	CHECK_EQ(gpl_dir->children().size(), 2);

	const auto* lgpl_md = gpl_dir->child("lgpl-3.0.md");
	CHECK_EQ(gpl_dir->child("lgpl"), nullptr);
	CHECK_NE(lgpl_md, nullptr);
	CHECK(lgpl_md->type() == phoenix::VfsNodeType::FILE);

	const auto* gpl_md = vdf.find("gpl-3.0.MD");
	CHECK_NE(gpl_md, nullptr);
	CHECK(gpl_md->type() == phoenix::VfsNodeType::FILE);

	CHECK_NE(vdf.find("lGpL-3.0.Md"), nullptr);
	CHECK_EQ(vdf.find("nonexistent"), nullptr);
	CHECK_NE(vdf.find("liceNSES"), nullptr);
	CHECK_EQ(vdf.find(""), nullptr);

	CHECK_NE(vdf.resolve("licEnSES/GPL/gpl-3.0.md"), nullptr);
	CHECK_EQ(vdf.resolve("licEnSES/GPL/nonexistent"), nullptr);
	CHECK_NE(vdf.resolve("/LICENSES"), nullptr);
	CHECK_NE(vdf.resolve(""), nullptr);
	CHECK_NE(vdf.resolve("/"), nullptr);
}

TEST_SUITE("Vfs") {
	TEST_CASE("Vfs(mount_disk:?)") {
		auto vdf = phoenix::Vfs {};
		vdf.mount_disk("./samples/basic.vdf");
		check_vfs(vdf);
	}

	TEST_CASE("Vfs(mount_host)") {
		auto vdf = phoenix::Vfs {};
		vdf.mount_host("./samples/basic.vdf.dir", "/");
		check_vfs(vdf);
	}

	TEST_CASE("vdf_file(open:g1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("vdf_file(open:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}
