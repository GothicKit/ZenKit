// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/messages.hh>

TEST_SUITE("messages") {
	TEST_CASE("message databases are read correctly") {
		auto buf = phoenix::buffer::mmap("./samples/ou.proprietary.bin");
		auto msgs = phoenix::messages::parse(buf);

		CHECK(msgs.blocks.size() == 7360);

		auto* msg20 = msgs.block_by_name("DIA_ARTO_PERM_15_00");
		auto* msg100 = msgs.block_by_name("DIA_BaalKagan_WasDrin_13_01");
		auto* msg200 = &msgs.blocks[200];
		auto* msg_none = msgs.block_by_name("nonexistent");

		CHECK(msg20 != nullptr);
		CHECK(msg100 != nullptr);
		CHECK(msg200 != nullptr);
		CHECK(msg_none == nullptr);

		CHECK(msg20->message.type == 0);
		CHECK(msg20->message.text == "Du redest nicht viel, was?");
		CHECK(msg20->message.name == "DIA_ARTO_PERM_15_00.WAV");

		CHECK(msg100->message.type == 0);
		CHECK(msg100->message.text == "Ich kann dich auf viele Arten entlohnen.");
		CHECK(msg100->message.name == "DIA_BAALKAGAN_WASDRIN_13_01.WAV");

		CHECK(msg200->message.type == 0);
		CHECK(msg200->message.text == "Stimmt genau.");
		CHECK(msg200->message.name == "DIA_BAALTARAN_INTOCASTLE_EXACTLY_15_00.WAV");
	}
}
