// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "doctest/doctest.h"
#include <phoenix/texture.hh>

using namespace phoenix;

TEST_SUITE("texture") {
	TEST_CASE("textures are read correctly") {
		auto in = reader::from("./samples/erz.tex");
		auto texture = texture::parse(in);

		CHECK(texture.height() == 128);
		CHECK(texture.width() == 128);

		CHECK(texture.mipmap_width(0) == 128);
		CHECK(texture.mipmap_height(0) == 128);
		CHECK(texture.mipmap_width(1) == 64);
		CHECK(texture.mipmap_height(1) == 64);
		CHECK(texture.mipmap_width(2) == 32);
		CHECK(texture.mipmap_height(2) == 32);

		CHECK(texture.ref_height() == 128);
		CHECK(texture.ref_width() == 128);

		CHECK(texture.mipmaps() == 5);
		CHECK(texture.format() == tex_R8G8B8A8);

		// we only test mipmap 0 (full size) here
		const auto& data = texture.as_rgba8(0);
		auto expected = reader::from("./samples/erz.expected.0.bin");

		for (u32 i = 0; i < data.size(); i += 4) {
			u8 r = data[i];
			u8 g = data[i + 1];
			u8 b = data[i + 2];
			u8 a = data[i + 3];

			CHECK(r == expected.read_u8());
			CHECK(g == expected.read_u8());
			CHECK(b == expected.read_u8());
			CHECK(a == expected.read_u8());
		}
	}
}

// TODO: Check other formats as well (DXT3, DXT5, ...)
