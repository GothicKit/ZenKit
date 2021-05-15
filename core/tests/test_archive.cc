// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/archive.hh>
#include <phoenix/detail/error.hh>

#include "catch2.hh"

TEST_CASE("ascii archives are read correctly", "[archive]") {
	auto in = phoenix::reader::from("./samples/ascii.zen");
	auto reader = phoenix::archive_reader::open(in);

	phoenix::archive_object obj;

	REQUIRE(reader->read_object_begin(obj));
	REQUIRE(!reader->read_object_begin(obj));

	REQUIRE(obj.object_name == "root_obj");
	REQUIRE(obj.class_name == "%");
	REQUIRE(obj.version == 21237);
	REQUIRE(obj.index == 1);

	REQUIRE(reader->read_string() == "a basic string");

	// This failure will skip the offending entry. I am not yet sure if this is the correct behavior
	// or if it should just revert to the beginning of the line to enable re-parsing it.
	REQUIRE_THROWS_AS(reader->read_string(), phoenix::parser_error);

	reader->skip_object();

	REQUIRE(reader->read_int() == 672);
	REQUIRE(reader->read_float() == 12.12421f);
	REQUIRE(reader->read_byte() == 255);
	REQUIRE(reader->read_word() == 32621);
	REQUIRE(reader->read_enum() == 6);
	REQUIRE(reader->read_bool());

	auto color = reader->read_color();
	REQUIRE(color.r == 255);
	REQUIRE(color.g == 1);
	REQUIRE(color.b == 2);
	REQUIRE(color.a == 3);

	auto vec3 = reader->read_vec3();
	REQUIRE(vec3.x == 50);
	REQUIRE(vec3.y == 100.123f);
	REQUIRE(vec3.z == -150);

	REQUIRE(reader->read_object_begin(obj));
	REQUIRE(!reader->read_object_begin(obj));

	REQUIRE(obj.object_name == "child_obj");
	REQUIRE(obj.class_name == "test:class:name");
	REQUIRE(obj.version == 7323);
	REQUIRE(obj.index == 2);

	REQUIRE(reader->read_object_end());
	REQUIRE(reader->read_object_end());
	REQUIRE_THROWS_AS(reader->read_float(), phoenix::io_error);
}

TEST_CASE("binary archives are read correctly", "[archive]") {
	auto in = phoenix::reader::from("./samples/binary.zen");
	auto reader = phoenix::archive_reader::open(in);
	REQUIRE(reader->read_string() == "DT_BOOKSHELF_V1_1");

	phoenix::archive_object obj;
	REQUIRE(reader->read_object_begin(obj));

	REQUIRE(obj.object_name == "%");
	REQUIRE(obj.class_name == "zCMaterial");
	REQUIRE(obj.version == 17408);
	REQUIRE(obj.index == 0);

	REQUIRE(reader->read_string() == "DT_BOOKSHELF_V1_1");
	REQUIRE(reader->read_byte() == 0);

	auto color = reader->read_color();
	REQUIRE(color.r == 0x19);
	REQUIRE(color.g == 0x23);
	REQUIRE(color.b == 0x2A);
	REQUIRE(color.a == 0xFF);

	REQUIRE(reader->read_float() == 60.0f);
	REQUIRE(reader->read_string() == "MODIBOOKS01.TGA");
	REQUIRE(reader->read_string() == "256 256");
	REQUIRE(reader->read_float() == 0.0f);
}

TEST_CASE("binsafe archives are read correctly", "[archive]") {
	// FIXME: Stub
}