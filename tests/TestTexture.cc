// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/Stream.hh>
#include <zenkit/Texture.hh>

TEST_SUITE("Texture") {
	TEST_CASE("Texture.load(GOTHIC?)") {
		auto in = zenkit::Read::from("./samples/erz.tex");
		zenkit::Texture texture {};
		texture.load(in.get());

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
		CHECK_EQ(texture.format(), zenkit::TextureFormat::DXT1);
	}

	TEST_CASE("Texture.load(GOTHIC1)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("Texture.load(GOTHIC2)" * doctest::skip()) {
		// TODO: Stub
	}
}

// TODO: Check other formats as well (DXT3, DXT5, ...)
