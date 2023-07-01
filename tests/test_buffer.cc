// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/buffer.hh>

#include <array>
#include <cstring>

template <typename... Args>
static std::vector<std::byte> bytes(Args... bytes) {
	return std::vector<std::byte> {static_cast<std::byte>(bytes)...};
}

static std::vector<std::byte> bytes_str(const char* str) {
	std::vector<std::byte> v {};
	v.resize(std::strlen(str));
	std::memcpy(v.data(), str, v.size());
	return v;
}

TEST_SUITE("buffer") {
	TEST_CASE("buffer(of)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c'));
		CHECK_EQ(buf.limit(), 3);
		CHECK_EQ(buf.position(), 0);
		CHECK_EQ(buf.remaining(), 3);
		CHECK_EQ(buf.capacity(), 3);
		CHECK_EQ(buf.direct(), false);
		CHECK_EQ(buf.readonly(), true);
	}

	TEST_CASE("buffer(limit)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c'));

		SUBCASE("new_limit > capacity") {
			CHECK_THROWS(buf.limit(4));
			CHECK_EQ(buf.limit(), 3);
			CHECK_EQ(buf.capacity(), 3);
			CHECK_EQ(buf.remaining(), 3);
			CHECK_EQ(buf.position(), 0);
		}

		SUBCASE("new_limit < capacity") {
			// limit the buffer to one element
			buf.limit(1);
			CHECK_EQ(buf.limit(), 1);
			CHECK_EQ(buf.capacity(), 3);
			CHECK_EQ(buf.remaining(), 1);
			CHECK_EQ(buf.position(), 0);
		}

		SUBCASE("new_limit, capacity") {
			// un-limit the buffer to its capacity again
			buf.limit(3);
			CHECK_EQ(buf.limit(), 3);
			CHECK_EQ(buf.capacity(), 3);
			CHECK_EQ(buf.remaining(), 3);
			CHECK_EQ(buf.position(), 0);
		}

		SUBCASE("position is restored") {
			buf.position(3);
			buf.limit(1);
			CHECK_EQ(buf.limit(), 1);
			CHECK_EQ(buf.capacity(), 3);
			CHECK_EQ(buf.remaining(), 0);
			CHECK_EQ(buf.position(), 1);
		}
	}

	TEST_CASE("buffer(position)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c'));
		CHECK_EQ(buf.position(), 0);
		CHECK_EQ(buf.remaining(), 3);

		buf.position(1);
		CHECK_EQ(buf.position(), 1);
		CHECK_EQ(buf.remaining(), 2);

		CHECK_THROWS(buf.position(4));
		CHECK_EQ(buf.position(), 1);
		CHECK_EQ(buf.remaining(), 2);

		buf.position(3);
		CHECK_EQ(buf.position(), 3);
		CHECK_EQ(buf.remaining(), 0);

		buf.rewind();
		CHECK_EQ(buf.position(), 0);
		CHECK_EQ(buf.remaining(), 3);
	}

	TEST_CASE("buffer(clear)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c'));
		buf.position(3);
		buf.limit(2);

		CHECK_EQ(buf.limit(), 2);
		CHECK_EQ(buf.capacity(), 3);
		CHECK_EQ(buf.remaining(), 0);
		CHECK_EQ(buf.position(), 2);

		buf.clear();
		CHECK_EQ(buf.limit(), 3);
		CHECK_EQ(buf.capacity(), 3);
		CHECK_EQ(buf.remaining(), 3);
		CHECK_EQ(buf.position(), 0);
	}

	TEST_CASE("buffer(duplicate)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c'));
		buf.position(1);
		buf.limit(2);

		auto dup = buf.duplicate();
		CHECK_EQ(buf.position(), dup.position());
		CHECK_EQ(buf.capacity(), dup.capacity());
		CHECK_EQ(buf.remaining(), dup.remaining());
		CHECK_EQ(buf.limit(), dup.limit());
	}

	TEST_CASE("buffer(flip)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c'));
		buf.position(1);
		buf.flip();

		CHECK_EQ(buf.limit(), 1);
		CHECK_EQ(buf.capacity(), 3);
		CHECK_EQ(buf.remaining(), 1);
		CHECK_EQ(buf.position(), 0);
	}

	TEST_CASE("buffer(slice)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c', 'd'));

		SUBCASE("fully") {
			buf.position(2);

			auto slice = buf.slice();
			CHECK_EQ(slice.position(), 0);
			CHECK_EQ(slice.capacity(), 2);
			CHECK_EQ(slice.limit(), 2);
			CHECK_EQ(slice.remaining(), 2);

			CHECK_EQ(buf.position(), 2);
		}

		SUBCASE("partially") {
			auto slice = buf.slice(1, 2);
			CHECK_EQ(slice.position(), 0);
			CHECK_EQ(slice.capacity(), 2);
			CHECK_EQ(slice.limit(), 2);
			CHECK_EQ(slice.remaining(), 2);

			CHECK_EQ(buf.position(), 0);
		}
	}

	TEST_CASE("buffer(mark)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'));

		SUBCASE("basic") {
			buf.position(1);
			buf.mark();
			buf.position(5);
			CHECK_EQ(buf.position(), 5);
			CHECK_EQ(buf.capacity(), 8);
			CHECK_EQ(buf.limit(), 8);
			CHECK_EQ(buf.remaining(), 3);

			buf.reset();
			CHECK_EQ(buf.position(), 1);
			CHECK_EQ(buf.capacity(), 8);
			CHECK_EQ(buf.limit(), 8);
			CHECK_EQ(buf.remaining(), 7);
		}

		SUBCASE("underflow") {
			buf.position(3);
			buf.mark();
			buf.position(5);
			CHECK_EQ(buf.position(), 5);
			CHECK_EQ(buf.capacity(), 8);
			CHECK_EQ(buf.limit(), 8);
			CHECK_EQ(buf.remaining(), 3);

			buf.limit(2);
			buf.reset();
			CHECK_EQ(buf.position(), 2);
			CHECK_EQ(buf.capacity(), 8);
			CHECK_EQ(buf.limit(), 2);
			CHECK_EQ(buf.remaining(), 0);
		}

		SUBCASE("positioned") {
			buf.position(3);
			buf.mark();
			buf.position(2);
			CHECK_EQ(buf.position(), 2);
			CHECK_EQ(buf.capacity(), 8);
			CHECK_EQ(buf.limit(), 8);
			CHECK_EQ(buf.remaining(), 6);

			buf.reset();
			CHECK_EQ(buf.position(), 2);
			CHECK_EQ(buf.capacity(), 8);
			CHECK_EQ(buf.limit(), 8);
			CHECK_EQ(buf.remaining(), 6);
		}

		SUBCASE("sliced") {
			buf.position(1);
			buf.mark();
			buf.position(5);
			CHECK_EQ(buf.position(), 5);
			CHECK_EQ(buf.capacity(), 8);
			CHECK_EQ(buf.limit(), 8);
			CHECK_EQ(buf.remaining(), 3);

			auto slice = buf.slice();
			slice.reset();

			CHECK_EQ(slice.position(), 0);
			CHECK_EQ(slice.capacity(), 3);
			CHECK_EQ(slice.limit(), 3);
			CHECK_EQ(slice.remaining(), 3);
		}
	}

	TEST_CASE("buffer(extract)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c', 'd'));

		auto slice = buf.extract(2);
		CHECK_EQ(slice.limit(), 2);
		CHECK_EQ(slice.position(), 0);
		CHECK_EQ(buf.position(), 2);
		CHECK_EQ(buf.remaining(), 2);

		CHECK_EQ(slice.get_char(), 'a');
		CHECK_EQ(buf.get_char(), 'c');
	}

	TEST_CASE("buffer(array)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b', 'c', 'd'));
		buf.limit(3);

		auto array = buf.array();
		CHECK_EQ((char) *array, 'a');
		CHECK(std::equal(array, array + buf.limit(), bytes_str("abc").begin()));
	}

	TEST_CASE("buffer(get)") {
		auto buf = phoenix::buffer::of(bytes('\x1A', 0xA1, 'c', 'd'));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get(), 0x1A);
		CHECK_EQ(buf.position(), 1);

		CHECK_EQ(buf.get(), 0xA1);
		CHECK_EQ(buf.position(), 2);

		std::array<std::byte, 2> array {};
		buf.get(array.data(), array.size());

		CHECK_EQ(buf.position(), 4);
		CHECK_EQ(buf.remaining(), 0);
		CHECK(std::equal(array.begin(), array.end(), bytes_str("cd").begin()));

		CHECK_THROWS((void) buf.get());
		CHECK_THROWS(buf.get(array.data(), array.size()));
	}

	TEST_CASE("buffer(get_char)") {
		auto buf = phoenix::buffer::of(bytes('a', 'b'));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_char(), 'a');
		CHECK_EQ(buf.position(), 1);

		CHECK_EQ(buf.get_char(), 'b');
		CHECK_EQ(buf.position(), 2);

		CHECK_THROWS((void) buf.get_char());
	}

	TEST_CASE("buffer(get_short)") {
		auto buf = phoenix::buffer::of(bytes(0xFF, 0xFF, 0x01, 0x00, 0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_short(), -1);
		CHECK_EQ(buf.position(), 2);

		CHECK_EQ(buf.get_short(), 1);
		CHECK_EQ(buf.position(), 4);

		CHECK_THROWS((void) buf.get_short());
	}

	TEST_CASE("buffer(get_ushort)") {
		auto buf = phoenix::buffer::of(bytes(0xFF, 0xFF, 0x01, 0x00, 0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_ushort(), 0xFF'FF);
		CHECK_EQ(buf.position(), 2);

		CHECK_EQ(buf.get_ushort(), 1);
		CHECK_EQ(buf.position(), 4);

		CHECK_THROWS((void) buf.get_short());
	}

	TEST_CASE("buffer(get_int)") {
		auto buf = phoenix::buffer::of(bytes(0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_int(), -1);
		CHECK_EQ(buf.position(), 4);

		CHECK_EQ(buf.get_int(), 1);
		CHECK_EQ(buf.position(), 8);

		CHECK_THROWS((void) buf.get_int());
	}

	TEST_CASE("buffer(get_uint)") {
		auto buf = phoenix::buffer::of(bytes(0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_uint(), 0xFFFF'FFFF);
		CHECK_EQ(buf.position(), 4);

		CHECK_EQ(buf.get_uint(), 1);
		CHECK_EQ(buf.position(), 8);

		CHECK_THROWS((void) buf.get_uint());
	}

	TEST_CASE("buffer(get_long)") {
		auto buf = phoenix::buffer::of(bytes(0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0x01,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_long(), -1);
		CHECK_EQ(buf.position(), 8);

		CHECK_EQ(buf.get_long(), 1);
		CHECK_EQ(buf.position(), 16);

		CHECK_THROWS((void) buf.get_long());
	}

	TEST_CASE("buffer(get_ulong)") {
		auto buf = phoenix::buffer::of(bytes(0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0x01,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0x00,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_ulong(), 0xFFFFFFFF'FFFFFFFFL);
		CHECK_EQ(buf.position(), 8);

		CHECK_EQ(buf.get_ulong(), 1);
		CHECK_EQ(buf.position(), 16);

		CHECK_THROWS((void) buf.get_ulong());
	}

	TEST_CASE("buffer(get_float)") {

		auto buf = phoenix::buffer::of(bytes(0x52, 0x58, 0xD2, 0x43, 0x0A, 0xD7, 0x8A, 0xC2, 0xFF, 0xFF, 0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_float(), 420.69f);
		CHECK_EQ(buf.position(), 4);

		CHECK_EQ(buf.get_float(), -69.420f);
		CHECK_EQ(buf.position(), 8);

		CHECK_THROWS((void) buf.get_float());
	}

	TEST_CASE("buffer(get_double)") {
		auto buf = phoenix::buffer::of(bytes(0xD7,
		                                     0xA3,
		                                     0x70,
		                                     0x3D,
		                                     0x0A,
		                                     0x4B,
		                                     0x7A,
		                                     0x40,
		                                     0x7B,
		                                     0x14,
		                                     0xAE,
		                                     0x47,
		                                     0xE1,
		                                     0x5A,
		                                     0x51,
		                                     0xC0,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF,
		                                     0xFF));
		CHECK_EQ(buf.position(), 0);

		CHECK_EQ(buf.get_double(), 420.69);
		CHECK_EQ(buf.position(), 8);

		CHECK_EQ(buf.get_double(), -69.420);
		CHECK_EQ(buf.position(), 16);

		CHECK_THROWS((void) buf.get_double());
	}

	TEST_CASE("buffer(get_string)") {
		auto buf =
		    phoenix::buffer::of(bytes('H', 'i', 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'));

		CHECK_EQ(buf.get_string(2), "Hi");
		CHECK_EQ(buf.position(), 2);

		CHECK_EQ(buf.get_string(13), "Hello, World!");
		CHECK_EQ(buf.position(), 15);

		CHECK_THROWS((void) buf.get_string(1));
	}

	TEST_CASE("buffer(get_line)") {
		auto buf = phoenix::buffer::of(bytes('H',
		                                     'i',
		                                     '\n',
		                                     ' ',
		                                     ' ',
		                                     '\r',
		                                     '\t',
		                                     'H',
		                                     'e',
		                                     'l',
		                                     'l',
		                                     'o',
		                                     ',',
		                                     '\\',
		                                     't',
		                                     'W',
		                                     'o',
		                                     'r',
		                                     'l',
		                                     'd',
		                                     '!',
		                                     '\n'));

		CHECK_EQ(buf.get_line(true), "Hi");
		CHECK_EQ(buf.position(), 7);
		buf.mark();

		CHECK_EQ(buf.get_line(true), "Hello,\\tWorld!");
		CHECK_EQ(buf.position(), 22);

		buf.reset();
		CHECK_EQ(buf.get_line_escaped(true), "Hello,\tWorld!");
		CHECK_EQ(buf.position(), 22);

		CHECK_THROWS((void) buf.get_line());
	}

	TEST_CASE("buffer(get_vec2)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("buffer(get_vec3)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("buffer(get_vec4)" * doctest::skip()) {
		// TODO: Stub
	}

	TEST_CASE("buffer(put*)") {
		auto buf = phoenix::buffer::allocate(57);

		buf.put(0xFF);
		buf.put_short(-16);
		buf.put_ushort(16);
		buf.put_int(-16);
		buf.put_uint(16);
		buf.put_long(-16);
		buf.put_ulong(16);
		buf.put_float(69.420f);
		buf.put_double(420.69);
		buf.put_string("Hi");
		buf.put_line("Hello, World!");
		buf.flip();

		CHECK_EQ(buf.limit(), 57);
		CHECK_EQ(buf.get(), 0xFF);
		CHECK_EQ(buf.get_short(), -16);
		CHECK_EQ(buf.get_ushort(), 16);
		CHECK_EQ(buf.get_int(), -16);
		CHECK_EQ(buf.get_uint(), 16);
		CHECK_EQ(buf.get_long(), -16);
		CHECK_EQ(buf.get_ulong(), 16);
		CHECK_EQ(buf.get_float(), 69.420f);
		CHECK_EQ(buf.get_double(), 420.69);
		CHECK_EQ(buf.get_string(2), "Hi");
		CHECK_EQ(buf.get_line(), "Hello, World!");

		CHECK_EQ(buf.remaining(), 0);
	}

	TEST_CASE("buffer(empty)") {
		auto empty = phoenix::buffer::empty();
		CHECK_EQ(empty.limit(), 0);
		CHECK_EQ(empty.capacity(), 0);
		CHECK_EQ(empty.remaining(), 0);
		CHECK_EQ(empty.position(), 0);

		auto buf = phoenix::buffer::allocate(10);
		CHECK_FALSE(empty == buf);
	}

	TEST_CASE("buffer(mmap)") {
		SUBCASE("empty file") {
			auto buf = phoenix::buffer::mmap("samples/empty.txt");
			CHECK_EQ(buf.limit(), 0);
			CHECK_EQ(buf.capacity(), 0);
			CHECK_THROWS_AS((void) buf.get_char(), phoenix::buffer_underflow);
		}
	}
}
