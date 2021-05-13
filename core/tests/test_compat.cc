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
#include <phoenix/detail/compat.hh>
#include "catch2.hh"

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