// Copyright © 2022 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <doctest/doctest.h>
#include <phoenix/detail/buffer.hh>

TEST_SUITE("buffer") {
	TEST_CASE("wrap()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c'});
		CHECK(buf.limit() == 3);
		CHECK(buf.position() == 0);
		CHECK(buf.remaining() == 3);
		CHECK(buf.capacity() == 3);
		CHECK(buf.direct() == false);
		CHECK(buf.readonly() == true);
	}

	TEST_CASE("limit()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c'});

		SUBCASE("new_limit > capacity") {
			CHECK_THROWS(buf.limit(4));
			CHECK(buf.limit() == 3);
			CHECK(buf.capacity() == 3);
			CHECK(buf.remaining() == 3);
			CHECK(buf.position() == 0);
		}

		SUBCASE("new_limit < capacity") {
			// limit the buffer to one element
			buf.limit(1);
			CHECK(buf.limit() == 1);
			CHECK(buf.capacity() == 3);
			CHECK(buf.remaining() == 1);
			CHECK(buf.position() == 0);
		}

		SUBCASE("new_limit == capacity") {
			// un-limit the buffer to its capacity again
			buf.limit(3);
			CHECK(buf.limit() == 3);
			CHECK(buf.capacity() == 3);
			CHECK(buf.remaining() == 3);
			CHECK(buf.position() == 0);
		}

		SUBCASE("position is restored") {
			buf.position(3);
			buf.limit(1);
			CHECK(buf.limit() == 1);
			CHECK(buf.capacity() == 3);
			CHECK(buf.remaining() == 0);
			CHECK(buf.position() == 1);
		}
	}

	TEST_CASE("position()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c'});
		CHECK(buf.position() == 0);
		CHECK(buf.remaining() == 3);

		buf.position(1);
		CHECK(buf.position() == 1);
		CHECK(buf.remaining() == 2);

		CHECK_THROWS(buf.position(4));
		CHECK(buf.position() == 1);
		CHECK(buf.remaining() == 2);

		buf.position(3);
		CHECK(buf.position() == 3);
		CHECK(buf.remaining() == 0);

		buf.rewind();
		CHECK(buf.position() == 0);
		CHECK(buf.remaining() == 3);
	}

	TEST_CASE("clear()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c'});
		buf.position(3);
		buf.limit(2);

		CHECK(buf.limit() == 2);
		CHECK(buf.capacity() == 3);
		CHECK(buf.remaining() == 0);
		CHECK(buf.position() == 2);

		buf.clear();
		CHECK(buf.limit() == 3);
		CHECK(buf.capacity() == 3);
		CHECK(buf.remaining() == 3);
		CHECK(buf.position() == 0);
	}

	TEST_CASE("duplicate()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c'});
		buf.position(1);
		buf.limit(2);

		auto dup = buf.duplicate();
		CHECK(buf.position() == dup.position());
		CHECK(buf.capacity() == dup.capacity());
		CHECK(buf.remaining() == dup.remaining());
		CHECK(buf.limit() == dup.limit());
	}

	TEST_CASE("flip()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c'});
		buf.position(1);
		buf.flip();

		CHECK(buf.limit() == 1);
		CHECK(buf.capacity() == 3);
		CHECK(buf.remaining() == 1);
		CHECK(buf.position() == 0);
	}

	TEST_CASE("slice()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c', 'd'});

		SUBCASE("fully") {
			buf.position(2);

			auto slice = buf.slice();
			CHECK(slice.position() == 0);
			CHECK(slice.capacity() == 2);
			CHECK(slice.limit() == 2);
			CHECK(slice.remaining() == 2);

			CHECK(buf.position() == 2);
		}

		SUBCASE("partially") {
			auto slice = buf.slice(1, 2);
			CHECK(slice.position() == 0);
			CHECK(slice.capacity() == 2);
			CHECK(slice.limit() == 2);
			CHECK(slice.remaining() == 2);

			CHECK(buf.position() == 0);
		}
	}

	TEST_CASE("array()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b', 'c', 'd'});
		buf.limit(3);

		auto array = buf.array();
		CHECK(array.size() == 3);
		CHECK(std::equal(array.begin(), array.end(), "abc"));
	}

	TEST_CASE("get()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'\x1A', 0xA1, 'c', 'd'});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get() == 0x1A);
			CHECK(buf.position() == 1);

			CHECK(buf.get() == 0xA1);
			CHECK(buf.position() == 2);

			std::array<std::uint8_t, 2> array {};
			buf.get({array.begin(), array.end()});

			CHECK(buf.position() == 4);
			CHECK(buf.remaining() == 0);
			CHECK(std::equal(array.begin(), array.end(), "cd"));

			CHECK_THROWS((void) buf.get());
			CHECK_THROWS(buf.get(array));
		}

		SUBCASE("absolute") {
			CHECK(buf.get(1) == 0xA1);
			CHECK(buf.position() == 0);

			std::array<std::uint8_t, 2> array {};
			buf.get(2, array);

			CHECK(buf.position() == 0);
			CHECK(std::equal(array.begin(), array.end(), "cd"));

			CHECK_THROWS((void) buf.get(4));
			CHECK_THROWS((void) buf.get(3, array));
		}
	}

	TEST_CASE("get_char()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {'a', 'b'});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_char() == 'a');
			CHECK(buf.position() == 1);

			CHECK(buf.get_char() == 'b');
			CHECK(buf.position() == 2);

			CHECK_THROWS((void) buf.get_char());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_char(1) == 'b');
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_char(2));
		}
	}

	TEST_CASE("get_short()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {0xFF, 0xFF, 0x01, 0x00, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_short() == -1);
			CHECK(buf.position() == 2);

			CHECK(buf.get_short() == 1);
			CHECK(buf.position() == 4);

			CHECK_THROWS((void) buf.get_short());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_short(2) == 1);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_short(4));
		}
	}

	TEST_CASE("get_ushort()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {0xFF, 0xFF, 0x01, 0x00, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_ushort() == 0xFF'FF);
			CHECK(buf.position() == 2);

			CHECK(buf.get_ushort() == 1);
			CHECK(buf.position() == 4);

			CHECK_THROWS((void) buf.get_short());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_ushort(2) == 1);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_ushort(4));
		}
	}

	TEST_CASE("get_int()") {
		auto buf = phoenix::buffer::wrap(
		    std::vector<std::uint8_t> {0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_int() == -1);
			CHECK(buf.position() == 4);

			CHECK(buf.get_int() == 1);
			CHECK(buf.position() == 8);

			CHECK_THROWS((void) buf.get_int());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_int(4) == 1);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_int(8));
		}
	}

	TEST_CASE("get_uint()") {
		auto buf = phoenix::buffer::wrap(
		    std::vector<std::uint8_t> {0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_uint() == 0xFFFF'FFFF);
			CHECK(buf.position() == 4);

			CHECK(buf.get_uint() == 1);
			CHECK(buf.position() == 8);

			CHECK_THROWS((void) buf.get_uint());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_uint(4) == 1);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_uint(8));
		}
	}

	TEST_CASE("get_long()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		                                                            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		                                                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_long() == -1);
			CHECK(buf.position() == 8);

			CHECK(buf.get_long() == 1);
			CHECK(buf.position() == 16);

			CHECK_THROWS((void) buf.get_long());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_long(8) == 1);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_long(16));
		}
	}

	TEST_CASE("get_ulong()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		                                                            0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		                                                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_ulong() == 0xFFFFFFFF'FFFFFFFFL);
			CHECK(buf.position() == 8);

			CHECK(buf.get_ulong() == 1);
			CHECK(buf.position() == 16);

			CHECK_THROWS((void) buf.get_ulong());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_ulong(8) == 1);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_ulong(16));
		}
	}

	TEST_CASE("get_float()") {

		auto buf = phoenix::buffer::wrap(
		    std::vector<std::uint8_t> {0x52, 0x58, 0xD2, 0x43, 0x0A, 0xD7, 0x8A, 0xC2, 0xFF, 0xFF, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_float() == 420.69f);
			CHECK(buf.position() == 4);

			CHECK(buf.get_float() == -69.420f);
			CHECK(buf.position() == 8);

			CHECK_THROWS((void) buf.get_float());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_float(4) == -69.420f);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_float(8));
		}
	}

	TEST_CASE("get_double()") {
		auto buf = phoenix::buffer::wrap(std::vector<std::uint8_t> {0xD7, 0xA3, 0x70, 0x3D, 0x0A, 0x4B, 0x7A, 0x40,
		                                                            0x7B, 0x14, 0xAE, 0x47, 0xE1, 0x5A, 0x51, 0xC0,
		                                                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
		CHECK(buf.position() == 0);

		SUBCASE("relative") {
			CHECK(buf.get_double() == 420.69);
			CHECK(buf.position() == 8);

			CHECK(buf.get_double() == -69.420);
			CHECK(buf.position() == 16);

			CHECK_THROWS((void) buf.get_double());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_double(8) == -69.420);
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_double(16));
		}
	}

	TEST_CASE("get_string()") {
		auto buf = phoenix::buffer::wrap(
		    std::vector<std::uint8_t> {'H', 'i', 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'});

		SUBCASE("relative") {
			CHECK(buf.get_string(2) == "Hi");
			CHECK(buf.position() == 2);

			CHECK(buf.get_string(13) == "Hello, World!");
			CHECK(buf.position() == 15);

			CHECK_THROWS((void) buf.get_string(1));
		}

		SUBCASE("absolute") {
			CHECK(buf.get_string(2, 13) == "Hello, World!");
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_string(14, 2));
		}
	}

	TEST_CASE("get_line()") {
		auto buf =
		    phoenix::buffer::wrap(std::vector<std::uint8_t> {'H', 'i', '\n', ' ', ' ', '\r', '\t', 'H', 'e', 'l', 'l',
		                                                     'o', ',', ' ',  'W', 'o', 'r',  'l',  'd', '!', '\n'});

		SUBCASE("relative") {
			CHECK(buf.get_line(true) == "Hi");
			CHECK(buf.position() == 7);

			CHECK(buf.get_line(true) == "Hello, World!");
			CHECK(buf.position() == 21);

			CHECK_THROWS((void) buf.get_line());
		}

		SUBCASE("absolute") {
			CHECK(buf.get_line_at(1) == "i");
			CHECK(buf.position() == 0);

			CHECK_THROWS((void) buf.get_line_at(21));
		}
	}

	TEST_CASE("get_vec2()") {
		// TODO!
	}

	TEST_CASE("get_vec3()") {
		// TODO!
	}

	TEST_CASE("get_vec4()") {
		// TODO!
	}

	TEST_CASE("mismatch()") {
		auto buf = phoenix::buffer::wrap(
		    std::vector<std::uint8_t> {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n'});

		SUBCASE("relative functional") {
			buf.position(5);

			auto index = buf.mismatch([](char chr) { return chr == 'W'; });
			CHECK(index == 2);
			CHECK(buf.position() == 5);

			index = buf.mismatch([](char) { return false; });
			CHECK(index == -1);
		}

		SUBCASE("relative buffer") {
			auto comp = phoenix::buffer::wrap(std::vector<std::uint8_t> {'W', 'o', 'r', 'l', 'd', '.'});
			auto index = buf.mismatch(comp);
			CHECK(index == 0);
			CHECK(buf.position() == 0);
			CHECK(comp.position() == 0);

			buf.position(7);
			index = buf.mismatch(comp);
			CHECK(index == 5);
			CHECK(buf.position() == 7);
			CHECK(comp.position() == 0);

			comp.limit(comp.limit() - 1);
			index = buf.mismatch(comp);
			CHECK(index == -1);
			CHECK(buf.position() == 7);
			CHECK(comp.position() == 0);
		}

		SUBCASE("absolute functional") {
			auto index = buf.mismatch(5, [](char chr) { return chr == 'W'; });
			CHECK(index == 2);
			CHECK(buf.position() == 0);

			index = buf.mismatch(7, [](char) { return false; });
			CHECK(index == -1);
		}

		SUBCASE("absolute buffer") {
			auto comp = phoenix::buffer::wrap(std::vector<std::uint8_t> {'W', 'o', 'r', 'l', 'd', '.'});
			auto index = buf.mismatch(0, comp);
			CHECK(index == 0);
			CHECK(buf.position() == 0);
			CHECK(comp.position() == 0);

			index = buf.mismatch(7, comp);
			CHECK(index == 5);
			CHECK(buf.position() == 0);
			CHECK(comp.position() == 0);

			comp.limit(comp.limit() - 1);
			index = buf.mismatch(7, comp);
			CHECK(index == -1);
			CHECK(buf.position() == 0);
			CHECK(comp.position() == 0);
		}
	}

	TEST_CASE("put*()") {
		auto buf = phoenix::buffer::allocate(57);

		buf.put(0xFF)
		    .put_short(-16)
		    .put_ushort(16)
		    .put_int(-16)
		    .put_uint(16)
		    .put_long(-16)
		    .put_ulong(16)
		    .put_float(69.420f)
		    .put_double(420.69)
		    .put_string("Hi")
		    .put_line("Hello, World!")
		    .flip();

		CHECK(buf.limit() == 57);
		CHECK(buf.get() == 0xFF);
		CHECK(buf.get_short() == -16);
		CHECK(buf.get_ushort() == 16);
		CHECK(buf.get_int() == -16);
		CHECK(buf.get_uint() == 16);
		CHECK(buf.get_long() == -16);
		CHECK(buf.get_ulong() == 16);
		CHECK(buf.get_float() == 69.420f);
		CHECK(buf.get_double() == 420.69);
		CHECK(buf.get_string(2) == "Hi");
		CHECK(buf.get_line() == "Hello, World!");

		CHECK(buf.remaining() == 0);
	}

	TEST_CASE("empty()") {
		auto empty = phoenix::buffer::empty();
		CHECK(empty.limit() == 0);
		CHECK(empty.capacity() == 0);
		CHECK(empty.remaining() == 0);
		CHECK(empty.position() == 0);

		auto buf = phoenix::buffer::allocate(10);
		CHECK_FALSE(empty == buf);
	}
}
