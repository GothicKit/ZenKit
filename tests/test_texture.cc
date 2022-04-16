// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "doctest/doctest.h"
#include <phoenix/texture.hh>

using namespace phoenix;

TEST_SUITE("texture") {
	TEST_CASE("textures are read correctly") {
		auto in = buffer::open("./samples/erz.tex");
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
		auto expected = buffer::open("./samples/erz.expected.0.bin");

		for (std::uint32_t i = 0; i < data.size(); i += 4) {
			std::uint8_t r = data[i];
			std::uint8_t g = data[i + 1];
			std::uint8_t b = data[i + 2];
			std::uint8_t a = data[i + 3];

			CHECK(r == expected.get());
			CHECK(g == expected.get());
			CHECK(b == expected.get());
			CHECK(a == expected.get());
		}
	}
}

// TODO: Check other formats as well (DXT3, DXT5, ...)
