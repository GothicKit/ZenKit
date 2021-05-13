// Copyright 2021 Luis Michaelis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
// Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <phoenix/material.hh>

#include "catch2.hh"

using namespace phoenix;

TEST_CASE("Gothic 1 materials are read correctly", "[material]") {
	// Tests two different material archives containing two materials each.

	{
		auto in = reader::from("./samples/material1_g1.mat");
		auto archive = archive_reader::open(in);
		auto m1 = material::read(archive);

		REQUIRE(m1.name() == "DT_BOOKSHELF_V1_1");
		REQUIRE(m1.group() == material_group::undef);
		REQUIRE(m1.color() == 0xFF'2A'23'19 /* ARGB */);
		REQUIRE(m1.smooth_angle() == 60.0f);
		REQUIRE(m1.texture() == "MODIBOOKS01.TGA");
		REQUIRE(m1.texture_scale() == glm::vec2{256.0f, 256.0f});
		REQUIRE(m1.texture_anim_fps() == 0.0f);
		REQUIRE(m1.texture_anim_map_mode() == 0);
		REQUIRE(m1.texture_anim_map_dir() == glm::vec2{9.9999997e-005, 0.0f});
		REQUIRE(!m1.disable_collision());
		REQUIRE(!m1.disable_lightmap());
		REQUIRE(m1.dont_collapse() == 0);
		REQUIRE(m1.detail_object().empty());
		REQUIRE(m1.default_mapping().x == 2.343750f);
		REQUIRE(m1.default_mapping().y == 2.343750f);

		auto m2 = material::read(archive);
		REQUIRE(m2.name() == "DT_BOOKSHELF_V1_2");
		REQUIRE(m2.group() == material_group::undef);
		REQUIRE(m2.color() == 0xFF'39'2A'19 /* ARGB */);
		REQUIRE(m2.smooth_angle() == 60.0f);
		REQUIRE(m2.texture() == "MOWOPLANKS05.TGA");
		REQUIRE(m2.texture_scale() == glm::vec2{128.0f, 128.0f});
		REQUIRE(m2.texture_anim_fps() == 0.0f);
		REQUIRE(m2.texture_anim_map_mode() == 0);
		REQUIRE(m2.texture_anim_map_dir() == glm::vec2{9.9999997e-005, 0.0f});
		REQUIRE(!m2.disable_collision());
		REQUIRE(!m2.disable_lightmap());
		REQUIRE(m2.dont_collapse() == 0);
		REQUIRE(m2.detail_object().empty());
		REQUIRE(m2.default_mapping().x == 2.343750f);
		REQUIRE(m2.default_mapping().y == 2.343750f);
	}

	{
		auto in = reader::from("./samples/material2_g1.mat");
		auto archive = archive_reader::open(in);
		auto m1 = material::read(archive);

		REQUIRE(m1.name() == "MATERIAL #356");
		REQUIRE(m1.group() == material_group::undef);
		REQUIRE(m1.color() == 0xFF'75'46'3B /* ARGB */);
		REQUIRE(m1.smooth_angle() == 60.0f);
		REQUIRE(m1.texture() == "HUM_EBRM2_ARMOR_V0.TGA");
		REQUIRE(m1.texture_scale() == glm::vec2{512.0f, 512.0f});
		REQUIRE(m1.texture_anim_fps() == 0.0f);
		REQUIRE(m1.texture_anim_map_mode() == 0);
		REQUIRE(m1.texture_anim_map_dir() == glm::vec2{9.9999997e-005, 0.0f});
		REQUIRE(!m1.disable_collision());
		REQUIRE(!m1.disable_lightmap());
		REQUIRE(m1.dont_collapse() == 0);
		REQUIRE(m1.detail_object().empty());
		REQUIRE(m1.default_mapping().x == 2.343750f);
		REQUIRE(m1.default_mapping().y == 2.343750f);

		auto m2 = material::read(archive);
		REQUIRE(m2.name() == "ERZZB1_TL");
		REQUIRE(m2.group() == material_group::undef);
		REQUIRE(m2.color() == 0xFF'21'1A'17 /* ARGB */);
		REQUIRE(m2.smooth_angle() == 60.0f);
		REQUIRE(m2.texture() == "HUM_EBRM1_ARMOR_V0.TGA");
		REQUIRE(m2.texture_scale() == glm::vec2{256.0f, 512.0f});
		REQUIRE(m2.texture_anim_fps() == 0.0f);
		REQUIRE(m2.texture_anim_map_mode() == 0);
		REQUIRE(m2.texture_anim_map_dir() == glm::vec2{9.9999997e-005, 0.0f});
		REQUIRE(!m2.disable_collision());
		REQUIRE(!m2.disable_lightmap());
		REQUIRE(m2.dont_collapse() == 0);
		REQUIRE(m2.detail_object().empty());
		REQUIRE(m2.default_mapping().x == 2.343750f);
		REQUIRE(m2.default_mapping().y == 2.343750f);
	}
}