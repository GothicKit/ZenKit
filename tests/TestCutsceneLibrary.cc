// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/CutsceneLibrary.hh>
#include <zenkit/Stream.hh>

#include <fstream>

static void verify_g1(zenkit::CutsceneLibrary const& msgs) {
	CHECK_EQ(msgs.blocks.size(), 7360);

	auto msg20 = msgs.block_by_name("DIA_ARTO_PERM_15_00");
	auto msg100 = msgs.block_by_name("DIA_BaalKagan_WasDrin_13_01");
	auto msg200 = msgs.blocks[200];
	auto msg_none = msgs.block_by_name("nonexistent");

	CHECK_NE(msg20, nullptr);
	CHECK_NE(msg100, nullptr);
	CHECK_NE(msg200, nullptr);
	CHECK_EQ(msg_none, nullptr);

	auto message = msg20->get_message();
	CHECK_EQ(message->type, 0);
	CHECK_EQ(message->text, "Du redest nicht viel, was?");
	CHECK_EQ(message->name, "DIA_ARTO_PERM_15_00.WAV");

	message = msg100->get_message();
	CHECK_EQ(message->type, 0);
	CHECK_EQ(message->text, "Ich kann dich auf viele Arten entlohnen.");
	CHECK_EQ(message->name, "DIA_BAALKAGAN_WASDRIN_13_01.WAV");

	message = msg200->get_message();
	CHECK_EQ(message->type, 0);
	CHECK_EQ(message->text, "Stimmt genau.");
	CHECK_EQ(message->name, "DIA_BAALTARAN_INTOCASTLE_EXACTLY_15_00.WAV");
}

TEST_SUITE("CutsceneLibrary") {
	TEST_CASE("CutsceneLibrary.load(GOTHIC1)") {
		auto r = zenkit::Read::from("./samples/ou.proprietary.bin");
		auto ar = zenkit::ReadArchive::from(r.get());
		auto msgs = ar->read_object<zenkit::CutsceneLibrary>(zenkit::GameVersion::GOTHIC_1);
		verify_g1(*msgs);
	}

	TEST_CASE("CutsceneLibrary.dump(GOTHIC1)") {
		auto r = zenkit::Read::from("./samples/ou.proprietary.bin");
		auto ar = zenkit::ReadArchive::from(r.get());
		auto msgs_tmp = ar->read_object<zenkit::CutsceneLibrary>(zenkit::GameVersion::GOTHIC_1);

		std::vector<std::byte> data;
		auto w = zenkit::Write::to(&data);
		auto aw = zenkit::WriteArchive::to(w.get(), zenkit::ArchiveFormat::BINARY);
		aw->write_object(msgs_tmp, zenkit::GameVersion::GOTHIC_1);

		r = zenkit::Read::from(&data);
		ar = zenkit::ReadArchive::from(r.get());
		auto msgs = ar->read_object<zenkit::CutsceneLibrary>(zenkit::GameVersion::GOTHIC_1);

		verify_g1(*msgs);
	}

	TEST_CASE("CutsceneLibrary.load(GOTHIC2)" * doctest::skip()) {
		// TODO: Stub
	}
}
