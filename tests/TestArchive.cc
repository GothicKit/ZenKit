// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>

#include <doctest/doctest.h>

TEST_SUITE("archive") {
	TEST_CASE("archive(open:ASCII)") {
		auto in = phoenix::buffer::mmap("./samples/ascii.zen");
		auto reader = phoenix::archive_reader::open(in);

		phoenix::archive_object obj;

		CHECK(reader->read_object_begin(obj));
		CHECK_FALSE(reader->read_object_begin(obj));

		CHECK_EQ(obj.object_name, "root_obj");
		CHECK_EQ(obj.class_name, "%");
		CHECK_EQ(obj.version, 21237);
		CHECK_EQ(obj.index, 1);

		CHECK_EQ(reader->read_string(), "a basic string");

		// This failure will skip the offending entry. I am not yet sure if this is the correct behavior
		// or if it should just revert to the beginning of the line to enable re-parsing it.
		REQUIRE_THROWS_AS(reader->read_string(), phoenix::parser_error);

		reader->skip_object(false);

		CHECK_EQ(reader->read_int(), 672);
		CHECK_EQ(reader->read_float(), 12.12421f);
		CHECK_EQ(reader->read_byte(), 255);
		CHECK_EQ(reader->read_word(), 32621);
		CHECK_EQ(reader->read_enum(), 6);
		CHECK(reader->read_bool());

		auto color = reader->read_color();
		CHECK_EQ(color.r, 1);
		CHECK_EQ(color.g, 2);
		CHECK_EQ(color.b, 3);
		CHECK_EQ(color.a, 255);

		auto vec3 = reader->read_vec3();
		CHECK_EQ(vec3.x, 50.0f);
		CHECK_EQ(vec3.y, 100.123f);
		CHECK_EQ(vec3.z, -150.0f);

		auto vec2 = reader->read_vec2();
		CHECK_EQ(vec2.x, 111.11f);
		CHECK_EQ(vec2.y, -12.99f);

		auto box0 = reader->read_bbox();
		CHECK_EQ(box0.min.x, 12.0f);
		CHECK_EQ(box0.min.y, 34.0f);
		CHECK_EQ(box0.min.z, 56.0f);
		CHECK_EQ(box0.max.x, 78.0f);
		CHECK_EQ(box0.max.y, 89.0f);
		CHECK_EQ(box0.max.z, 0.0f);

		auto mat3 = reader->read_mat3x3();
		CHECK_EQ(mat3[0][0], 0.994702816f);
		CHECK_EQ(mat3[1][0], 0.0f);
		CHECK_EQ(mat3[2][0], 0.102792539f);
		CHECK_EQ(mat3[0][1], 0.0f);
		CHECK_EQ(mat3[1][1], 1.0f);
		CHECK_EQ(mat3[2][1], 0.0f);
		CHECK_EQ(mat3[0][2], -0.102792539f);
		CHECK_EQ(mat3[1][2], 0.0f);
		CHECK_EQ(mat3[2][2], 0.994702816f);

		auto raw = reader->read_raw_bytes(4);
		CHECK_EQ(raw.get(), 0xf2);
		CHECK_EQ(raw.get(), 0x42);
		CHECK_EQ(raw.get(), 0xa7);
		CHECK_EQ(raw.get(), 0x10);

		CHECK(reader->read_object_begin(obj));
		CHECK_FALSE(reader->read_object_begin(obj));

		CHECK_EQ(obj.object_name, "child_obj");
		CHECK_EQ(obj.class_name, "test:class:name");
		CHECK_EQ(obj.version, 7323);
		CHECK_EQ(obj.index, 2);

		CHECK(reader->read_object_end());
		CHECK(reader->read_object_end());
		REQUIRE_THROWS_AS(reader->read_float(), phoenix::parser_error);
	}

	TEST_CASE("archive(open:BINARY)") {
		auto in = phoenix::buffer::mmap("./samples/binary.zen");
		auto reader = phoenix::archive_reader::open(in);
		CHECK_EQ(reader->read_string(), "DT_BOOKSHELF_V1_1");

		phoenix::archive_object obj;
		CHECK(reader->read_object_begin(obj));

		CHECK_EQ(obj.object_name, "%");
		CHECK_EQ(obj.class_name, "zCMaterial");
		CHECK_EQ(obj.version, 17408);
		CHECK_EQ(obj.index, 0);

		CHECK_EQ(reader->read_string(), "DT_BOOKSHELF_V1_1");
		CHECK_EQ(reader->read_byte(), 0);

		auto color = reader->read_color();
		CHECK_EQ(color.r, 0x2A);
		CHECK_EQ(color.g, 0x23);
		CHECK_EQ(color.b, 0x19);
		CHECK_EQ(color.a, 0xFF);

		CHECK_EQ(reader->read_float(), 60.0f);
		CHECK_EQ(reader->read_string(), "MODIBOOKS01.TGA");
		CHECK_EQ(reader->read_string(), "256 256");
		CHECK_EQ(reader->read_float(), 0.0f);
	}

	TEST_CASE("archive(open:BIN_SAFE)" * doctest::skip()) {
		// FIXME: Stub
	}
}
