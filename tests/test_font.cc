// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/font.hh>

#include "doctest/doctest.h"

using namespace phoenix;

TEST_SUITE("font") {
	TEST_CASE("fonts are read correctly") {
		auto in = buffer::open("./samples/font.fnt");
		auto fnt = font::parse(in);

		CHECK(fnt.name() == "FONT_OLD_10_WHITE_HI.TGA");
		CHECK(fnt.height() == 17);

		// We'll look at the three glyphs in the middle
		auto middle = (fnt.glyphs().size() / 2) - 1;

		auto f1 = fnt.glyphs()[middle - 1];
		auto f2 = fnt.glyphs()[middle];
		auto f3 = fnt.glyphs()[middle + 1];

		CHECK((std::uint16_t) f1.width == 9);
		CHECK((std::uint16_t) f2.width == 8);
		CHECK((std::uint16_t) f3.width == 0);

		CHECK(f1.uv[0] == glm::vec2 {0.36132812f, 0.23828125f});
		CHECK(f2.uv[0] == glm::vec2 {0.3984375f, 0.23828125f});
		CHECK(f3.uv[0] == glm::vec2 {-0.001953125f, 0.23828125f});

		CHECK(f1.uv[1] == glm::vec2 {0.37695312f, 0.30859375f});
		CHECK(f2.uv[1] == glm::vec2 {0.41210938f, 0.30859375f});
		CHECK(f3.uv[1] == glm::vec2 {-0.00390625f, 0.30859375f});
	}
}
