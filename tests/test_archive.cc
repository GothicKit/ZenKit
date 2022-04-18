// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>
#include <phoenix/detail/error.hh>

#include <doctest/doctest.h>

TEST_SUITE("archive") {
	TEST_CASE("ascii archives are read correctly") {
		auto in = phoenix::buffer::open("./samples/ascii.zen");
		auto reader = phoenix::archive_reader::open(in);

		phoenix::archive_object obj;

		CHECK(reader->read_object_begin(obj));
		CHECK(!reader->read_object_begin(obj));

		CHECK(obj.object_name == "root_obj");
		CHECK(obj.class_name == "%");
		CHECK(obj.version == 21237);
		CHECK(obj.index == 1);

		CHECK(reader->read_string() == "a basic string");

		// This failure will skip the offending entry. I am not yet sure if this is the correct behavior
		// or if it should just revert to the beginning of the line to enable re-parsing it.
		REQUIRE_THROWS_AS(reader->read_string(), phoenix::parser_error);

		reader->skip_object(false);

		CHECK(reader->read_int() == 672);
		CHECK(reader->read_float() == 12.12421f);
		CHECK(reader->read_byte() == 255);
		CHECK(reader->read_word() == 32621);
		CHECK(reader->read_enum() == 6);
		CHECK(reader->read_bool());

		auto color = reader->read_color();
		CHECK(color.r == 255);
		CHECK(color.g == 1);
		CHECK(color.b == 2);
		CHECK(color.a == 3);

		auto vec3 = reader->read_vec3();
		CHECK(vec3.x == 50);
		CHECK(vec3.y == 100.123f);
		CHECK(vec3.z == -150);

		auto vec2 = reader->read_vec2();
		CHECK(vec2.x == 111.11f);
		CHECK(vec2.y == -12.99f);

		auto box0 = reader->read_bbox();
		CHECK(box0.min.x == 12.0f);
		CHECK(box0.min.y == 34.0f);
		CHECK(box0.min.z == 56.0f);
		CHECK(box0.max.x == 78.0f);
		CHECK(box0.max.y == 89.0f);
		CHECK(box0.max.z == 0.0f);

		auto mat3 = reader->read_mat3x3();
		CHECK(mat3[0][0] == 0.994702816f);
		CHECK(mat3[1][0] == 0.0f);
		CHECK(mat3[2][0] == 0.102792539f);
		CHECK(mat3[0][1] == 0.0f);
		CHECK(mat3[1][1] == 1.0f);
		CHECK(mat3[2][1] == 0.0f);
		CHECK(mat3[0][2] == -0.102792539f);
		CHECK(mat3[1][2] == 0.0f);
		CHECK(mat3[2][2] == 0.994702816f);

		auto raw = reader->read_raw_bytes();
		CHECK(raw.get() == 0xf2);
		CHECK(raw.get() == 0x42);
		CHECK(raw.get() == 0xa7);
		CHECK(raw.get() == 0x10);

		CHECK(reader->read_object_begin(obj));
		CHECK(!reader->read_object_begin(obj));

		CHECK(obj.object_name == "child_obj");
		CHECK(obj.class_name == "test:class:name");
		CHECK(obj.version == 7323);
		CHECK(obj.index == 2);

		CHECK(reader->read_object_end());
		CHECK(reader->read_object_end());
		REQUIRE_THROWS_AS(reader->read_float(), phoenix::buffer_underflow_error);
	}

	TEST_CASE("binary archives are read correctly") {
		auto in = phoenix::buffer::open("./samples/binary.zen");
		auto reader = phoenix::archive_reader::open(in);
		CHECK(reader->read_string() == "DT_BOOKSHELF_V1_1");

		phoenix::archive_object obj;
		CHECK(reader->read_object_begin(obj));

		CHECK(obj.object_name == "%");
		CHECK(obj.class_name == "zCMaterial");
		CHECK(obj.version == 17408);
		CHECK(obj.index == 0);

		CHECK(reader->read_string() == "DT_BOOKSHELF_V1_1");
		CHECK(reader->read_byte() == 0);

		auto color = reader->read_color();
		CHECK(color.r == 0x19);
		CHECK(color.g == 0x23);
		CHECK(color.b == 0x2A);
		CHECK(color.a == 0xFF);

		CHECK(reader->read_float() == 60.0f);
		CHECK(reader->read_string() == "MODIBOOKS01.TGA");
		CHECK(reader->read_string() == "256 256");
		CHECK(reader->read_float() == 0.0f);
	}

	TEST_CASE("binsafe archives are read correctly") {
		// FIXME: Stub
	}
}
