// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/texture.hh>

using namespace phoenix;

TEST_SUITE("texture") {
	TEST_CASE("textures are read correctly") {
		auto in = buffer::mmap("./samples/erz.tex");
		auto texture = texture::parse(in);

		CHECK(texture.height() == 128);
		CHECK(texture.width() == 128);

		CHECK(texture.mipmap_width(0) == 128);
		CHECK(texture.mipmap_height(0) == 128);
		CHECK(texture.mipmap_width(1) == 64);
		CHECK(texture.mipmap_height(1) == 64);
		CHECK(texture.mipmap_width(2) == 32);
		CHECK(texture.mipmap_height(2) == 32);
		CHECK(texture.average_color() == 0xff443a3c);

		CHECK(texture.ref_height() == 128);
		CHECK(texture.ref_width() == 128);

		CHECK(texture.mipmaps() == 5);
		CHECK(texture.format() == phoenix::tex_dxt1);
	}
}

// TODO: Check other formats as well (DXT3, DXT5, ...)
