// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/texture.hh>
#include "catch2.hh"

using namespace phoenix;

TEST_CASE("textures are read correctly", "[texture]") {
	auto in = reader::from("./samples/erz.tex");
	auto texture = texture::read(in);

	REQUIRE(texture.height() == 128);
	REQUIRE(texture.width() == 128);
	REQUIRE(texture.format() == tex_R8G8B8A8);

	// we only test mipmap 0 (full size) here
	const auto& data = texture.as_rgba8(0);
	auto expected = reader::from("./samples/erz.expected.0.bin");

	for (u32 i = 0; i < data.size(); i += 4) {
		u8 r = data[i];
		u8 g = data[i + 1];
		u8 b = data[i + 2];
		u8 a = data[i + 3];

		REQUIRE(r == expected.read_u8());
		REQUIRE(g == expected.read_u8());
		REQUIRE(b == expected.read_u8());
		REQUIRE(a == expected.read_u8());
	}
}

// TODO: Check other formats as well (DXT3, DXT5, ...)