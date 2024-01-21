// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Archive.hh"

#include <zenkit/Font.hh>
#include <zenkit/Stream.hh>

#include <doctest/doctest.h>

static constexpr zenkit::FontGlyph G1_GLYPH0 {0, {glm::vec2 {0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}}};
static constexpr zenkit::FontGlyph G1_GLYPH127 {
    8,
    {glm::vec2 {0.3984375f, 0.23828125f}, glm::vec2 {0.412109375f, 0.30859375f}}};
static constexpr zenkit::FontGlyph G1_GLYPH255 {
    9,
    {glm::vec2 {0.95703125f, 0.55078125f}, glm::vec2 {0.97265625f, 0.62109375f}}};

static constexpr zenkit::FontGlyph G2_GLYPH0 {0, {glm::vec2 {0.0f, 0.0f}, glm::vec2 {0.0f, 0.0f}}};
static constexpr zenkit::FontGlyph G2_GLYPH127 {
    8,
    {glm::vec2 {0.3984375f, 0.23828125f}, glm::vec2 {0.412109375f, 0.30859375f}}};
static constexpr zenkit::FontGlyph G2_GLYPH255 {
    10,
    {glm::vec2 {0.958984375f, 0.55078125f}, glm::vec2 {0.9765625f, 0.62109375f}}};

static void verify_g1(zenkit::Font const& fnt) {
	CHECK_EQ(fnt.name, "FONT_OLD_10_WHITE_HI.TGA");
	CHECK_EQ(fnt.height, 17);
	CHECK_EQ(fnt.glyphs.size(), 256);

	CHECK_EQ(fnt.glyphs[0], G1_GLYPH0);
	CHECK_EQ(fnt.glyphs[127], G1_GLYPH127);
	CHECK_EQ(fnt.glyphs[255], G1_GLYPH255);
}

static void verify_g2(zenkit::Font const& fnt) {
	CHECK_EQ(fnt.name, "FONT_OLD_10_WHITE_HI.TGA");
	CHECK_EQ(fnt.height, 18);
	CHECK_EQ(fnt.glyphs.size(), 256);

	CHECK_EQ(fnt.glyphs[0], G2_GLYPH0);
	CHECK_EQ(fnt.glyphs[127], G2_GLYPH127);
	CHECK_EQ(fnt.glyphs[255], G2_GLYPH255);
}

TEST_SUITE("Font") {
	TEST_CASE("Font.load(GOTHIC1)") {
		auto in = zenkit::Read::from("./samples/G1/FONT_OLD_10_WHITE_HI.FNT");
		zenkit::Font fnt {};
		fnt.load(in.get());

		verify_g1(fnt);
	}

	TEST_CASE("Font.save(GOTHIC1)") {
		auto r = zenkit::Read::from("./samples/G1/FONT_OLD_10_WHITE_HI.FNT");
		zenkit::Font fnt_tmp {};
		fnt_tmp.load(r.get());

		std::vector<std::byte> data;
		auto w = zenkit::Write::to(&data);
		fnt_tmp.save(w.get());

		r = zenkit::Read::from(&data);
		zenkit::Font fnt {};
		fnt.load(r.get());

		verify_g1(fnt);
	}

	TEST_CASE("Font.load(GOTHIC2)") {
		auto in = zenkit::Read::from("./samples/G2/FONT_OLD_10_WHITE_HI.FNT");
		zenkit::Font fnt {};
		fnt.load(in.get());

		verify_g2(fnt);
	}

	TEST_CASE("Font.save(GOTHIC2)") {
		auto r = zenkit::Read::from("./samples/G2/FONT_OLD_10_WHITE_HI.FNT");
		zenkit::Font fnt_tmp {};
		fnt_tmp.load(r.get());

		std::vector<std::byte> data;
		auto w = zenkit::Write::to(&data);
		fnt_tmp.save(w.get());

		r = zenkit::Read::from(&data);
		zenkit::Font fnt {};
		fnt.load(r.get());

		verify_g2(fnt);
	}
}
