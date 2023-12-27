// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/CutsceneLibrary.hh>
#include <zenkit/Stream.hh>

#include <fstream>

TEST_SUITE("CutsceneLibrary") {
	TEST_CASE("CutsceneLibrary.load(GOTHIC1)") {
		auto r = zenkit::Read::from("./samples/ou.proprietary.bin");
		zenkit::CutsceneLibrary msgs {};
		msgs.load(r.get());

		CHECK_EQ(msgs.blocks.size(), 7360);

		auto* msg20 = msgs.block_by_name("DIA_ARTO_PERM_15_00");
		auto* msg100 = msgs.block_by_name("DIA_BaalKagan_WasDrin_13_01");
		auto* msg200 = &msgs.blocks[200];
		auto* msg_none = msgs.block_by_name("nonexistent");

		CHECK_NE(msg20, nullptr);
		CHECK_NE(msg100, nullptr);
		CHECK_NE(msg200, nullptr);
		CHECK_EQ(msg_none, nullptr);

		CHECK_EQ(msg20->message.type, 0);
		CHECK_EQ(msg20->message.text, "Du redest nicht viel, was?");
		CHECK_EQ(msg20->message.name, "DIA_ARTO_PERM_15_00.WAV");

		CHECK_EQ(msg100->message.type, 0);
		CHECK_EQ(msg100->message.text, "Ich kann dich auf viele Arten entlohnen.");
		CHECK_EQ(msg100->message.name, "DIA_BAALKAGAN_WASDRIN_13_01.WAV");

		CHECK_EQ(msg200->message.type, 0);
		CHECK_EQ(msg200->message.text, "Stimmt genau.");
		CHECK_EQ(msg200->message.name, "DIA_BAALTARAN_INTOCASTLE_EXACTLY_15_00.WAV");
	}

	TEST_CASE("CutsceneLibrary.dump(GOTHIC1)") {
		auto r = zenkit::Read::from("./samples/ou.proprietary.bin");
		zenkit::CutsceneLibrary msgs_tmp {};
		msgs_tmp.load(r.get());

		std::vector<std::byte> data;
		auto w = zenkit::Write::to(&data);
		msgs_tmp.save(w.get(), zenkit::ArchiveFormat::BINARY);

		r = zenkit::Read::from(&data);
		zenkit::CutsceneLibrary msgs {};
		msgs.load(r.get());

		CHECK_EQ(msgs.blocks.size(), 7360);

		auto* msg20 = msgs.block_by_name("DIA_ARTO_PERM_15_00");
		auto* msg100 = msgs.block_by_name("DIA_BaalKagan_WasDrin_13_01");
		auto* msg200 = &msgs.blocks[200];
		auto* msg_none = msgs.block_by_name("nonexistent");

		CHECK_NE(msg20, nullptr);
		CHECK_NE(msg100, nullptr);
		CHECK_NE(msg200, nullptr);
		CHECK_EQ(msg_none, nullptr);

		CHECK_EQ(msg20->message.type, 0);
		CHECK_EQ(msg20->message.text, "Du redest nicht viel, was?");
		CHECK_EQ(msg20->message.name, "DIA_ARTO_PERM_15_00.WAV");

		CHECK_EQ(msg100->message.type, 0);
		CHECK_EQ(msg100->message.text, "Ich kann dich auf viele Arten entlohnen.");
		CHECK_EQ(msg100->message.name, "DIA_BAALKAGAN_WASDRIN_13_01.WAV");

		CHECK_EQ(msg200->message.type, 0);
		CHECK_EQ(msg200->message.text, "Stimmt genau.");
		CHECK_EQ(msg200->message.name, "DIA_BAALTARAN_INTOCASTLE_EXACTLY_15_00.WAV");
	}

	TEST_CASE("CutsceneLibrary.load(GOTHIC2)" * doctest::skip()) {
		// TODO: Stub
	}
}
