// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/material.hh>

#include <doctest/doctest.h>

using namespace phoenix;

TEST_SUITE("material") {
	TEST_CASE("Gothic 1 materials are read correctly") {
		// Tests two different material archives containing two materials each.

		{
			auto in = buffer::mmap("./samples/material1_g1.mat");
			auto archive = archive_reader::open(in);
			auto m1 = material::parse(*archive);

			CHECK(m1.name == "DT_BOOKSHELF_V1_1");
			CHECK(m1.group == material_group::undefined);
			CHECK(m1.color == glm::u8vec4 {0x19, 0x23, 0x2A, 0xFF});
			CHECK(m1.smooth_angle == 60.0f);
			CHECK(m1.texture == "MODIBOOKS01.TGA");
			CHECK(m1.texture_scale == glm::vec2 {256.0f, 256.0f});
			CHECK(m1.texture_anim_fps == 0.0f);
			CHECK(m1.texture_anim_map_mode == phoenix::animation_mapping_mode::none);
			CHECK(m1.texture_anim_map_dir == glm::vec2 {9.9999997e-005, 0.0f});
			CHECK(!m1.disable_collision);
			CHECK(!m1.disable_lightmap);
			CHECK(!m1.dont_collapse);
			CHECK(m1.detail_object.empty());
			CHECK(m1.default_mapping.x == 2.343750f);
			CHECK(m1.default_mapping.y == 2.343750f);

			auto m2 = material::parse(*archive);
			CHECK(m2.name == "DT_BOOKSHELF_V1_2");
			CHECK(m2.group == material_group::undefined);
			CHECK(m2.color == glm::u8vec4 {0x19, 0x2A, 0x39, 0xFF});
			CHECK(m2.smooth_angle == 60.0f);
			CHECK(m2.texture == "MOWOPLANKS05.TGA");
			CHECK(m2.texture_scale == glm::vec2 {128.0f, 128.0f});
			CHECK(m2.texture_anim_fps == 0.0f);
			CHECK(m2.texture_anim_map_mode == phoenix::animation_mapping_mode::none);
			CHECK(m2.texture_anim_map_dir == glm::vec2 {9.9999997e-005, 0.0f});
			CHECK(!m2.disable_collision);
			CHECK(!m2.disable_lightmap);
			CHECK(m2.dont_collapse == 0);
			CHECK(m2.detail_object.empty());
			CHECK(m2.default_mapping.x == 2.343750f);
			CHECK(m2.default_mapping.y == 2.343750f);
		}

		{
			auto in = buffer::mmap("./samples/material2_g1.mat");
			auto archive = archive_reader::open(in);
			auto m1 = material::parse(*archive);

			CHECK(m1.name == "MATERIAL #356");
			CHECK(m1.group == material_group::undefined);
			CHECK(m1.color == glm::u8vec4 {0x3B, 0x46, 0x75, 0xFF});
			CHECK(m1.smooth_angle == 60.0f);
			CHECK(m1.texture == "HUM_EBRM2_ARMOR_V0.TGA");
			CHECK(m1.texture_scale == glm::vec2 {512.0f, 512.0f});
			CHECK(m1.texture_anim_fps == 0.0f);
			CHECK(m1.texture_anim_map_mode == phoenix::animation_mapping_mode::none);
			CHECK(m1.texture_anim_map_dir == glm::vec2 {9.9999997e-005, 0.0f});
			CHECK(!m1.disable_collision);
			CHECK(!m1.disable_lightmap);
			CHECK(m1.dont_collapse == 0);
			CHECK(m1.detail_object.empty());
			CHECK(m1.default_mapping.x == 2.343750f);
			CHECK(m1.default_mapping.y == 2.343750f);

			auto m2 = material::parse(*archive);
			CHECK(m2.name == "ERZZB1_TL");
			CHECK(m2.group == material_group::undefined);
			CHECK(m2.color == glm::u8vec4 {0x17, 0x1A, 0x21, 0xFF});
			CHECK(m2.smooth_angle == 60.0f);
			CHECK(m2.texture == "HUM_EBRM1_ARMOR_V0.TGA");
			CHECK(m2.texture_scale == glm::vec2 {256.0f, 512.0f});
			CHECK(m2.texture_anim_fps == 0.0f);
			CHECK(m2.texture_anim_map_mode == phoenix::animation_mapping_mode::none);
			CHECK(m2.texture_anim_map_dir == glm::vec2 {9.9999997e-005, 0.0f});
			CHECK(!m2.disable_collision);
			CHECK(!m2.disable_lightmap);
			CHECK(m2.dont_collapse == 0);
			CHECK(m2.detail_object.empty());
			CHECK(m2.default_mapping.x == 2.343750f);
			CHECK(m2.default_mapping.y == 2.343750f);
		}
	}
}
