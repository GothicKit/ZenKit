// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/detail/compat.hh>

TEST_CASE("time encodings are converted correctly", "[compat][time]") {
	auto unix_now = time(nullptr);

	// We need to make sure to use an even number of seconds
	// because DOS time is weird

	while (unix_now % 2 != 0) {
		unix_now = time(nullptr);
	}

	auto dos_now = phoenix::unix_time_to_dos(unix_now);
	auto unix_now_from_dos = phoenix::dos_to_unix_time(dos_now);

	REQUIRE(unix_now == unix_now_from_dos);
}

TEST_CASE("case-insensitive strings are compared correctly", "[compat][iequals]") {
	REQUIRE(phoenix::iequals("test\nstring", "test\nstring"));
	REQUIRE(phoenix::iequals("test\nSTRING", "test\nstring"));
	REQUIRE(phoenix::iequals("TeSt\nStRiNg", "test\nstring"));
	REQUIRE(!phoenix::iequals("TeStStRiNg", "test\nstring"));
	REQUIRE(phoenix::iequals("test\nstring", "test\nstring"));
	REQUIRE(phoenix::iequals("test\nstring", "test\nSTRING"));
	REQUIRE(phoenix::iequals("test\nstring", "TeSt\nStRiNg"));
	REQUIRE(!phoenix::iequals("test\nstring", "TeStStRiNg"));
}