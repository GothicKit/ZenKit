// Copyright 2021 Luis Michaelis
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
// Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <phoenix/archive.hh>
#include <phoenix/detail/error.hh>

#include "catch2.hh"

TEST_CASE("ascii archives are read correctly", "[archive]") {
	auto reader = phoenix::archive_reader::open(phoenix::reader::from("./samples/ascii.zen"));

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
	auto reader = phoenix::archive_reader::open(phoenix::reader::from("./samples/binary.zen"));
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