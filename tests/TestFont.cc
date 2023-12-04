// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <zenkit/Font.hh>
#include <zenkit/Stream.hh>

#include <doctest/doctest.h>

using namespace zenkit;

static FontGlyph const G1_GLYPH0 {0, {glm::vec2 {0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}}};
static FontGlyph const G1_GLYPH127 {8, {glm::vec2 {0.3984375f, 0.23828125f}, glm::vec2 {0.412109375f, 0.30859375f}}};
static FontGlyph const G1_GLYPH255 {9, {glm::vec2 {0.95703125f, 0.55078125f}, glm::vec2 {0.97265625f, 0.62109375f}}};

static FontGlyph const G2_GLYPH0 {0, {glm::vec2 {0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}}};
static FontGlyph const G2_GLYPH127 {8, {glm::vec2 {0.3984375f, 0.23828125f}, glm::vec2 {0.412109375f, 0.30859375f}}};
static FontGlyph const G2_GLYPH255 {10, {glm::vec2 {0.958984375f, 0.55078125f}, glm::vec2 {0.9765625f, 0.62109375f}}};

TEST_SUITE("Font") {
	TEST_CASE("Font.load(GOTHIC1)") {
		auto in = zenkit::Read::from("./samples/G1/FONT_OLD_10_WHITE_HI.FNT");
		zenkit::Font fnt {};
		fnt.load(in.get());

		CHECK_EQ(fnt.name, "FONT_OLD_10_WHITE_HI.TGA");
		CHECK_EQ(fnt.height, 17);
		CHECK_EQ(fnt.glyphs.size(), 256);

		CHECK_EQ(fnt.glyphs[0], G1_GLYPH0);
		CHECK_EQ(fnt.glyphs[127], G1_GLYPH127);
		CHECK_EQ(fnt.glyphs[255], G1_GLYPH255);
	}

	TEST_CASE("Font.load(GOTHIC2)") {
		auto in = zenkit::Read::from("./samples/G2/FONT_OLD_10_WHITE_HI.FNT");
		zenkit::Font fnt {};
		fnt.load(in.get());

		CHECK_EQ(fnt.name, "FONT_OLD_10_WHITE_HI.TGA");
		CHECK_EQ(fnt.height, 18);
		CHECK_EQ(fnt.glyphs.size(), 256);

		CHECK_EQ(fnt.glyphs[0], G2_GLYPH0);
		CHECK_EQ(fnt.glyphs[127], G2_GLYPH127);
		CHECK_EQ(fnt.glyphs[255], G2_GLYPH255);
	}
}
