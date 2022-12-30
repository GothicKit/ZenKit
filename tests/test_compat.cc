// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/phoenix.hh>
#include <phoenix/vdfs.hh>

TEST_SUITE("phoenix") {
	TEST_CASE("unix_time_to_dos" * doctest::skip()) {
		// TODO: This test is broken.
		auto unix_now = time(nullptr);

		// We need to make sure to use an even number of seconds
		// because DOS time is weird

		while (unix_now % 2 != 0) {
			unix_now = time(nullptr);
		}

		auto dos_now = phoenix::unix_time_to_dos(unix_now);
		auto unix_now_from_dos = phoenix::dos_to_unix_time(dos_now);

		CHECK_EQ(unix_now, unix_now_from_dos);
	}

	TEST_CASE("iequals") {
		CHECK(phoenix::iequals("test\nstring", "test\nstring"));
		CHECK(phoenix::iequals("test\nSTRING", "test\nstring"));
		CHECK(phoenix::iequals("TeSt\nStRiNg", "test\nstring"));
		CHECK_FALSE(phoenix::iequals("TeStStRiNg", "test\nstring"));
		CHECK(phoenix::iequals("test\nstring", "test\nstring"));
		CHECK(phoenix::iequals("test\nstring", "test\nSTRING"));
		CHECK(phoenix::iequals("test\nstring", "TeSt\nStRiNg"));
		CHECK_FALSE(phoenix::iequals("test\nstring", "TeStStRiNg"));
	}
}
