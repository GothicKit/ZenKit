// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <zenkit/Stream.hh>
#include <zenkit/Vfs.hh>

#include <doctest/doctest.h>

#include <stack>
#include <string>
#include <vector>

void check_vfs(zenkit::Vfs const& vdf) {
	// Checks if all entries are here

	auto const& roots = vdf.root().children();
	CHECK_EQ(roots.size(), 3);

	auto const* config_yml = vdf.find("config.yml");
	CHECK_NE(config_yml, nullptr);
	CHECK(config_yml->type() == zenkit::VfsNodeType::FILE);

	auto const* readme_md = vdf.find("readme.md");
	CHECK_NE(readme_md, nullptr);
	CHECK(readme_md->type() == zenkit::VfsNodeType::FILE);

	auto const* licenses_dir = vdf.find("licenses");
	CHECK_NE(licenses_dir, nullptr);
	CHECK(licenses_dir->type() == zenkit::VfsNodeType::DIRECTORY);
	CHECK_EQ(licenses_dir->children().size(), 2);

	auto const* mit_md = vdf.find("MIT.MD");
	CHECK_NE(mit_md, nullptr);
	CHECK(mit_md->type() == zenkit::VfsNodeType::FILE);

	auto const* gpl_dir = licenses_dir->child("gpl");
	CHECK_NE(gpl_dir, nullptr);
	CHECK(gpl_dir->type() == zenkit::VfsNodeType::DIRECTORY);
	CHECK_EQ(gpl_dir->children().size(), 2);

	auto const* lgpl_md = gpl_dir->child("lgpl-3.0.md");
	CHECK_EQ(gpl_dir->child("lgpl"), nullptr);
	CHECK_NE(lgpl_md, nullptr);
	CHECK(lgpl_md->type() == zenkit::VfsNodeType::FILE);

	auto const* gpl_md = vdf.find("gpl-3.0.MD");
	CHECK_NE(gpl_md, nullptr);
	CHECK(gpl_md->type() == zenkit::VfsNodeType::FILE);

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

	TEST_CASE("Vfs.save_compressed") {
		// Build a VFS from scratch with various file types and sizes.
		auto vfs = zenkit::Vfs {};

		// Helper: create a byte pattern of the given size.
		auto make_data = [](size_t size, uint8_t seed) {
			std::vector<std::byte> data(size);
			for (size_t i = 0; i < size; i++) {
				data[i] = static_cast<std::byte>((seed + i * 7 + i / 256) & 0xFF);
			}
			return data;
		};

		struct TestFile {
			std::string path; // directory path (empty for root)
			std::string name;
			std::vector<std::byte> data;
		};

		std::vector<TestFile> test_files = {
		    // Small file — fits in a single block
		    {"", "README.TXT", make_data(128, 0x41)},
		    // File exactly one block (8192 bytes)
		    {"", "ONEBLOCK.BIN", make_data(8192, 0x10)},
		    // Multi-block file — spans several ZippedStream blocks
		    {"SCRIPTS", "STARTUP.D", make_data(25000, 0x55)},
		    // Large compressible data (repeating pattern)
		    {"MESHES", "WORLD.MRM", std::vector<std::byte>(50000, std::byte {0xAB})},
		    // Audio file — should be stored raw even in compressed mode
		    {"SOUND", "THEME.WAV", make_data(4096, 0xCC)},
		    // Another audio format
		    {"SOUND", "VOICE.OGG", make_data(2048, 0xDD)},
		    // File in a nested directory
		    {"TEXTURES/_COMPILED", "STONE.TEX", make_data(16384, 0x77)},
		};

		// Create directory structure and add files
		for (auto& tf : test_files) {
			zenkit::VfsNode* parent = nullptr;
			if (tf.path.empty()) {
				parent = const_cast<zenkit::VfsNode*>(&vfs.root());
			} else {
				parent = &vfs.mkdir(tf.path);
			}

			parent->create(
			    zenkit::VfsNode::file(tf.name, zenkit::VfsFileDescriptor {tf.data.data(), tf.data.size(), false}));
		}

		// Save as compressed VDF
		std::vector<std::byte> compressed_output;
		{
			auto writer = zenkit::Write::to(&compressed_output);
			vfs.save_compressed(writer.get(), zenkit::GameVersion::GOTHIC_2);
		}
		CHECK_GT(compressed_output.size(), 0);

		// Also save uncompressed for size comparison
		std::vector<std::byte> normal_output;
		{
			auto writer = zenkit::Write::to(&normal_output);
			vfs.save(writer.get(), zenkit::GameVersion::GOTHIC_2);
		}

		// Compressed should be smaller (the repeating-pattern file compresses well)
		CHECK_LT(compressed_output.size(), normal_output.size());

		// Reload the compressed VDF and verify every file
		auto vfs_loaded = zenkit::Vfs {};
		auto rd = zenkit::Read::from(&compressed_output);
		vfs_loaded.mount_disk(rd.get());

		for (auto const& tf : test_files) {
			auto const* node = vfs_loaded.find(tf.name);
			REQUIRE_NE(node, nullptr);
			REQUIRE(node->type() == zenkit::VfsNodeType::FILE);

			auto reader = node->open_read();
			reader->seek(0, zenkit::Whence::END);
			auto sz = reader->tell();
			reader->seek(0, zenkit::Whence::BEG);

			CHECK_EQ(sz, tf.data.size());

			std::vector<std::byte> buf(sz);
			reader->read(buf.data(), sz);
			CHECK_EQ(buf, tf.data);
		}

		// Also reload the uncompressed VDF and cross-verify
		auto vfs_normal = zenkit::Vfs {};
		auto rd2 = zenkit::Read::from(&normal_output);
		vfs_normal.mount_disk(rd2.get());

		for (auto const& tf : test_files) {
			auto const* node = vfs_normal.find(tf.name);
			REQUIRE_NE(node, nullptr);

			auto reader = node->open_read();
			reader->seek(0, zenkit::Whence::END);
			auto sz = reader->tell();
			reader->seek(0, zenkit::Whence::BEG);

			CHECK_EQ(sz, tf.data.size());

			std::vector<std::byte> buf(sz);
			reader->read(buf.data(), sz);
			CHECK_EQ(buf, tf.data);
		}
	}
}
