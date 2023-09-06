// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/font.hh>

#include <doctest/doctest.h>

using namespace phoenix;

static const glyph G1_GLYPH0 {0, {glm::vec2 {0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}}};
static const glyph G1_GLYPH127 {8, {glm::vec2 {0.3984375f, 0.23828125f}, glm::vec2 {0.412109375f, 0.30859375f}}};
static const glyph G1_GLYPH255 {9, {glm::vec2 {0.95703125f, 0.55078125f}, glm::vec2 {0.97265625f, 0.62109375f}}};

static const glyph G2_GLYPH0 {0, {glm::vec2 {0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}}};
static const glyph G2_GLYPH127 {8, {glm::vec2 {0.3984375f, 0.23828125f}, glm::vec2 {0.412109375f, 0.30859375f}}};
static const glyph G2_GLYPH255 {10, {glm::vec2 {0.958984375f, 0.55078125f}, glm::vec2 {0.9765625f, 0.62109375f}}};

TEST_SUITE("font") {
	TEST_CASE("font(parse:g1)") {
		auto in = buffer::mmap("./samples/G1/FONT_OLD_10_WHITE_HI.FNT");
		auto fnt = font::parse(in);

		CHECK_EQ(fnt.name, "FONT_OLD_10_WHITE_HI.TGA");
		CHECK_EQ(fnt.height, 17);
		CHECK_EQ(fnt.glyphs.size(), 256);

		CHECK_EQ(fnt.glyphs[0], G1_GLYPH0);
		CHECK_EQ(fnt.glyphs[127], G1_GLYPH127);
		CHECK_EQ(fnt.glyphs[255], G1_GLYPH255);
	}

	TEST_CASE("font(parse:g2)") {
		auto in = buffer::mmap("./samples/G2/FONT_OLD_10_WHITE_HI.FNT");
		auto fnt = font::parse(in);

		CHECK_EQ(fnt.name, "FONT_OLD_10_WHITE_HI.TGA");
		CHECK_EQ(fnt.height, 18);
		CHECK_EQ(fnt.glyphs.size(), 256);

		CHECK_EQ(fnt.glyphs[0], G2_GLYPH0);
		CHECK_EQ(fnt.glyphs[127], G2_GLYPH127);
		CHECK_EQ(fnt.glyphs[255], G2_GLYPH255);
	}
}
