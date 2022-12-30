// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/texture.hh>

using namespace phoenix;

TEST_SUITE("texture") {
	TEST_CASE("texture(parse:?)") {
		auto in = buffer::mmap("./samples/erz.tex");
		auto texture = texture::parse(in);

		CHECK_EQ(texture.height(), 128);
		CHECK_EQ(texture.width(), 128);

		CHECK_EQ(texture.mipmap_width(0), 128);
		CHECK_EQ(texture.mipmap_height(0), 128);
		CHECK_EQ(texture.mipmap_width(1), 64);
		CHECK_EQ(texture.mipmap_height(1), 64);
		CHECK_EQ(texture.mipmap_width(2), 32);
		CHECK_EQ(texture.mipmap_height(2), 32);
		CHECK_EQ(texture.average_color(), 0xff443a3c);

		CHECK_EQ(texture.ref_height(), 128);
		CHECK_EQ(texture.ref_width(), 128);

		CHECK_EQ(texture.mipmaps(), 5);
		CHECK_EQ(texture.format(), phoenix::tex_dxt1);
	}

	TEST_CASE("texture(parse:g1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("texture(parse:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}

// TODO: Check other formats as well (DXT3, DXT5, ...)
