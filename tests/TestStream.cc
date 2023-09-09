// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Stream.hh"

#include <doctest/doctest.h>

template <typename... Args>
static std::vector<std::byte> bytes(Args... bytes) {
	return std::vector<std::byte> {static_cast<std::byte>(bytes)...};
}

TEST_SUITE("Read") {
	TEST_CASE("Read.seek") {
		auto r = zenkit::Read::from(bytes('a', 'b', 'c', 'd'));

		r->seek(1, zenkit::Whence::BEG);
		CHECK_EQ(r->read_char(), 'b');

		r->seek(-1, zenkit::Whence::CUR);
		CHECK_EQ(r->read_char(), 'b');

		r->seek(-1, zenkit::Whence::END);
		CHECK_EQ(r->read_char(), 'd');
	}

	TEST_CASE("Read.read_byte") {
		auto r = zenkit::Read::from(bytes(0x1F, 0xFF));
		CHECK_EQ(r->read_byte(), 0x1F);
		CHECK_EQ(r->read_byte(), -1);

		CHECK(r->eof());
		CHECK_EQ(r->read_byte(), 0);
	}

	TEST_CASE("Read.read_ubyte") {
		auto r = zenkit::Read::from(bytes(0x1F, 0xFF));
		CHECK_EQ(r->read_ubyte(), 0x1F);
		CHECK_EQ(r->read_ubyte(), 0xFF);

		CHECK(r->eof());
		CHECK_EQ(r->read_ubyte(), 0);
	}

	TEST_CASE("Read.read_short") {
		auto r = zenkit::Read::from(bytes(0xFF, 0xFF, 0x01, 0x00, 0xFF));
		CHECK_EQ(r->read_short(), -1);
		CHECK_EQ(r->read_short(), 1);

		CHECK_FALSE(r->eof());
		CHECK_EQ(r->read_short(), 0x00FF);

		CHECK(r->eof());
		CHECK_EQ(r->read_short(), 0);
	}

	TEST_CASE("Read.read_ushort") {
		auto r = zenkit::Read::from(bytes(0xFF, 0xFF, 0x01, 0x00, 0xFF));
		CHECK_EQ(r->read_ushort(), 0xFFFF);
		CHECK_EQ(r->read_ushort(), 1);

		CHECK_FALSE(r->eof());
		CHECK_EQ(r->read_ushort(), 0x00FF);

		CHECK(r->eof());
		CHECK_EQ(r->read_ushort(), 0);
	}

	TEST_CASE("Read.read_int") {
		auto r = zenkit::Read::from(bytes(0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF));
		CHECK_EQ(r->read_int(), -1);
		CHECK_EQ(r->read_int(), 1);

		CHECK_FALSE(r->eof());
		CHECK_EQ(r->read_int(), 0x00FF'FFFF);

		CHECK(r->eof());
		CHECK_EQ(r->read_int(), 0);
	}

	TEST_CASE("Read.read_uint") {
		auto r = zenkit::Read::from(bytes(0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF));
		CHECK_EQ(r->read_uint(), 0xFFFF'FFFF);
		CHECK_EQ(r->read_uint(), 1);

		CHECK_FALSE(r->eof());
		CHECK_EQ(r->read_uint(), 0x00FF'FFFF);

		CHECK(r->eof());
		CHECK_EQ(r->read_uint(), 0);
	}

	TEST_CASE("Read.read_float") {
		auto r = zenkit::Read::from(bytes(0x52, 0x58, 0xD2, 0x43, 0x0A, 0xD7, 0x8A, 0xC2, 0xFF, 0xFF, 0xFF));
		CHECK_EQ(r->read_float(), 420.69f);
		CHECK_EQ(r->read_float(), -69.420f);

		CHECK_FALSE(r->eof());
		CHECK_EQ(r->read_float(), 2.35098856151e-38f);

		CHECK(r->eof());
		CHECK_EQ(r->read_float(), 0.f);
	}

	TEST_CASE("Read.read_string") {
		auto r = zenkit::Read::from(bytes('H', 'i', 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'));
		CHECK_EQ(r->read_string(2), "Hi");
		CHECK_EQ(r->read_string(13), "Hello, World!");

		CHECK(r->eof());
		CHECK_EQ(r->read_string(1), std::string_view {"\0", 1});
	}

	TEST_CASE("Read.read_string") {
		auto r = zenkit::Read::from(bytes('H',
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
		CHECK_EQ(r->read_line(true), "Hi");
		CHECK_EQ(r->tell(), 7);

		CHECK_EQ(r->read_line(true), "Hello,\\tWorld!");

		CHECK(r->eof());
		CHECK(r->read_line(true).empty());
	}
}

TEST_SUITE("Write") {
	static std::vector<std::byte> BUF {};

	TEST_CASE("Write.write_char") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_char('a');
		w->write_char('b');

		CHECK_EQ(BUF[0], std::byte {'a'});
		CHECK_EQ(BUF[1], std::byte {'b'});
	}

	TEST_CASE("Write.write_byte") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_byte(-1);
		w->write_byte(1);

		CHECK_EQ(BUF[0], std::byte {0xFF});
		CHECK_EQ(BUF[1], std::byte {0x01});
	}

	TEST_CASE("Write.write_ubyte") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_ubyte(0xFF);
		w->write_ubyte(1);

		CHECK_EQ(BUF[0], std::byte {0xFF});
		CHECK_EQ(BUF[1], std::byte {0x01});
	}

	TEST_CASE("Write.write_short") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_short(-1);
		w->write_short(1);

		CHECK_EQ(BUF[0], std::byte {0xFF});
		CHECK_EQ(BUF[1], std::byte {0xFF});
		CHECK_EQ(BUF[2], std::byte {0x01});
		CHECK_EQ(BUF[3], std::byte {0x00});
	}

	TEST_CASE("Write.write_ushort") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_ushort(0xFFFF);
		w->write_ushort(1);

		CHECK_EQ(BUF[0], std::byte {0xFF});
		CHECK_EQ(BUF[1], std::byte {0xFF});
		CHECK_EQ(BUF[2], std::byte {0x01});
		CHECK_EQ(BUF[3], std::byte {0x00});
	}

	TEST_CASE("Write.write_int") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_int(-1);
		w->write_int(1);

		CHECK_EQ(BUF[0], std::byte {0xFF});
		CHECK_EQ(BUF[1], std::byte {0xFF});
		CHECK_EQ(BUF[2], std::byte {0xFF});
		CHECK_EQ(BUF[3], std::byte {0xFF});

		CHECK_EQ(BUF[4], std::byte {0x01});
		CHECK_EQ(BUF[5], std::byte {0x00});
		CHECK_EQ(BUF[6], std::byte {0x00});
		CHECK_EQ(BUF[7], std::byte {0x00});
	}

	TEST_CASE("Write.write_uint") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_uint(0xFFFF'FFFF);
		w->write_uint(1);

		CHECK_EQ(BUF[0], std::byte {0xFF});
		CHECK_EQ(BUF[1], std::byte {0xFF});
		CHECK_EQ(BUF[2], std::byte {0xFF});
		CHECK_EQ(BUF[3], std::byte {0xFF});

		CHECK_EQ(BUF[4], std::byte {0x01});
		CHECK_EQ(BUF[5], std::byte {0x00});
		CHECK_EQ(BUF[6], std::byte {0x00});
		CHECK_EQ(BUF[7], std::byte {0x00});
	}

	TEST_CASE("Write.write_float") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_float(420.69f);
		w->write_float(69.420f);

		CHECK_EQ(BUF[0], std::byte {0x52});
		CHECK_EQ(BUF[1], std::byte {0x58});
		CHECK_EQ(BUF[2], std::byte {0xD2});
		CHECK_EQ(BUF[3], std::byte {0x43});

		CHECK_EQ(BUF[4], std::byte {0x0A});
		CHECK_EQ(BUF[5], std::byte {0xD7});
		CHECK_EQ(BUF[6], std::byte {0x8A});
		CHECK_EQ(BUF[7], std::byte {0x42});
	}

	TEST_CASE("Write.write_string") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_string("Hello!");

		CHECK_EQ(BUF[0], std::byte {'H'});
		CHECK_EQ(BUF[1], std::byte {'e'});
		CHECK_EQ(BUF[2], std::byte {'l'});
		CHECK_EQ(BUF[3], std::byte {'l'});
		CHECK_EQ(BUF[4], std::byte {'o'});
		CHECK_EQ(BUF[5], std::byte {'!'});
	}

	TEST_CASE("Write.write_line") {
		auto w = zenkit::Write::to(&BUF);
		BUF.clear();

		w->write_line("Hello!");

		CHECK_EQ(BUF[0], std::byte {'H'});
		CHECK_EQ(BUF[1], std::byte {'e'});
		CHECK_EQ(BUF[2], std::byte {'l'});
		CHECK_EQ(BUF[3], std::byte {'l'});
		CHECK_EQ(BUF[4], std::byte {'o'});
		CHECK_EQ(BUF[5], std::byte {'!'});
		CHECK_EQ(BUF[6], std::byte {'\n'});
	}
}
