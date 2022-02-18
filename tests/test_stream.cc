// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/detail/stream.hh>

#include "doctest/doctest.h"

#include <fstream>
#include <sstream>

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

// TODO: Add tests for vector_reader!

TEST_SUITE("stream") {
	TEST_CASE("reader fails to open illegal paths") {
		CHECK_THROWS_AS(reader::from("."), std::system_error);
		CHECK_THROWS_AS(reader::from("./nonexistent"), std::system_error);
	}

	TEST_CASE("reader reads binary files correctly") {
		auto in = reader::from("./samples/basic.bin");
		CHECK(in.size() == 76);
		CHECK(in.tell() == 0);

		CHECK(in.read_s8() == -8);
		CHECK(in.read_s16() == -16);
		CHECK(in.read_s32() == -32);
		CHECK(in.read_s64() == -64);
		CHECK(in.read_u8() == 8);
		CHECK(in.read_u16() == 16);
		CHECK(in.read_u32() == 32);
		CHECK(in.read_u64() == 64);
		CHECK(in.read_f32() == -32.42f);
		CHECK(in.read_f64() == -64.42069);

		auto strings = in.tell();

		CHECK(in.read_line() == "line1");
		CHECK(in.read_line() == "line2");

		in.seek(strings);

		CHECK(in.read_line(false) == "line1");
		CHECK(in.read_line() == "  ");
		CHECK(in.read_line() == "line2");
		CHECK(in.read_string(19) == "arbitrarylongstring");

		CHECK_THROWS_AS((void) in.read_u8(), io_error);
		CHECK_THROWS_AS(in.seek(in.size() + 10), io_error);
	}

	TEST_CASE("readers can be forked") {
		auto in = reader::from("./samples/basic.bin");
		CHECK(in.size() == 76);

		auto fork = in.fork(10);
		CHECK(in.tell() == 10);
		CHECK(fork.size() == 10);
		CHECK(fork.tell() == 0);

		CHECK_THROWS_AS((void) in.fork(100), io_error);
		CHECK_THROWS_AS((void) in.fork(100, 10), io_error);

		auto fork2 = in.fork(50, 10);
		CHECK(in.tell() == 10);
		CHECK(fork2.size() == 50);
		CHECK(fork2.tell() == 0);
	}

	TEST_CASE("writer writes binary files correctly") {
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

		CHECK(out.tell() == 76);
		CHECK(ss.str() == read_file("./samples/basic.bin"));
	}
}
