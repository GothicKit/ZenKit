// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/messages.hh>

TEST_SUITE("messages") {
	TEST_CASE("messages(parse:g1)") {
		auto buf = phoenix::buffer::mmap("./samples/ou.proprietary.bin");
		auto msgs = phoenix::messages::parse(buf);

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

	TEST_CASE("messages(parse:g2)" * doctest::skip()) {
		// TODO: Stub
	}
}
