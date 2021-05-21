// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/detail/stream.hh>

#include "catch2.hh"

#include <sstream>
#include <fstream>

using namespace phoenix;

static std::string read_file(const std::string& str) {
	std::string v {};
	std::ifstream in {str, std::ios::binary | std::ios::ate};

	v.resize(in.tellg());
	in.seekg(0);
	in.read(reinterpret_cast<char*>(v.data()), static_cast<s32>(v.size()));
	in.close();

	return v;
}

TEST_CASE("reader fails to open illegal paths", "[stream][reader]") {
	REQUIRE_THROWS_AS(reader::from("."), io_error);
	REQUIRE_THROWS_AS(reader::from("./nonexistent"), io_error);
}

TEST_CASE("reader reads binary files correctly", "[stream][reader]") {
	auto in = reader::from("./samples/basic.bin");
	REQUIRE(in.size() == 76);
	REQUIRE(in.tell() == 0);

	REQUIRE(in.read_s8() == -8);
	REQUIRE(in.read_s16() == -16);
	REQUIRE(in.read_s32() == -32);
	REQUIRE(in.read_s64() == -64);
	REQUIRE(in.read_u8() == 8);
	REQUIRE(in.read_u16() == 16);
	REQUIRE(in.read_u32() == 32);
	REQUIRE(in.read_u64() == 64);
	REQUIRE(in.read_f32() == -32.42f);
	REQUIRE(in.read_f64() == -64.42069);

	auto strings = in.tell();

	REQUIRE(in.read_line() == "line1");
	REQUIRE(in.read_line() == "line2");

	in.seek(strings);

	REQUIRE(in.read_line(false) == "line1");
	REQUIRE(in.read_line() == "  ");
	REQUIRE(in.read_line() == "line2");
	REQUIRE(in.read_string(19) == "arbitrarylongstring");

	REQUIRE_THROWS_AS(in.read_u8(), io_error);
	REQUIRE_THROWS_AS(in.seek(in.size() + 10), io_error);
}

TEST_CASE("readers can be forked", "[stream][reader]") {
	auto in = reader::from("./samples/basic.bin");
	REQUIRE(in.size() == 76);

	auto fork = in.fork(10);
	REQUIRE(in.tell() == 10);
	REQUIRE(fork.size() == 10);
	REQUIRE(fork.tell() == 0);

	REQUIRE_THROWS_AS(in.fork(100), io_error);
	REQUIRE_THROWS_AS(in.fork(100, 10), io_error);

	auto fork2 = in.fork(50, 10);
	REQUIRE(in.tell() == 10);
	REQUIRE(fork2.size() == 50);
	REQUIRE(fork2.tell() == 0);
}

TEST_CASE("writer writes binary files correctly", "[stream][writer]") {
	std::ostringstream ss {};
	phoenix::writer out {ss};

	out.write_s8(-8);
	out.write_s16(-16);
	out.write_s32(-32);
	out.write_s64(-64);
	out.write_u8(8);
	out.write_u16(16);
	out.write_u32(32);
	out.write_u64(64);
	out.write_f32(-32.42f);
	out.write_f64(-64.42069);

	out.write_line("line1");
	out.write_string("  \n");
	out.write_line("line2");
	out.write_string("arbitrarylongstring");

	REQUIRE(out.tell() == 76);
	REQUIRE(ss.str() == read_file("./samples/basic.bin"));
}